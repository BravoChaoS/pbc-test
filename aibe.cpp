#include <iostream>
#include "aibe.h"
#include <ctime>
#include <fstream>

void test_lambda() {

    clock_t s, e, sum, sum1, sum2;
    clock_t cnt[5];

    for (int i = 0; i < 5; ++i) {
        cnt[i] = 0;
    }

    AibeAlgo aibeAlgo;
    const int na = 100, nb = 10;

    uint8_t output[20000], decrypt[20000];
    char input[20000];
    for (int i = 0; i < 1000; ++i) {
        input[i] = (int) ('a') + i % 26;
    }
    input[1000] = '\0';

    for (int i = 4; i <= 16; ++i) {
        const char path[] = "param/aibe.param";
        int rbits = 160;
        int qbits = (i << 6); // lambda

        sum = 0;
        sum1 = 0;
        sum2 = 0;
        for (int j = 0; j < na; ++j) {
            FILE *file = fopen(path, "w+");

            pbc_param_t p;
            pbc_param_init_a_gen(p, rbits, qbits);
            pbc_param_out_str(file, p);

            fclose(file);

            if (aibeAlgo.load_param(param_path)) {
                fprintf(stderr, "Param File Path error\n");
                exit(-1);
            }
            aibeAlgo.init();

////    aibe load_param
            aibeAlgo.pkg_setup_generate();

            int size;

            s = clock();
            for (int j = 0; j < nb; ++j) {
                aibeAlgo.keygen1(ID);
            }
            e = clock();
            sum += e - s;

            s = clock();
            for (int j = 0; j < nb; ++j) {
                aibeAlgo.keygen2();
            }
            e = clock();
            sum1 += e - s;

            s = clock();
            for (int j = 0; j < nb; ++j) {
                aibeAlgo.keygen3();
            }
            e = clock();
            sum2 += e - s;
        }
        printf("1,%d,%d,%lf\n", i, qbits, double(sum) / (na * nb * 1000));
        printf("2,%d,%d,%lf\n", i, qbits, double(sum1) / (na * nb * 1000));
        printf("3,%d,%d,%lf\n", i, qbits, double(sum2) / (na * nb * 1000));


    }
}

void test_algo() {

    double hw = 9.6;
//    std::ofstream ofstream(eval_path);

//    std::string out_dir = "/media/jojjiw/EX/ub-space/Accountable-data/testing-data/";
    const std::string out_dir = "/media/jojjiw/EX/ub-space/Accountable-data/testing-data/N_SN/N_SN_" + std::to_string(N_SN) + "_";
    std::ofstream ofstream(out_dir + "time-ibe.csv");
    clock_t s, e;
    double cnt[6];

    for (int i = 0; i < 6; ++i) {
        cnt[i] = 0;
    }

    AibeAlgo aibeAlgo;
    const int na = 1000, nb = 1;

//    ofstream << "kgen1(us),kgen2(us),kgen3(us),encrypt(us),decrypt(us),inspect(us)" << std::endl;
    ofstream << "kgen1(us),kgen2(us),kgen3(us)" << std::endl;

    if (aibeAlgo.load_param(param_path)) {
        fprintf(stderr, "Param File Path error\n");
        exit(-1);
    }
    aibeAlgo.init();

////    aibe load_param
//    aibeAlgo.pkg_setup_generate();
    aibeAlgo.mpk_load();
    aibeAlgo.msk_load();

    aibeAlgo.keygen1(IDSN);
    aibeAlgo.keygen2();
    aibeAlgo.keygen3();

    for (int i = 0; i < na; ++i) {
        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.keygen1(IDSN);
        }
        e = clock();
        cnt[0] += e - s;
        ofstream << e - s << ',';

        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.set_Hz(IDSN);
            aibeAlgo.keygen2();
        }
        e = clock();
        cnt[1] += e - s;
        ofstream << e - s << ',';

        s = clock();
        for (int j = 0; j < nb; ++j) {
            aibeAlgo.keygen3();
        }
        e = clock();
        cnt[2] += e - s + hw;
