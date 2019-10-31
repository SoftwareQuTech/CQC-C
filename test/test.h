#ifndef TEST_H
#define TEST_H

#include <assert.h>
#include "cqc.h"

enum test_rc {
    TEST_RC_OK = 0,             /* Success */
    TEST_RC_FAIL = 1,           /* Test failure */
};

#define ASSERT_CQC_CALL(CALL) assert((CALL) == CQC_LIB_OK)
#define ASSERT_TEST_CALL(CALL) assert((CALL) == TEST_RC_OK)

#endif
