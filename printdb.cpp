#include "utils.h"
#include "functions.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    Block* head = nullptr;
    string directory = string(getenv("HOME")) + "/Ex1/data";
    
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
      cout << "You have no blocks to print" << endl;
      return 1;
    }
    
    int count = countFiles(directory);
    loadBlocks(head, directory, count);
    print_all_blocks(head);
    freeBlocks(head);
    return 0;
}

