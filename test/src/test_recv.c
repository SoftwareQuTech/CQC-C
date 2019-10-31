#include <stdio.h>
#include <stdlib.h>

#include "cqc.h"
#include "test.h"

int main(int argc, char *argv[]) {
    /* Retrieve arguments from command line */
    if (argc != 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    char *hostname = argv[1];
    uint16_t portno = atoi(argv[2]);

    /* In this example, we are simply application 10 */
    uint16_t app_id = 10;

    /* In this example, we will not check for errors. All functions return -1
     * on failure */
    cqc_ctx *cqc = cqc_init(app_id);
    assert(cqc != NULL);
    ASSERT_CQC_CALL(cqc_connect(cqc, hostname, portno));

    uint16_t qubit;
    ASSERT_CQC_CALL(cqc_recv(cqc, &qubit));

    printf("Received qubit %d\n", qubit);

    return 0;
}
