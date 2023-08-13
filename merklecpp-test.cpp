#include "merklecpp.h"
#include <iostream>
#include <cstdio>
#include "log.h"
#include <cmath>
#include <chrono>
#include "aibe.h"

using namespace std::chrono;

void test_mt() {
    int sum = 0;
    clock_t start, end;
    double ts[10100];
    for (int i = 0; i < 100; ++i) {
        start = clock();

        LogTree logTree;

        end = clock();
        ts[i] = end - start;
        sum += ts[i];
    }
    printf("MT.init(μs): %lf\n", double(sum) / 100);
    for (int i = 0; i < 100; ++i) {
        printf("%d\n", (int) ts[i]);
    }
};


void test_log_insert(){

    LogTree logTree;
    Proofs proofs;
    std::string baseStr = "01010101010101010101", encodedHexStr, srcStr;

    time_t start, end;

    int ni = 13, nj = 1000, ni2 = 1;

    int size, id, sn;
    time_point<steady_clock> tp[100];
    microseconds us[100];

    for (int i = 0; i < ni; ++i) {

        tp[i] = steady_clock::now();
        for (int j = 1 << i; j < 1 << (i + 1); ++j) {
            sn = j;
            id = get_idsn(ID, sn);

            json j_node{
                    {"id", id},
                    {"sn", sn}
            };

            // MT.Insert
            json j_str = j_node.dump();
            std::cout << "jnode" << j_node << std::endl;
            sha256(j_str, encodedHexStr);
            ChronTreeT::Hash hash(encodedHexStr);
            logTree.append(id, j_str, hash, proofs);
        }
    }
    tp[ni] = steady_clock::now();

    for (int i = 1; i <= ni; ++i) {
        us[i] = duration_cast<microseconds>(tp[i] - tp[i - 1]);
        std::cout << "(" << i << "," << double(us[i].count()) / (1000 << (i - 1)) << ")";
    }

    std::cout << std::endl;
}

void test_log_search(){

    LogTree logTree;
    Proofs proofs;
    std::string baseStr = "01010101010101010101", encodedHexStr, srcStr;

    time_t start, end;

    int ni = 13, nj = 1000, ni2 = 1;

    int size;
    time_point<steady_clock> s, e;
    microseconds us[100];
    int id = IDSN, sn;

    for (int i = 0; i < ni; ++i) {

        for (int j = 1 << i; j < 1 << (i + 1); ++j) {

            sn = j;
            id = get_idsn(ID, sn);

            json j_node{
                    {"id", id},
                    {"sn", sn}
            };

            // MT.Insert
            json j_str = j_node.dump();
//            std::cout << "jnode" << j_node << std::endl;
            sha256(j_str, encodedHexStr);
            ChronTreeT::Hash hash(encodedHexStr);
            logTree.append(id, j_str, hash, proofs);
        }

        s = steady_clock::now();

        for (int j = 1 << i; j < 1 << (i + 1); ++j) {
            sn = j;
            id = get_idsn(ID, sn);
            std::vector<json> lst;
            logTree.trace(id, lst);
        }

        e = steady_clock::now();
        us[i + 1] = duration_cast<microseconds>(e - s);
    }

    for (int i = 1; i <= ni; ++i) {
        std::cout << "(" << i << "," << double(us[i].count()) / (1000 << (i - 1)) << ")";
    }

    std::cout << std::endl;
}

void test_log_size(){

    LogTree logTree;
    Proofs proofs;
    std::string srcStr = "01010101", encodedHexStr;

    time_t start, end;

    int ni = 13, nj = 1000, ni2 = 1;

    int size, id, sn;
    for (int i = 0; i < (1 << ni2); ++i) {
        sn = i;
        id = get_idsn(ID, sn);

        json j_node{
                {"id", id},
                {"sn", sn}
        };

        // MT.Insert
        json j_str = j_node.dump();
        std::cout << "jnode" << j_node << std::endl;
        sha256(j_str, encodedHexStr);
        ChronTreeT::Hash hash(encodedHexStr);
        logTree.append(id, j_str, hash, proofs);
    }

    for (int i = ni2; i <= ni; ++i) {
//  tree size - log entries
        std::shared_ptr<ChronTreeT::Path> poe;
        ChronTreeT::Hash old_rth;

        for (int j = (1 << i); j < (1 << (i + 1)); ++j) {
            if(j == (1 << (i + 1)) - 1){
                poe = logTree.chronTree.path(logTree.chronTree.max_index());
                old_rth = logTree.chronTree.root();
            }
            sn = j;
            id = get_idsn(ID, sn);

            json j_node{
                    {"id", id},
                    {"sn", sn}
            };

            // MT.Insert
            json j_str = j_node.dump();
            std::cout << "jnode" << j_node << std::endl;
            sha256(j_str, encodedHexStr);
            ChronTreeT::Hash hash(encodedHexStr);
            logTree.append(id, j_str, hash, proofs);
        }
//        std::cout << (i + 1) * nj << ' ' << size * logTree.chronTree.get_node_size() << std::endl;

//  proof time - log entries
        int sum = 0;
        int maxsize = logTree.chronTree.max_index();
        std::shared_ptr<ChronTreeT::Path> pop = logTree.chronTree.path(maxsize);
        ChronTreeT::Hash new_rth = logTree.chronTree.root();
        start = clock();
        for (int j = 0; j < 1000; ++j) {
            assert(pop->verify(new_rth));
            assert(poe->verify(old_rth));
        }

        end = clock();
        sum += end - start;

        std::cout << '(' << std::log2(1 << (i + 1)) << ',' << double(sum) / CLOCKS_PER_SEC << ')';
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void test_proof_size(){

    LogTree logTree;
    Proofs proofs;
    std::string oriStr = "10101010101010101010101010101010101010101010101010";
    std::string srcStr = " ", encodedHexStr;

    time_t start, end;

    int n = 1000, shift = -10, dlt = 50;

    int size, id, sn;

    for (int i = 1; i <= n; ++i) {
//  tree size - log entries
        std::shared_ptr<ChronTreeT::Path> poe;
        ChronTreeT::Hash old_rth;


        sn = i;
        id = get_idsn(ID, sn);

        json j_node{
                {"id", id},
                {"sn", sn}
        };

        // MT.Insert
        json j_str = j_node.dump();
        std::cout << "jnode" << j_node << std::endl;
        sha256(j_str, encodedHexStr);
        ChronTreeT::Hash hash(encodedHexStr);
        logTree.append(id, j_str, hash, proofs);


        if ( (i + shift) % dlt == 0 ) {
            double sum = 0, tot = 0;
            for (int j = logTree.chronTree.min_index(); j <= logTree.chronTree.max_index(); ++j) {
                std::shared_ptr<ChronTreeT::Path> pop = logTree.chronTree.path(j);
                sum += pop->serialised_size();
                ++tot;
            }
            std::cout << "(" << i << "," << sum / tot << ")" << std::endl;

        }



    }
    std::cout << std::endl;
}

int main(){
    test_log_search();
//    test_log_insert();
//    test_mt();
//    test_proof_size();
    return 0;
}

size_t physical_memory_used_by_process()
{
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != nullptr) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            int len = strlen(line);

            const char* p = line;
            for (; std::isdigit(*p) == false; ++p) {}

            line[len - 3] = 0;
            result = atoi(p);

            break;
        }
    }

    fclose(file);

    return result;
}