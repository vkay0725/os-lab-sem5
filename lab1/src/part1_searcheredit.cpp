#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        cout << "usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position>\nprovided arguments:\n";
        for (int i = 0; i < argc; i++)
            cout << argv[i] << "\n";
        return -1;
    }

    char *file_to_search_in = argv[1];
    char *pattern_to_search_for = argv[2];
    int search_start_position = atoi(argv[3]);
    int search_end_position = atoi(argv[4]);

    int pattern_len = strlen(pattern_to_search_for);
    ifstream file(file_to_search_in);

    if (!file) {
        cerr << "Error opening file" << endl;
        return -1;
    }

    file.seekg(search_start_position);

    string window;
    char c;
    int i = search_start_position;

    for (int k = 0; k < pattern_len && i <= search_end_position && file.get(c); ++k, ++i) {
        window += c;
    }

    while (i <= search_end_position) {
        if (window == pattern_to_search_for) {
            cout << "[" << getpid() << "] found at " << (i - pattern_len) << endl;
            return 1;
        }

        window.erase(0, 1);
        if (file.get(c)) {
            window += c;
            i++;
        } else {
            break;
        }
    }

    if (window == pattern_to_search_for) {
        cout << "[" << getpid() << "] found at " << (i - pattern_len) << endl;
        return 1;
    }

    cout << "[" << getpid() << "] didn't find" << endl;
    return 0;
}
