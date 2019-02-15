#include "sgx_eid.h"
#include "error_codes.h"
#include "sgx_dh.h"

#include "sgx_utils.h"

void print_hex(uint8_t * str, size_t size) {
    int i;
    for (i=0; i<size; i++) {
        printe("%x", str[i]);
    }
}

ATTESTATION_STATUS session_request(sgx_enclave_id_t src_enclave_id,
                          sgx_dh_msg1_t *dh_msg1,
                          uint32_t *session_id ) {
    dh_session_t session_info;
    sgx_dh_session_t sgx_dh_session;
    sgx_status_t status = SGX_SUCCESS;

    if(!session_id || !dh_msg1)
    {
        return INVALID_PARAMETER_ERROR;
    }
    //Intialize the session as a session responder
    status = sgx_dh_init_session(SGX_DH_SESSION_RESPONDER, &sgx_dh_session);
    if(SGX_SUCCESS != status)
    {
        return status;
    }
    
    //get a new SessionID
    if ((status = (sgx_status_t)generate_session_id(session_id)) != SUCCESS)
        return status; //no more sessions available

    //Allocate memory for the session id tracker
    g_session_id_tracker[*session_id] = (session_id_tracker_t *)malloc(sizeof(session_id_tracker_t));
    if(!g_session_id_tracker[*session_id])
    {
        return MALLOC_ERROR;
    }

    memset(g_session_id_tracker[*session_id], 0, sizeof(session_id_tracker_t));
    g_session_id_tracker[*session_id]->session_id = *session_id;
    session_info.status = IN_PROGRESS;

    //Generate Message1 that will be returned to Source Enclave
    status = sgx_dh_responder_gen_msg1((sgx_dh_msg1_t*)dh_msg1, &sgx_dh_session);
    if(SGX_SUCCESS != status)
    {
        SAFE_FREE(g_session_id_tracker[*session_id]);
        return status;
    }
    memcpy(&session_info.in_progress.dh_session, &sgx_dh_session, sizeof(sgx_dh_session_t));
    //Store the session information under the correspoding source enlave id key
    
    return status;
}



void ecall_get_mrenclave()
{
    sgx_report_t report;
    sgx_status_t ret;

    //printe("begin sgx_create_report\n");
    ret = sgx_create_report(NULL, NULL, &report);
    if ( ret != SGX_SUCCESS )
    {
        printe("failed sgx_create_report\n");
        return ret;
    }

    // ret = sgx_verify_report(&report);
    // if ( ret != SGX_SUCCESS )
    // {
    //     printe("failed sgx_verify_report\n");
    //     return ret;
    // }

    print_hex((char*)&report.body.mr_enclave, sizeof(sgx_measurement_t));
}