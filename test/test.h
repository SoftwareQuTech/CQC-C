#ifndef TEST_H
#define TEST_H

#include <stdint.h>

#include "cqc.h"

float cqc_tomography_dir(cqc_lib *cqc,
                         uint16_t (*func)(cqc_lib *),
                         uint32_t iter,
                         uint8_t dir);
int cqc_test_qubit(cqc_lib *cqc,
                   uint16_t (*func)(cqc_lib *),
                   uint32_t iter,
                   float epsilon,
                   float exp_x,
                   float exp_y,
                   float exp_z);

#endif
