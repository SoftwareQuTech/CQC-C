#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include "cqc.h"
#include "test.h"

uint8_t epr_send(uint16_t app_id,
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

    /* Lookup remote host */
    struct hostent *server = gethostbyname(recv_host);
    assert(server != NULL);
    uint32_t remote_node = ntohl(*((uint32_t *)server->h_addr_list[0]));

    /* Create EPR */
    uint16_t qubit;
    entanglementHeader ent_info;
    ASSERT_CQC_CALL(cqc_epr(cqc, app_id, recv_port, remote_node,
                            &qubit, &ent_info));

    /* Verify endianness - assume if one field is correct, they all are */
    assert(ent_info.app_id_A == app_id);

    /* Measure qubit */
    uint8_t outcome;
    ASSERT_CQC_CALL(cqc_measure(cqc, qubit, &outcome));

    cqc_close(cqc);
    cqc_destroy(cqc);

    return outcome;
}

uint8_t epr_recv(uint16_t app_id, char *recv_host, uint16_t recv_port)
{
    /* In this example, we will not check for errors. All functions return -1
     * on failure */
    cqc_ctx *cqc = cqc_init(app_id);
    assert(cqc != NULL);
    ASSERT_CQC_CALL(cqc_connect(cqc, recv_host, recv_port));

    /* Receive EPR */
    uint16_t qubit;
    entanglementHeader ent_info;
    ASSERT_CQC_CALL(cqc_epr_recv(cqc, &qubit, &ent_info));

    /* Verify endianness - assume if one field is correct, they all are */
    assert(ent_info.app_id_B == app_id);

    /* Measure qubit */
    uint8_t outcome;
    ASSERT_CQC_CALL(cqc_measure(cqc, qubit, &outcome));

    cqc_close(cqc);
    cqc_destroy(cqc);

    return outcome;
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

    /* Create the EPR pair */
    uint8_t ma = epr_send(app_id, send_host, send_port, recv_host, recv_port);
    uint8_t mb = epr_recv(app_id, recv_host, recv_port);

    /* The outcomes should be equal. */
    printf("epr.c :: qubit A outcome: %d\n", ma);
    printf("epr.c :: qubit B outcome: %d\n", mb);
    assert(ma == mb);

    return 0;
}
