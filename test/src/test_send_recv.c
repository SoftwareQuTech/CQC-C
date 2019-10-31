#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include "cqc.h"
#include "test.h"

void test_send(uint16_t app_id,
               char *send_host,
               uint16_t send_port,
               char *recv_host,
               uint16_t recv_port)
{
    /* In this example, we will not check for errors. All functions return -1
     * on failure */
    cqc_ctx *cqc = cqc_init(app_id);
    assert(cqc != NULL);
    ASSERT_CQC_CALL(cqc_connect(cqc, send_host, send_port));

    uint16_t qubit;
    ASSERT_CQC_CALL(cqc_simple_cmd(cqc, CQC_CMD_NEW, 0, false));
    ASSERT_CQC_CALL(cqc_wait_until_newok(cqc, &qubit));

    /* Lookup remote host */
    struct hostent *server = gethostbyname(recv_host);
    assert(server != NULL);
    uint32_t remote_node = ntohl(*((uint32_t *)server->h_addr_list[0]));

    ASSERT_CQC_CALL(cqc_send(cqc, qubit, app_id, recv_port, remote_node));
    ASSERT_CQC_CALL(cqc_wait_until_done(cqc, 1));
}

void test_recv(uint16_t app_id, char *recv_host, uint16_t recv_port)
{
    /* In this example, we will not check for errors. All functions return -1
     * on failure */
    cqc_ctx *cqc = cqc_init(app_id);
    assert(cqc != NULL);
    ASSERT_CQC_CALL(cqc_connect(cqc, recv_host, recv_port));

    uint16_t qubit;
    ASSERT_CQC_CALL(cqc_recv(cqc, &qubit));

    printf("Received qubit %d\n", qubit);
}

int main(int argc, char *argv[]) {
    /* Retrieve arguments from command line */
    if (argc < 5) {
        fprintf(stderr,"usage %s send_host send_port recv_host recv_port\n",
                argv[0]);
        exit(1);
    }
    char *send_host = argv[1];
    uint16_t send_port = atoi(argv[2]);
    char *recv_host = argv[3];
    uint16_t recv_port = atoi(argv[4]);

    /* In this example, we are simply application 10 */
    uint16_t app_id = 10;

    test_send(app_id, send_host, send_port, recv_host, recv_port);
    test_recv(app_id, recv_host, recv_port);

    return 0;
}
