#include "merklecpp.h"
#include <iostream>
#include <cstdio>
#include "log.h"
#include <cmath>

int main(){
    LogTree logTree;
    Proofs proofs;
    std::string srcStr = "01010101", encodedHexStr;

    time_t start, end;

    int ni = 13, nj = 1000, ni2 = 1;

    int size;
    for (int i = 0; i < (1 << ni2); ++i) {
        srcStr = std::to_string(i);
        sha256(srcStr, encodedHexStr);
        ChronTreeT::Hash hash(encodedHexStr);
        logTree.append(hash, proofs);
    }

    for (int i = ni2; i <= ni; ++i) {
//        std::cout << (1 << i) << ' ' << (1 << (i + 1)) << std::endl;
        std::shared_ptr<ChronTreeT::Path> poe;
        ChronTreeT::Hash old_rth;

        for (int j = (1 << i); j < (1 << (i + 1)); ++j) {
            if(j == (1 << (i + 1)) - 1){
                poe = logTree.chronTree.path(logTree.chronTree.max_index());
                old_rth = logTree.chronTree.root();
            }
            srcStr = std::to_string(j);
            sha256(srcStr, encodedHexStr);
            ChronTreeT::Hash hash(encodedHexStr);
            logTree.append(hash, proofs);
        }
//        std::cout << (i + 1) * nj << ' ' << size * logTree.chronTree.get_node_size() << std::endl;

        int sum = 0;
        int maxsize = logTree.chronTree.max_index();
//        std::cout << logTree.chronTree.min_index() << ' ' << maxsize << std::endl;
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