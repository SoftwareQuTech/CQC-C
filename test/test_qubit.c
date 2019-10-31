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

    /* Create qubit */
    uint16_t qubit;
    ASSERT_CQC_CALL(cqc_simple_cmd(cqc, CQC_CMD_NEW, 0, false));
    ASSERT_CQC_CALL(cqc_wait_until_newok(cqc, &qubit));

    /* Apply H gate */
    ASSERT_CQC_CALL(cqc_simple_cmd(cqc, CQC_CMD_H, qubit, true));
    ASSERT_CQC_CALL(cqc_wait_until_done(cqc, 1));

    uint8_t outcome;
    ASSERT_CQC_CALL(cqc_measure(cqc, qubit, &outcome));
    printf("Outcome: %d\n",outcome);

    return 0;
}
