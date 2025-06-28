#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
struct Block {
    string filename;
    string hash;
    string height;
    string total;
    string time;
    string relayed_by;
    string prev_block;
    Block* next = nullptr;
};

Block parse_block(const fs::path& path);
void print_block(const Block& b);
void addBlock(Block*& head, const Block& newBlock);
void loadBlocks(Block*& head, const string& directory, int count);
void freeBlocks(Block* head);

#endif

