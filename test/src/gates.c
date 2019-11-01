#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cqc.h"
#include "test.h"


/*
 *  cqc_tomography_dir
 *
 *  Obtain tomographic data for a prepared qubit, for testing purposes.
 *
 *  Arguments:
 *  func    function to call to prepare qubit for tomography
 *  iter    iterations to perform
 *  dir     direction to measure (0=Z, 1=X, 2=Y)
 *
 */

double cqc_tomography_dir(cqc_ctx *cqc,
                          uint16_t (*func)(cqc_ctx *),
                          uint32_t iter,
                          uint8_t dir)
{
    /* Translate the direction into a rotation command */
    uint8_t cmd;
    switch(dir) {
    case 1:
        cmd = CQC_CMD_H;
        break;
    case 2:
        cmd = CQC_CMD_K;
        break;
    }

    /* Measure in the given direction iter times to gather stats */
    int count = 0;
    for(int _ = 0; _ < iter; ++_) {

        /* Prepare the qubit */
        uint16_t qubit = func(cqc);

        /* Measure in the indicated direction */
        if (dir > 0) {
            ASSERT_CQC_CALL(cqc_simple_cmd(cqc, cmd, qubit, true));
            ASSERT_CQC_CALL(cqc_wait_until_done(cqc, 1));
        }

        uint8_t outcome;
        ASSERT_CQC_CALL(cqc_measure(cqc, qubit, &outcome));

        /* Add to the total count: note outcome needs to be +/-1 for 0/1 to
         * yield expectations */
        if (outcome == 0) {
            count++;
        } else {
            count--;
        }
    }

    return ((double)count)/((double)iter);
}

/*
 * cqc_test_qubit
 *
 * Prepares a qubit according to the indicated function, then performs
 * tomography to verify the preparation up to the indicated precision.
 *
 * Arguments
 * cqc        cqc connection
 * func       function to invoke to prepare qubit
 * iter       number of times to iterate the test in tomography
 * epsilon    desired precision
 * exp_x      expected value for <X>
 * exp_y      expected value for <Y>
 * exp_z      expected value for <Z>
 *
 * Returns
 * 0     for success - state lies in desired interval
 * 1     for no functional failure but state does not lie in desired interval
 * -1    functional error
 */

int cqc_test_qubit(cqc_ctx *cqc,
                   uint16_t (*func)(cqc_ctx *),
                   uint32_t iter,
                   double epsilon,
                   double exp_x,
                   double exp_y,
                   double exp_z)
{

    /* Run tomography in X, Z and Y directions */
    double tomo_z = cqc_tomography_dir(cqc, func, iter, 0);
    double tomo_x = cqc_tomography_dir(cqc, func, iter, 1);
    double tomo_y = cqc_tomography_dir(cqc, func, iter, 2);

    /* Compare to the expected results up to the desired precision */
    double diff_x = fabs(tomo_x - exp_x);
    double diff_y = fabs(tomo_y - exp_y);
    double diff_z = fabs(tomo_z - exp_z);

    if (diff_x > epsilon) {
        printf("X target not met, got %f expected %f\n", tomo_x, exp_x);
        return TEST_RC_FAIL;
    }
    if (diff_z > epsilon) {
        printf("Z target not met, got %f expected %f\n", tomo_z, exp_z);
        return TEST_RC_FAIL;
    }
    if (diff_y > epsilon) {
        printf("Y target not met, got %f expected %f\n", tomo_y, exp_y);
        return TEST_RC_FAIL;
    }

    return TEST_RC_OK;
}

/*
 * make_state
 *
 * Create qubit and apply gate
 *
 */
uint16_t make_state(cqc_ctx *cqc, uint8_t gate)
{
    /* Qubit ID */
    uint16_t qubit;

    /* Create a new qubit in |0> */
    ASSERT_CQC_CALL(cqc_simple_cmd(cqc, CQC_CMD_NEW, 0, false));
    ASSERT_CQC_CALL(cqc_wait_until_newok(cqc, &qubit));

    /* Turn it into |+> */
    ASSERT_CQC_CALL(cqc_simple_cmd(cqc, gate, qubit, true));
    ASSERT_CQC_CALL(cqc_wait_until_done(cqc, 1));

    return qubit;
}

/*
 * make_plus
 *
 * Prepares a plus state
 *
 */
uint16_t make_plus(cqc_ctx *cqc)
{
    return make_state(cqc, CQC_CMD_H);
}


/*
 * make_zero
 *
 * Prepares a plus state
 *
 */
uint16_t make_zero(cqc_ctx *cqc)
{
    return make_state(cqc, CQC_CMD_I);
}

/*
 * make_k
 *
 * Prepares a y eigenstate
 *
 */
uint16_t make_k(cqc_ctx *cqc)
{
    return make_state(cqc, CQC_CMD_K);
}


int main(int argc, char *argv[]) {
    int n_iter = 100;
    int conf = 2;
    double err = ((double)conf) / sqrt((double)n_iter);

    /* Retrieve arguments from command line */
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port \n", argv[0]);
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

    /* Test whether we can make the zero state */
    printf("gates.c :: testing Z-basis preparation\n");
    ASSERT_TEST_CALL(cqc_test_qubit(cqc, make_zero, n_iter, err, 0, 0, 1));

    /* Test whether we can make the plus state */
    printf("gates.c :: testing X-basis preparation\n");
    ASSERT_TEST_CALL(cqc_test_qubit(cqc, make_plus, n_iter, err, 1, 0, 0));

    /* Test whether we can make the y 0 eigenstate */
    printf("gates.c :: testing Y-basis preparation\n");
    ASSERT_TEST_CALL(cqc_test_qubit(cqc, make_k, n_iter, err, 0, 1, 0));

    cqc_close(cqc);
    cqc_destroy(cqc);

    return 0;
}
