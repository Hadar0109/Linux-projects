#include "utils.h"
#include "functions.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
    Block* head = nullptr;
    string directory = string(getenv("HOME")) + "/Ex1/data";
    refreshBlocks(head, directory);
    freeBlocks(head);
    return 0;
}

