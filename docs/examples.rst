Example using the C library
===========================

The following example involves one node (localhost:8803). It is assumed that you already started the SimulaQron backend, as well as the CQC backend. Instructions on how to do this are available in the `SimulaQron docs <https://softwarequtech.github.io/SimulaQron/html/ConfNodes.html>`_.

^^^^^^^^^^^^^
Qubit example
^^^^^^^^^^^^^

The following code is based on the `test/src/qubit.c` test.

Copy the following code into a file called `qubit.c`:

.. code-block:: c

    #include <stdio.h>
    #include <stdlib.h>

    #include "cqc.h"

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
        cqc_connect(cqc, hostname, portno);

        /* Create qubit */
        uint16_t qubit;
        cqc_simple_cmd(cqc, CQC_CMD_NEW, 0, false);
        cqc_wait_until_newok(cqc, &qubit);

        /* Apply H gate */
        cqc_simple_cmd(cqc, CQC_CMD_H, qubit, true);
        cqc_wait_until_done(cqc, 1);

        uint8_t outcome;
        cqc_measure(cqc, qubit, &outcome);
        printf("Outcome: %d\n",outcome);

        return 0;
    }

To compile run

.. code-block:: bash

    gcc -I<path_to_cqc> -o qubit <path_to_cqc>/cqc.c

And run using

.. code-block:: bash

    ./qubit localhost 8803

This example will create a qubit in the `|` 0 `>` state, transorm it into the `|` + `>` state, and measure it. The outcome will be 0 or 1 (randomly determined with equal probability).

^^^^^^^^^^^^^
More examples
^^^^^^^^^^^^^

For more examples, have a look at the tests in `test/src`. All examples should be easy to adapt by removing `#include "test.h"` and the `ASSERT_` macros.