#include <cstdlib>
#include <cstdio>

#include "aibe.h"

void check(bool b);

int main(int argc, char **argv) {
    pairing_t pairing;
    char param[1024];
    FILE *param_file = fopen(file_path, "r+");
    size_t count = fread(param, 1, 1024, param_file);
    if (!count) pbc_die("input error");
    pairing_init_set_buf(pairing, param, count);

////    elements
    // param elements
    element_t x;
    element_t g;
    mpk_t mpk;

    // user elements
    element_t Hz;
    element_t t0;
    element_t theta;
    element_t R;
    element_t r;
    element_t r2; // r''
    element_t el;
    element_t er;

    // pkg elements
    element_t r1; // r'
    element_t t1;
    dk_t dk; // d_ID
    dk_t dk1; // d'_ID

    // temp elements
    element_t tz;
    element_t tg;
    element_t te;

////    element init
    element_init_Zr(x, pairing);
    element_init_G2(g, pairing);
    mpk.init(pairing);

    element_init_G1(Hz, pairing);
    element_init_Zr(t0, pairing);
    element_init_Zr(theta, pairing);
    element_init_G1(R, pairing);
    element_init_Zr(r, pairing);
    element_init_Zr(r2, pairing);
    element_init_GT(el, pairing);
    element_init_GT(er, pairing);

    element_init_Zr(r1, pairing);
    element_init_Zr(t1, pairing);
    dk1.init(pairing);
    dk.init(pairing);

    element_init_Zr(tz, pairing);
    element_init_G1(tg, pairing);
    element_init_GT(te, pairing);

////    setup: pkg
    puts("pkg: setup...");

    element_random(g);
    element_random(mpk.h);
    element_random(mpk.Y);
    element_random(x);
    for (int i = 0; i < N; ++i) {
        element_random(mpk.Z[i]);
    }
    element_pow_zn(mpk.X, g, x);

    puts("pkg: setup finished");

////    keygen1: user
    puts("usr: keygen1...");

    element_random(t0);
    element_random(theta);

    element_set(Hz, mpk.Z[0]);

    {
        mpz_t digit;
        for (int i = 1; i <= mpk.z_size; ++i) {
            mpz_init_set_si(digit, get_bit(ID, i));
            if (!mpz_is0(digit))
                element_mul(Hz, Hz, mpk.Z[i]);
            mpz_clear(digit);
        }
    }

    // R = h^t0 * X^theta
    element_pow_zn(R, mpk.h, t0);
    element_pow_zn(tg, mpk.X, theta);
    element_mul(R, R, tg);

    puts("usr: keygen1 finished");

////    keygen2: pkg
    puts("pkg: keygen2...");

    element_random(r1);
    element_random(t1);

    //  d1 = (Y * R * h^t1)^(1/x) * Hz^r1
    //      d1 = Y * R
    element_mul(dk1.d1, mpk.Y, R);
    //      d1 = d1 * h^t1
    element_pow_zn(tg, mpk.h, t1);
    element_mul(dk1.d1, dk1.d1, tg);
    //      d1 = d1 ^ (1/x)
    element_invert(tz, x);
    element_pow_zn(dk1.d1, dk1.d1, tz);
    //      d1 = d1 * Hz^r1;
    element_pow_zn(tg, Hz, r1);
    element_mul(dk1.d1, dk1.d1, tg);
    // d2 = X^r1
    element_pow_zn(dk1.d2, mpk.X, r1);
    // d3 = t1
    element_set(dk1.d3, t1);

    puts("pkg: keygen2 finished");

////    keygen3: usr
    puts("usr: keygen3...");

    element_random(r2);
    element_add(r, r1, r2);
    //  d1 = d1' / g^theta * Hz^r2
    //      d1 = d1' / g^theta
    element_pow_zn(tg, g, theta);
    element_div(dk.d1, dk1.d1, tg);
    //      d1 = d1 * Hz^r2
    element_pow_zn(tg, Hz, r2);
    element_mul(dk.d1, dk.d1, tg);
    //  d2 = d2' * X^r2
    element_pow_zn(tg, mpk.X, r2);
    element_mul(dk.d2, dk1.d2, tg);
    //  d3 = d3' + t0
    element_add(dk.d3, dk1.d3, t0);

    //  el = e(d1, X)
    element_pairing(el, dk.d1, mpk.X);
    //  er = e(Y, g)
    element_pairing(er, mpk.Y, g);
    //  er = er * e(h, g)^d3
    element_pairing(te, mpk.h, g);
    element_pow_zn(te, te, dk.d3);
    element_mul(er, er, te);
    //  er = er * e(Hz, d2)
    element_pairing(te, Hz, dk.d2);
    element_mul(er, er, te);

    if (element_cmp(el, er)) {
        puts("invalid key");
    } else {
        puts("valid key");
    }

    puts("usr: keygen3 finished");

////    check d1
//    element_t td;
//    element_init_G1(td, pairing);
//
//    element_add(tz, t0, t1);
//    element_pow_zn(tg, mpk.h, tz);
//    element_mul(td, mpk.Y, tg);
//    element_invert(tz, x);
//    element_pow_zn(td, td, tz);
//    element_pow_zn(tg, Hz, r);
//    element_mul(td, td, tg);
//
//    printf("check d1: ");
//    check(element_cmp(td, dk.d1));
//
//    element_clear(td);
//
////    check d2
//    element_add(tz, r1, r2);
//    element_pow_zn(tg, mpk.X, tz);
//    printf("check d2: ");
//    check(element_cmp(tg, dk.d2));
//
////    check d3
//    element_add(tz, t0, t1);
//    printf("check d3: ");
//    check(element_cmp(tz, dk.d3));

////    element clear
    // find: element_init_([a-zA-Z0-9]*)\(([a-zA-Z0-9.\[\]]+), ([a-zA-Z]+)\)
    // repl: element_clear($2)
    element_clear(x);
    element_clear(g);
    mpk.clear();

    element_clear(Hz);
    element_clear(t0);
    element_clear(theta);
    element_clear(R);
    element_clear(r);
    element_clear(r2);
    element_clear(el);
    element_clear(er);

    element_clear(r1);
    element_clear(t1);
    dk1.clear();
    dk.clear();

    element_clear(tz);
    element_clear(tg);
    element_clear(te);

    return 0;
}

void check(bool b) {
    if (b) {
        puts("not equal");
    } else {
        puts("equal");
    }
}

int get_bit(int id, int n) {
    return (id >> (N - n)) & 1;
}

void mpk_t::init(pairing_t pairing) {
    element_init_G1(h, pairing);
    element_init_G1(X, pairing);
    element_init_G1(Y, pairing);
    for (int i = 0; i <= N; ++i) {
        element_init_G1(Z[i], pairing);
    }
}

void mpk_t::clear() {
    element_clear(h);
    element_clear(X);
    element_clear(Y);
    for (int i = 0; i <= N; ++i) {
        element_clear(Z[i]);
    }
}

void dk_t::init(pairing_t pairing) {
    element_init_G1(d1, pairing);
    element_init_G1(d2, pairing);
    element_init_Zr(d3, pairing);
}

void dk_t::clear() {
    element_clear(d1);
    element_clear(d2);
    element_clear(d3);
}