//        ofstream << e - s << ',';
        ofstream << e - s << std::endl;

//        s = clock();
//        for (int j = 0; j < nb; ++j) {
//            aibeAlgo.dk_verify();
//            aibeAlgo.dk_verify();
//        }
//        e = clock();
//        cnt[3] += e - s;
//        ofstream << e - s << ',';
//
//        element_random(aibeAlgo.m);
//
//        s = clock();
//        for (int j = 0; j < nb; ++j) {
//            aibeAlgo.block_encrypt(IDSN);
//        }
//        e = clock();
//        cnt[4] += e - s;
//        ofstream << e - s << ',';
////        std::cout << e - s << std::endl;
//
//        s = clock();
//        for (int j = 0; j < nb; ++j) {
//            aibeAlgo.block_decrypt();
//        }
//        e = clock();
//        cnt[5] += e - s;
//        ofstream << e - s << std::endl;

    }
    std::cout << "keygen1(ms): " << cnt[0] / (na * nb * 1000) << std::endl;

    std::cout << "keygen2(ms): " << cnt[1] / (na * nb * 1000) << std::endl;

    std::cout << "keygen3(ms): " << cnt[2] / (na * nb * 1000) << std::endl;

    std::cout << "encrypt(ms): " << cnt[4] / (na * nb * 1000) << std::endl;

    std::cout << "decrypt(ms): " << cnt[5] / (na * nb * 1000) << std::endl;

    std::cout << "inspect(ms): " << cnt[3] / (na * nb * 1000) << std::endl;

    aibeAlgo.clear();
}

void test_pbc() {

    clock_t s, e, sum, sum1, sum2;

    AibeAlgo aibeAlgo;

    aibeAlgo.load_param(param_path);
    aibeAlgo.init();
    const int na = 1000, nb = 10;


    aibeAlgo.mpk_load();
    aibeAlgo.msk_load();

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

    std::string eval_path = "/media/jojjiw/EX/ub-space/Accountable-data/testing-data/time-pbc.csv";
    std::ofstream ofstream(eval_path);

//    ofstream << "pairing(us),pow_zn(us),gt_pow_zn(us),mul(us)" << std::endl;
    double bp, ep_g1, ep_gt, bm, hw;
    hw = 0.0096;

    sum = 0;
    ofstream << "bp(us),";
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_pairing(gt, a, b);
        }
        e = clock();
        sum += e - s;
        ofstream << double (e - s) / nb << ',';
    }
    ofstream << std::endl;
    printf("bp(ms): %lf\n", bp = double(sum) / (na * nb * 1000));

    sum = 0;
    ofstream << "ep_g1(us),";
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(zr);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_pow_zn(g1, a, zr);
        }
        e = clock();
        sum += e - s;
        ofstream << double (e - s) / nb << ',';
    }
    ofstream << std::endl;
    printf("ep_g1(ms): %lf\n", ep_g1 = double(sum) / (na * nb * 1000));

    sum = 0;
    ofstream << "ep_gt(us),";
    for (int i = 0; i < na; ++i) {
        element_random(gt);
        element_random(zr);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_pow_zn(gt, gt, zr);
        }
        e = clock();
        sum += e - s;
        ofstream << double (e - s) / nb << ',';
    }
    ofstream << std::endl;
    printf("ep_gt(ms): %lf\n", ep_gt = double(sum) / (na * nb * 1000));

    sum = 0;
    ofstream << "bm(us),";
    for (int i = 0; i < na; ++i) {
        element_random(a);
        element_random(b);

        s = clock();
        for (int j = 0; j < nb; ++j) {
            element_mul(g1, a, b);
        }
        e = clock();
        sum += e - s;
        ofstream << double (e - s) / nb << ',';
    }
    ofstream << std::endl;
    printf("bm(ms): %lf\n", bm = double(sum) / (na * nb * 1000));

    std::cout << "encrypt(est): " << 2 * bp + 2 * ep_g1 + 2 * ep_gt + 1 * bm << std::endl;
    std::cout << "keygen1(est): " << 0 * bp + 2 * ep_g1 + 1 * ep_gt + 1 * bm << std::endl;
    std::cout << "keygen2(est): " << 0 * bp + 4 * ep_g1 + 0 * ep_gt + 3 * bm + hw << std::endl;
    std::cout << "keygen3(est): " << 4 * bp + 3 * ep_g1 + 1 * ep_gt + 5 * bm << std::endl;
    std::cout << "decrypt(est): " << 2 * bp + 0 * ep_g1 + 1 * ep_gt + 3 * bm << std::endl;
    std::cout << "inspect(est): " << 8 * bp + 0 * ep_g1 + 2 * ep_gt + 4 * bm << std::endl;
}

