#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

Block parse_block(const fs::path& path) {
    Block b;
    b.filename = path.filename();
    ifstream in(path);
    string line;

    while (getline(in, line)) {
        if (line.find("hash:") == 0)        b.hash = line.substr(6);
        else if (line.find("height:") == 0) b.height = line.substr(8);
        else if (line.find("total:") == 0)  b.total = line.substr(7);
        else if (line.find("time:") == 0)   b.time = line.substr(6);
        else if (line.find("relayed_by:") == 0) b.relayed_by = line.substr(12);
        else if (line.find("prev_block:") == 0) b.prev_block = line.substr(12);
    }

    return b;
}
void print_block(const Block& b) {
    cout << "hash: " << b.hash << endl;
    cout << "height: " << b.height << endl;
    cout << "total: " << b.total << endl;
    cout << "time: " << b.time << endl;
    cout << "relayed_by: " << b.relayed_by << endl;
    cout << "prev_block: " << b.prev_block << endl;
}
void addBlock(Block*& head, const Block& newBlock) {
    Block* newNode = new Block(newBlock);
    newNode->next = nullptr;

    if (!head) {
        head = newNode;
    } else {
        Block* curr = head;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = newNode;
    }
}

void loadBlocks(Block*& head, const string& directory, int count) {
    for (int i = 0; i < count; ++i) {
        fs::path filepath = directory + "/block_" + to_string(i) + ".txt";
        Block b = parse_block(filepath);
        addBlock(head, b);
    }
}


void freeBlocks(Block* head) {
    while (head) {
        Block* temp = head;
        head = head->next;
        delete temp;
    }
}

