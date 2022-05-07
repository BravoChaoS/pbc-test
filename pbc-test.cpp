
#include <stdio.h>
#include <time.h>
#include "pbc-test.h"
#include <iostream>

void check(int b);

int main(int argc, char **argv) {
    clock_t start, end;
    clock_t sum = 0;
    const int na = 100, nb = 10;

    pairing_t pairing;
    char param[1024];
    FILE *param_file = fopen(param_path, "r");
    size_t count = fread(param, sizeof(char), 1024, param_file);
    if (!count) pbc_die("input error");
    pairing_init_set_buf(pairing, param, count);

////    elements
    // param elements
    element_t a;
    element_t b;
    element_t g1;
    element_t zr;
    element_t gt;

    element_init_G1(a, pairing);
    element_init_G1(b, pairing);
    element_init_G1(g1, pairing);
    element_init_Zr(zr, pairing);
    element_init_GT(gt, pairing);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        start = clock();
        for (int j = 0; j < nb; ++j) {
            element_pairing(gt, a, b);
        }
        end = clock();
        sum += end - start;
    }
    printf("pairing(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(zr);

        start = clock();
        for (int j = 0; j < nb; ++j) {
            element_pow_zn(g1, a, zr);
        }
        end = clock();
        sum += end - start;
    }
    printf("pow_zn(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        start = clock();
        for (int j = 0; j < nb; ++j) {
            element_mul(g1, a, b);
        }
        end = clock();
        sum += end - start;
    }
    printf("mul(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        start = clock();
        for (int j = 0; j < nb; ++j) {
            element_div(g1, a, b);
        }
        end = clock();
        sum += end - start;
    }
    printf("div(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);
    return 0;
}

void check(int b) {
    if (b) {
        puts("not equal");
    } else {
        puts("equal");
    }
}

int get_bit(int id, int n) {
    return (id >> (N - n)) & 1;
}

void mpk_init(mpk_t *mpk, pairing_t pairing) {
    element_init_G1(mpk->h, pairing);
    element_init_G1(mpk->X, pairing);
    element_init_G1(mpk->Y, pairing);
    for (int i = 0; i <= N; ++i) {
        element_init_G1(mpk->Z[i], pairing);
    }
}

void mpk_clear(mpk_t *mpk) {
    element_clear(mpk->h);
    element_clear(mpk->X);
    element_clear(mpk->Y);
    for (int i = 0; i <= N; ++i) {
        element_clear(mpk->Z[i]);
    }
}

void dk_init(dk_t *dk, pairing_t pairing) {
    element_init_G1(dk->d1, pairing);
    element_init_G1(dk->d2, pairing);
    element_init_Zr(dk->d3, pairing);
}

void dk_clear(dk_t *dk) {
    element_clear(dk->d1);
    element_clear(dk->d2);
    element_clear(dk->d3);
}