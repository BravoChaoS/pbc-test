#include "merklecpp.h"
#include <iostream>
#include <cstdio>
#include "log.h"

int main(){
    LogTree logTree;
    Proofs proofs;
    std::string srcStr = "01010101", encodedHexStr;

    time_t start, end;

    int ni = 10, nj = 1000;

    int size;
    for (int i = 0; i <= ni; ++i) {
//        std::cout << (1 << i) << ' ' << (1 << (i + 1)) << std::endl;
        for (int j = 0; j < nj; ++j) {

            srcStr = std::to_string(i * j);
            sha256(srcStr, encodedHexStr);
            ChronTreeT::Hash hash(encodedHexStr);
            logTree.append(hash, proofs);
        }
//        std::cout << (i + 1) * nj << ' ' << size * logTree.chronTree.get_node_size() << std::endl;

        int sum = 0;
        int maxsize = logTree.chronTree.max_index();
//        std::cout << maxsize << std::endl;
        std::shared_ptr<ChronTreeT::Path> path = logTree.chronTree.path(1);
        start = clock();
        for (int k = logTree.chronTree.min_index(); k <= logTree.chronTree.max_index(); ++k) {
            logTree.chronTree.leaf(k);
        }
        end = clock();
        sum += end - start;

        std::cout << (i + 1) * nj << ' ' << double(sum) / 1000 << std::endl;
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