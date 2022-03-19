//
// Created by jojjiw on 2022/3/17.
//

#ifndef PBC_TEST_AIBE_H
#define PBC_TEST_AIBE_H

#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

const int N = 8;
const int ID = 0b10101010;
const char file_path[] = "param/aibe.param";

struct mpk_t {
    int z_size = N + 1;
    element_t X, Y, h, Z[N + 1];

    void init(pairing_t pairing);

    void clear();
};

struct dk_t {
    element_t d1, d2, d3;

    void init(pairing_t pairing);

    void clear();
};

int get_bit(int id, int n);

void calculate_hash(element_t hash, int id, element_t *z, int z_size, pairing_t pairing);

void calculate_R(element_t R, element_t t0, element_t theta, mpk_t mpk, pairing_t pairing);

void calculate_dk1(dk_t dk, element_t r1, element_t t1, element_t R, element_t Hz, mpk_t mpk, element_t msk,
                   pairing_t pairing);

void calculate_dk(dk_t dk, element_t g, element_t t0, element_t theta, element_t r2, dk_t dk1, mpk_t mpk, pairing_t pairing);

#endif //PBC_TEST_AIBE_H
