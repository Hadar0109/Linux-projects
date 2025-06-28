#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "utils.h"
#include <string>
using namespace std;

void print_all_blocks(Block* head);
void find_by_hash(Block* head, const string& target_hash);
void find_by_height(Block* head, const string& target_height);
void export_to_csv(Block* head);
int countFiles(const string& directory);
void refreshBlocks(Block*& head, const string& directory);

#endif

