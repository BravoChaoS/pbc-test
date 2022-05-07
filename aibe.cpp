#include <iostream>
#include "aibe.h"
#include <ctime>

int main(){
    clock_t s, e, sum;
    clock_t cnt[5];

    for (int i = 0; i < 5; ++i) {
        cnt[i] = 0;
    }

    const int na = 1, nb = 1000;
    AibeAlgo aibeAlgo;
    if (aibeAlgo.load_param(param_path)) {
        fprintf(stderr, "Param File Path error\n");
        exit(-1);
    }
    aibeAlgo.init();

////    aibe load_param
    aibeAlgo.pkg_setup_generate();
    aibeAlgo.mpk_load();
    aibeAlgo.msk_load();

    for (int i = 0; i < na; ++i) {
        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.keygen1(ID);
        }
        e = clock();
        cnt[0] += e - s;

        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.keygen2();
        }
        e = clock();
        cnt[1] += e - s;

        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.keygen3();
        }
        e = clock();
        cnt[2] += e - s;

        element_random(aibeAlgo.m);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.block_encrypt(ID);
        }
        e = clock();
        cnt[3] += e - s;

        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.block_decrypt();
        }
        e = clock();
        cnt[4] += e - s;
    }
    std::cout << "keygen1(ms): " << double(cnt[0]) / CLOCKS_PER_SEC << std::endl;

    std::cout << "keygen2(ms): " << double(cnt[1]) / CLOCKS_PER_SEC << std::endl;

    std::cout << "keygen3(ms): " << double(cnt[2]) / CLOCKS_PER_SEC << std::endl;

    std::cout << "encrypt(ms): " << double(cnt[3]) / CLOCKS_PER_SEC << std::endl;

    std::cout << "decrypt(ms): " << double(cnt[4]) / CLOCKS_PER_SEC << std::endl;

    aibeAlgo.clear();


    element_t a;
    element_t b;
    element_t g1;
    element_t zr;
    element_t gt;

    element_init_G1(a, aibeAlgo.pairing);
    element_init_G1(b, aibeAlgo.pairing);
    element_init_G1(g1, aibeAlgo.pairing);
    element_init_Zr(zr, aibeAlgo.pairing);
    element_init_GT(gt, aibeAlgo.pairing);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_pairing(gt, a, b);
        }
        e = clock();
        sum += e - s;
    }
    printf("pairing(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(zr);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_pow_zn(g1, a, zr);
        }
        e = clock();
        sum += e - s;
    }
    printf("pow_zn(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(gt);
        element_random(zr);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_pow_zn(gt, gt, zr);
        }
        e = clock();
        sum += e - s;
    }
    printf("gt_pow_zn(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_mul(g1, a, b);
        }
        e = clock();
        sum += e - s;
    }
    printf("mul(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    sum = 0;
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_div(g1, a, b);
        }
        e = clock();
        sum += e - s;
    }
    printf("div(ms): %lf\n", double(sum) / (na * nb / 1000) / CLOCKS_PER_SEC);

    return 0;
}