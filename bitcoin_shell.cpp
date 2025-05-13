#include "utils.h"
#include "functions.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>

using namespace std;
namespace fs = std::filesystem;

int main() {
    Block* head = nullptr;
    string directory = string(getenv("HOME")) + "/Ex1/data";
    int num_blocks;
    cout << "Enter number of blocks to fetch: ";
    cin >> num_blocks;

    cout << "Fetching initial blocks..." << endl;
    string command = "./fetch_blocks.sh " + to_string(num_blocks);
    system(command.c_str());

    int count = countFiles(directory);
    loadBlocks(head, directory, count);

    while (true) {
        cout << "\n--- Menu ---\n";
        cout << "1. Print all blocks\n";
        cout << "2. Find block by hash\n";
        cout << "3. Find block by height\n";
        cout << "4. Export to CSV\n";
        cout << "5. Refresh data\n";
        cout << "Choose option: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            print_all_blocks(head);
        } else if (choice == 2) {
            string hash;
            cout << "Enter hash: ";
            cin >> hash;
            find_by_hash(head, hash);
        } else if (choice == 3) {
            string height;
            cout << "Enter height: ";
            cin >> height;
            find_by_height(head, height);
        } else if (choice == 4) {
            export_to_csv(head);
        } else if (choice == 5) {
            refreshBlocks(head, directory);
        } else {
            cout << "Invalid option." << endl;
        }
    }

    freeBlocks(head);
    return 0;
}

