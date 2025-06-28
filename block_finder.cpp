#include "utils.h"
#include "functions.h"
#include <iostream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " --hash <value> OR --height <value>\n";
        return 1;
    }

    string option = argv[1];
    string value = argv[2];

    Block* head = nullptr;
    string directory = string(getenv("HOME")) + "/Ex1/data";
    
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
      cout << "You have no blocks to find" << endl;
      return 1;
    }
    
    int count = countFiles(directory);
    loadBlocks(head, directory, count);

    if (option == "--hash") {
        find_by_hash(head, value);
    } else if (option == "--height") {
        find_by_height(head, value);
    } else {
        cout << "Invalid option. Use --hash or --height.\n";
        freeBlocks(head);
        return 1;
    }

    freeBlocks(head);
    return 0;
}