void size_check() {

    clock_t s, e;
    clock_t cnt[6];

    for (int i = 0; i < 6; ++i) {
        cnt[i] = 0;
    }

    AibeAlgo aibeAlgo;
    const int na = 100, nb = 10;


    if (aibeAlgo.load_param(param_path)) {
        fprintf(stderr, "Param File Path error\n");
        exit(-1);
    }
    aibeAlgo.init();

////    aibe load_param
    aibeAlgo.pkg_setup_generate();
    aibeAlgo.mpk_load();
    aibeAlgo.msk_load();

    aibeAlgo.keygen1(ID);
    aibeAlgo.keygen2();
    aibeAlgo.keygen3();
    aibeAlgo.dk_verify();

    element_random(aibeAlgo.m);

    aibeAlgo.block_encrypt(ID);
    aibeAlgo.block_decrypt();

    int size_G1 = pairing_length_in_bytes_G1(aibeAlgo.pairing);
    int size_mpk = (3 + N_IDSN + 1) * size_G1;
    int size_ct = 2 * size_G1 + 2 * aibeAlgo.size_GT;
    int size_dk = 2 * size_G1 + aibeAlgo.size_Zr;

    const int n = 6, m = 6;
    std::string algs[n] = {
            "enc",
            "kgen1",
            "kgen2",
            "kgen3",
            "dec",
            "teeinspect"
    };
    int ele_sizes[m] = {aibeAlgo.size_Zr, size_G1, aibeAlgo.size_GT, size_mpk, size_dk, size_ct};
    int num[n][m] = {
            {1, 2, 1, 1, 0, 1},
            {0, 0, 3, 0, 1, 1},
            {2, 3, 0, 1, 0, 0},
            {4, 2, 0, 1, 1, 0},
            {5, 3, 3, 1, 2, 0},
            {0, 2, 3, 1, 2, 0}
    };

    for (int i = 0; i < n; ++i) {
        std::cout << algs[i] << ": ";
        long long sum = 0;
        for (int j = 0; j < m; ++j) {
            sum += ele_sizes[j] * num[i][j];
        }
        std::cout << double(sum) / 1024 << std::endl;
    }

//    std::cout << "enc: " << aibeAlgo.size_Zr + size_G1 + aibeAlgo.size_GT + size_mpk + size_dk + size_ct << std::endl;
//    std::cout << "kgen1: " << aibeAlgo.size_Zr + size_G1 + aibeAlgo.size_GT + size_mpk + size_dk + size_ct << std::endl;
//    std::cout << "kgen2: " << aibeAlgo.size_Zr + size_G1 + aibeAlgo.size_GT + size_mpk + size_dk + size_ct << std::endl;
//    std::cout << "kgen3: " << aibeAlgo.size_Zr + size_G1 + aibeAlgo.size_GT + size_mpk + size_dk + size_ct << std::endl;
//    std::cout << "dec: " << aibeAlgo.size_Zr + size_G1 + aibeAlgo.size_GT + size_mpk + size_dk + size_ct << std::endl;
//    std::cout << "teeinspect: " << aibeAlgo.size_Zr + size_G1 + aibeAlgo.size_GT + size_mpk + size_dk + size_ct << std::endl;

    aibeAlgo.clear();
}

int main() {
//    test_pbc();
    test_algo();
//    size_check();
    return 0;
}