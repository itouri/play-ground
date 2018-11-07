#include "sgx_eid.h"
#include "error_codes.h"
#include "datatypes.h"
#include "sgx_urts.h"
#include "sgx_dh.h"
#include <string.h>

#include "../MasterEnclave/MasterEnclave_u.h"
#include "../LibEnclave/LibEnclave_u.h"

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>

#define FILE_PATH "./unix_domain.uds"

#include "error_codes.h"

#include <map>

sgx_enclave_id_t master_enclave_id;
sgx_enclave_id_t app_enclave_id;

std::map<sgx_enclave_id_t, uint32_t>g_enclave_id_map;

#define MASTER_ENC_PATH "libMasterenclave.so"
#define LIB_ENC_PATH "libLibenclave.so"

//LIB
ATTESTATION_STATUS session_request_ocall(sgx_enclave_id_t src_enclave_id, sgx_enclave_id_t dest_enclave_id, sgx_dh_msg1_t* dh_msg1, uint32_t* session_id)
{
	uint32_t status = 0;
	sgx_status_t ret = SGX_SUCCESS;

	ret = session_request(dest_enclave_id, &status, src_enclave_id, dh_msg1, session_id);
	if (ret == SGX_SUCCESS)
		return (ATTESTATION_STATUS)status;
	else	
	    return INVALID_SESSION;

}

//Makes an sgx_ecall to the destination enclave sends message2 from the source enclave and gets message 3 from the destination enclave
ATTESTATION_STATUS exchange_report_ocall(sgx_enclave_id_t src_enclave_id, sgx_enclave_id_t dest_enclave_id, sgx_dh_msg2_t *dh_msg2, sgx_dh_msg3_t *dh_msg3, uint32_t session_id)
{
	uint32_t status = 0;
	sgx_status_t ret = SGX_SUCCESS;


	ret = exchange_report(dest_enclave_id, &status, src_enclave_id, dh_msg2, dh_msg3, session_id);
	if (ret == SGX_SUCCESS)
		return (ATTESTATION_STATUS)status;
	else	
	    return INVALID_SESSION;

}

uint32_t load_app_enclave(image_id_t image_id, sgx_enclave_id_t * app_enclave_id)
{
    uint32_t enclave_temp_no;
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    char * so_path = "./images/";
    strcat(so_path, (const char*)&image_id);
    strcat(so_path, "/emain.so");

    ret = sgx_create_enclave(so_path, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, app_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
        printf("cant app sgx_create_enclave() so_path = %s", so_path);
        return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(*app_enclave_id, enclave_temp_no));

    return SGX_SUCCESS;
}

uint32_t load_master_enclave()
{
    uint32_t enclave_temp_no;
    int ret, launch_token_updated;
    sgx_launch_token_t launch_token;

    enclave_temp_no = 0;

    ret = sgx_create_enclave(MASTER_ENC_PATH, SGX_DEBUG_FLAG, &launch_token, &launch_token_updated, &master_enclave_id, NULL);
    if (ret != SGX_SUCCESS) {
        return ret;
    }

    enclave_temp_no++;
    g_enclave_id_map.insert(std::pair<sgx_enclave_id_t, uint32_t>(master_enclave_id, enclave_temp_no));

    return SGX_SUCCESS;
}

void serve(int remote_fd) {
    image_id_t image_id;
    uint8_t * image_metadata;
    uint8_t * create_req_metadata;

    uint8_t buf[1024]; //TODO サイズを決める
    memset(buf, 0, sizeof buf);

    int n;
    n = read(remote_fd, buf, sizeof buf);
    if (n < 0) {
        perror("read length is zero");
        return;
    }
    memcpy(&image_id, buf, sizeof(image_id_t));

    size_t offset = (sizeof(image_id_t));

    uint32_t image_metadata_size;
    uint32_t create_req_metadata_size;

    memcpy(&image_metadata_size, &buf[offset], sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&create_req_metadata_size, &buf[offset], sizeof(uint32_t));
    offset += sizeof(uint32_t);

    image_metadata = (uint8_t*)malloc(image_metadata_size);
    memcpy(image_metadata, (const void*)&buf[offset], image_metadata_size);

    offset += image_metadata_size;

    create_req_metadata = (uint8_t*)malloc(create_req_metadata_size);
    memcpy(create_req_metadata, (const void*)&buf[offset], create_req_metadata_size);

    //print_hex(buf, n);
    printf("image_id_t size: %ld\n", sizeof(image_id_t));
    printf("image_metadata_size: %d\n", image_metadata_size);
    printf("create_req_metadata_size: %d\n", create_req_metadata_size);

    sgx_enclave_id_t app_enclave_id;

    /* create app enclave */
    if(load_app_enclave(image_id, &app_enclave_id) != SGX_SUCCESS)
    {
        printf("Load App Enclave Failure\n");
    }

    uint32_t ret_status;
    sgx_status_t status;

    status = create_session(master_enclave_id,
                            &ret_status,
                            master_enclave_id,
                            app_enclave_id,
                            image_metadata,
                            image_metadata_size,
                            create_req_metadata,
                            create_req_metadata_size);
                        
    if (status != SGX_SUCCESS) {
        printf("create_session is failed on sgx: Error code is %x\n", status);
        return;
    }
    if (ret_status != 0) {
        printf("Session establishment and key exchange failure : Error code is %x\n", ret_status);
        return;
    }
    printf("Secure Channel Establishment successful !!!\n");

    // app_enclave の main を実行
    // do_main(app_enclave_id);

    //printf("sgx_enclave_id: %s\n", image_id);

    // print_hex((uint8_t*)&image_id, sizeof(image_id_t));
    // print_hex(image_metadata, image_metadata_size);
    // print_hex(create_req_metadata, create_req_metadata_size);
}

void run_server()
{
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;
    uint8_t buf[1024];

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, FILE_PATH);
    unlink(local.sun_path);
    r = bind(listen_fd, (struct sockaddr *)&local, sizeof(local));
    if (r)
        perror("failed to bind");

    listen(listen_fd, 100);
    for (;;) {
        socklen_t len = sizeof(remote);
        int remote_fd = accept(listen_fd, (struct sockaddr *)&remote, &len);
        if (remote_fd < 0) {
            perror("failed to accept");
            return;
        }
        serve(remote_fd);
        close(remote_fd);
    }
    close(listen_fd);
}

int main (int argc, char * argv[])
{
    if(load_master_enclave() != SGX_SUCCESS)
    {
        printf("\nLoad Master Enclave Failure");
    }
    run_server();
    return 0;
}