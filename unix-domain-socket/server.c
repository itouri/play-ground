#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>

int main()
{
    int r;
    int listen_fd = 0;
    struct sockaddr_un local, remote;

    uint8_t buf[1024];

    signal(SIGPIPE, SIG_IGN);
    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, "test.uds");
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
            return 0;
        }
        for (;;) {
            int sent = write(remote_fd, buf, 20);
            printf("sent %d bytes\n", sent);
            if (sent < 0) {
                printf("%d %s\n", errno, strerror(errno));
                perror("fail to send");
                close(remote_fd);
                break;
            }
        }
    }
}