//
// Created by jojjiw on 2022/4/9.
//

#ifndef LM_LOG_H
#define LM_LOG_H

#include "merklecpp.h"
#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <chrono>
#include <sys/time.h>
#include "json.hpp"

#include <openssl/sha.h>

using json = nlohmann::json;

typedef merkle::TreeT<32, merkle::sha256_openssl> ChronTreeT;


typedef struct _log_header_t {
    uint32_t size[3];
} log_header_t;

void sha256(const std::string &srcStr, std::string &encodedHexStr) {
    unsigned char mdStr[33] = {0};
    SHA256((const unsigned char *) srcStr.c_str(), srcStr.length(), mdStr);// 调用sha256哈希

    char buf[65] = {0};
    char tmp[3] = {0};
    for (int i = 0; i < 32; i++)// 哈希后的十六进制串 32字节
    {
        sprintf(tmp, "%02x", mdStr[i]);
        strcat(buf, tmp);
    }
    buf[64] = '\0'; // 后面都是0，从32字节截断
    encodedHexStr = std::string(buf);
}

class Proofs {
public:
    ChronTreeT::Hash node;
    ChronTreeT::Hash root;
    std::shared_ptr<ChronTreeT::Path> path;

    bool verify_proofs() {
        return path->verify(root);
    }

    int serialise(uint8_t *bytes);

    int deserialise(uint8_t *bytes);
};

int Proofs::serialise(uint8_t *bytes) {
    log_header_t header;
    int size = 0;
    std::vector<uint8_t> vec;

    header.size[0] = node.serialised_size();
    header.size[1] = root.serialised_size();
//    header.size[2] = path->serialised_size();

    size += sizeof(log_header_t);

    // node
    vec.clear();
    node.serialise(vec);
    std::copy(vec.begin(), vec.end(), bytes + size);
    size += header.size[0];

    // root
    vec.clear();
    root.serialise(vec);
    std::copy(vec.begin(), vec.end(), bytes + size);
    size += header.size[1];

    // path
    vec.clear();
    path->serialise(vec);
    header.size[2] = vec.size();
    std::copy(vec.begin(), vec.end(), bytes + size);
    size += header.size[2];

    memcpy(bytes, &header, sizeof(log_header_t));

    return size;
}

int Proofs::deserialise(uint8_t *bytes) {

    int size = 0, len;
    std::vector<uint8_t> vec;
    log_header_t header;

    len = sizeof(log_header_t);
    memcpy(&header, bytes, len);
    size += len;

    // node
    len = header.size[0];
    vec.clear();
    vec.assign(bytes + size, bytes + size + len);
    node.deserialise(vec);
    size += len;

    // root
    len = header.size[1];
    vec.clear();
    vec.assign(bytes + size, bytes + size + len);
    root.deserialise(vec);
    size += len;

    // path
    len = header.size[2];
    vec.clear();
    vec.assign(bytes + size, bytes + size + len);
    path = std::make_shared<ChronTreeT::Path>(vec);
    size += len;

    return size;
}


class LogTree {
public:
    ChronTreeT chronTree;
    std::map<int, std::vector<std::string>> lexTree;

    int append(int id, const std::string& node, ChronTreeT::Hash hash, Proofs &prf);

    void trace(int ID, std::vector<json> &lst);

    int merkle_test() {
        std::string srcStr = "message", encodedHexStr;

        sha256(srcStr, encodedHexStr);

        ChronTreeT::Hash hash(encodedHexStr);
        std::vector<ChronTreeT::Hash> hashes;
        hashes.push_back(hash);
        for (auto h : hashes)
            chronTree.insert(h);
        auto root = chronTree.root();
        auto path = chronTree.path(hashes.size() - 1);
        assert(path->verify(root));
        std::cout << "verify succeed" << std::endl;
        return 0;
    }
};

std::string get_timestamp(timeval &tv) {
//    printf("%ld\t%ld\n", tv.tv_usec, tv.tv_sec);
    std::string image_n = std::to_string(tv.tv_sec) + "_" + std::to_string(tv.tv_usec);
    return image_n;
}

int LogTree::append(int id, const std::string& node, ChronTreeT::Hash hash, Proofs &prf) {
    int ret = 0;
    lexTree[id].push_back(node);
//    std::cout << "---------" << id << ' ' << lexTree[id].size() << ' ' << chronTree.size() << std::endl;
    prf.node = hash;
    chronTree.insert(hash);
    prf.root = chronTree.root();
    prf.path = chronTree.path(chronTree.max_index());
    return ret;
}

void LogTree::trace(int ID, std::vector<json> &lst) {
    if (lexTree.find(ID) == lexTree.end()) {
        return;
    }

    auto &vec = lexTree[ID];

    for (auto & i : vec) {
        lst.push_back(json::parse(i));
    }
    chronTree.path(chronTree.max_index());
}


#endif //LM_LOG_H
