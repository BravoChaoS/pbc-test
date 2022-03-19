#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

int main(int argc, char **argv) {
    pairing_t pairing;
    char param[1024];
    size_t count = fread(param, 1, 1024, stdin);
    if (!count) pbc_die("input error");
    pairing_init_set_buf(pairing, param, count);

    element_t x, ix, res;
    element_init_Zr(x, pairing);
    element_init_Zr(ix, pairing);
    element_init_Zr(res, pairing);

    element_random(x);
    element_invert(ix, x);
    element_mul_zn(res, x, ix);

    element_printf("x: %B\n", x);
    element_printf("ix: %B\n", ix);
    element_printf("res: %B\n", res);

    return 0;
}