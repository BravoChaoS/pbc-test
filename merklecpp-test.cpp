#include "merklecpp.h"
#include <iostream>
#include <cstdio>
#include "log.h"

int main(){
    LogTree logTree;
    Proofs proofs;
    std::string srcStr = "01010101", encodedHexStr;

    time_t start, end;

    int ni = 20, nj = 1000;

    int size;
    for (int i = 0; i < ni; ++i) {
        for (int j = 0; j < nj; ++j) {

            srcStr = std::to_string(i * j);
            sha256(srcStr, encodedHexStr);
            ChronTreeT::Hash hash(encodedHexStr);
            logTree.append(hash, proofs);
        }
        size = logTree.chronTree.size();
//        std::cout << (i + 1) * nj << ' ' << size * logTree.chronTree.get_node_size() << std::endl;

        int sum = 0;
        int maxsize = logTree.chronTree.max_index();
        start = clock();
        int ind = random() % maxsize;
        for (int j = 0; j < 1000; ++j) {
            std::shared_ptr<ChronTreeT::Path> path = logTree.chronTree.path(ind);
//            for (int k = 0; k < 10; ++k) {
//                path->verify(logTree.chronTree.root());
//            }
        }
        end = clock();
        sum += end - start;

        std::cout << double(sum) / CLOCKS_PER_SEC << std::endl;
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