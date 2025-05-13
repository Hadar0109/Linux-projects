#include "utils.h"
#include "functions.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    Block* head = nullptr;
    string directory = string(getenv("HOME")) + "/Ex1/data";
    
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
      cout << "You have no blocks to export" << endl;
      return 1;
    }
    
    int count = countFiles(directory);
    loadBlocks(head, directory, count);
    export_to_csv(head);
    freeBlocks(head);
    return 0;
}

