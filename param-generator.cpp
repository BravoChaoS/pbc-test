//
// Created by jojjiw on 2022/3/13.
//

#include <iostream>
#include <cstdio>
#include <fstream>
#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

const char path[] = "param/aibe.param";
const int rbits = 160;
const int qbits = (4 << 6); // lambda

int main(int argc, char **argv) {
    FILE *file = fopen(path, "w+");

    pbc_param_t p;
    pbc_param_init_a_gen(p, rbits, qbits);

    pbc_param_out_str(file, p);





    return 0;
}
