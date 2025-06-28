#include "functions.h"
#include <iostream>
#include <fstream>

using namespace std;

void print_all_blocks(Block* head) {
    Block* curr = head;
    while (curr) {
        print_block(*curr);
        cout << "      |" << endl;
        cout << "      |" << endl;
        cout << "      V" << endl << endl;
        curr = curr->next;
    }
}

void find_by_hash(Block* head, const string& target_hash) {
    Block* curr = head;
    while (curr) {
        if (curr->hash == target_hash) {
            print_block(*curr);
            return;
        }
        curr = curr->next;
    }
    cout << "Block with hash not found." << endl;
}

void find_by_height(Block* head, const string& target_height) {
    Block* curr = head;
    while (curr) {
        if (curr->height == target_height) {
            print_block(*curr);
            return;
        }
        curr = curr->next;
    }
    cout << "Block with height not found." << endl;
}

void export_to_csv(Block* head) {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;
    
    ofstream out(filename);
    out << "hash,height,total,time,relayed_by,prev_block\n";  // header

    Block* curr = head;
    while (curr) {
        out << curr->hash << "," << curr->height << "," << curr->total << "," << curr->time << ","
            << curr->relayed_by << "," << curr->prev_block << "\n";
        curr = curr->next;
    }
    
    cout << "Exported to " << filename << endl;
}

int countFiles(const string& directory) {
    int count = 0;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".txt")
            ++count;
    }
    return count;
}

void refreshBlocks(Block*& head, const string& directory) {
    freeBlocks(head);
    head = nullptr;

    std::filesystem::create_directories(directory);

    int num_blocks;
    cout << "Enter number of blocks to fetch: ";
    cin >> num_blocks;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.path().extension() == ".txt")
            std::filesystem::remove(entry.path());
    }

    cout << "Fetching blocks..." << endl;
    string command = "./fetch_blocks.sh " + to_string(num_blocks);
    system(command.c_str());

    int count = countFiles(directory);
    if (count != num_blocks) {
        cerr << "Warning: Expected " << num_blocks << " blocks, but found " << count << " in directory." << endl;
    }

    loadBlocks(head, directory, count);
    cout << "Reload complete." << endl;
}

