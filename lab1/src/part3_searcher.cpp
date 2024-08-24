#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv)
{
    if(argc != 5)
    {
        cout <<"usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position>\nprovided arguments:\n";
        for(int i = 0; i < argc; i++)
            cout << argv[i] << "\n";
        return -1;
    }
    
    char *file_to_search_in = argv[1];
    char *pattern_to_search_for = argv[2];
    int search_start_position = atoi(argv[3]);
    int search_end_position = atoi(argv[4]);

    int pattern_len = strlen(pattern_to_search_for);
    ifstream file(file_to_search_in);
    file.seekg(search_start_position);

    // Create shared memory to track if the pattern has been found
    int *found_flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *found_flag = 0;  // Initialize found flag to 0 (not found)

    char c;
    int i = search_start_position;
    int j = 0;

    while(file.get(c) && i <= search_end_position){
        if(c == pattern_to_search_for[j]){
            j++;
            if(j == pattern_len){
                // If pattern is found, set the found flag
                *found_flag = 1;
                cout << "[" << getpid() << "] " << "found at " << (i - pattern_len + 1) << endl;
                return 1;
            }
        } else {
            j = 0;
            if(c == pattern_to_search_for[j]){
                j++;
            }
        }
        i++;
    }

    // Check if another process has already found the pattern
    if (*found_flag == 0) {
        cout << "[" << getpid() << "] " << "didn't find" << endl;
    }

    return 0;
}
