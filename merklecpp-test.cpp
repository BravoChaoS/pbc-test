#include "merklecpp.h"
#include <iostream>
#include <cstdio>

#include <openssl/sha.h>

void sha256(const std::string &srcStr, std::string &encodedHexStr)
{

    unsigned char mdStr[33] = { 0 };
    SHA256((const unsigned char *)srcStr.c_str(), srcStr.length(), mdStr);// 调用sha256哈希

    char buf[65] = { 0 };
    char tmp[3] = { 0 };
    for (int i = 0; i < 32; i++)// 哈希后的十六进制串 32字节
    {
        sprintf(tmp, "%02x", mdStr[i]);
        strcat(buf, tmp);
    }
    buf[64] = '\0'; // 后面都是0，从32字节截断
    encodedHexStr = std::string(buf);
}

int main(){
    merkle::TreeT<32, merkle::sha256_openssl> tree;

    std::string srcStr = "message", encodedHexStr;

    sha256(srcStr, encodedHexStr);

    merkle::Hash hash(encodedHexStr);
    std::vector<merkle::Tree::Hash> hashes;
    hashes.push_back(hash);
    for (auto h : hashes)
    tree.insert(h);
    auto root = tree.root();
    auto path = tree.path(hashes.size() - 1);
    assert(path->verify(root));
    std::cout << "verify succeed" << std::endl;
    return 0;
}