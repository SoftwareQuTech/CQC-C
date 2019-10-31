#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include "cqc.h"
#include "test.h"

int main(int argc, char *argv[]) {


    /* Retrieve arguments from command line */
    if (argc < 5) {
        fprintf(stderr,"usage %s hostname port remoteHost remotePort\n",
                argv[0]);
        exit(0);
    }
    char *hostname = argv[1];
    uint16_t portno = atoi(argv[2]);
    char *remoteHost = argv[3];
    uint16_t remotePort = atoi(argv[4]);

    /* Lookup remote host */
    struct hostent *server = gethostbyname(remoteHost);
    assert(server != NULL);

    /* In this example, we are simply application 10 */
    uint16_t app_id = 10;

    /* In this example, we will not check for errors. All functions return -1
     * on failure */
    cqc_ctx *cqc = cqc_init(app_id);
    assert(cqc != NULL);
    ASSERT_CQC_CALL(cqc_connect(cqc, hostname, portno));

    uint16_t qubit;
    ASSERT_CQC_CALL(cqc_simple_cmd(cqc, CQC_CMD_NEW, 0, false));
    ASSERT_CQC_CALL(cqc_wait_until_newok(cqc, &qubit));

    uint32_t remote_node = ntohl(*((uint32_t *)server->h_addr_list[0]));
    ASSERT_CQC_CALL(cqc_send(cqc, qubit, app_id, remotePort, remote_node));
    ASSERT_CQC_CALL(cqc_wait_until_done(cqc, 1));

    return 0;
}
