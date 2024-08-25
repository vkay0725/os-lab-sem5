#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv) {
    if (argc != 6) {
        cerr << "Usage: ./part2_partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position> <max-chunk-size>\n";
        return -1;
    }

    char *file_to_search_in = argv[1];
    char *pattern_to_search_for = argv[2];
    int search_start_position = atoi(argv[3]);
    int search_end_position = atoi(argv[4]);
    int max_chunk_size = atoi(argv[5]);

    int chunk_size = search_end_position - search_start_position + 1;

    cout << "Process " << getpid() << " started, searching in range " << search_start_position << " - " << search_end_position << "\n";

    if (chunk_size > max_chunk_size) {
        int mid = search_start_position + (chunk_size / 2);

        pid_t left_pid = fork();
        if (left_pid == 0) {
            char mid_str[20]; // child process for left half
            snprintf(mid_str, sizeof(mid_str), "%d", mid);
            execl("./part2_partitioner.out", "part2_partitioner.out", file_to_search_in, pattern_to_search_for, argv[3], mid_str, to_string(max_chunk_size).c_str(), NULL);
            cerr << "Failed to execute left partitioner.\n";
            return -1;
        } else {
            pid_t right_pid = fork();
            if (right_pid == 0) { //child process for the right half
                char mid_str[20];
                snprintf(mid_str, sizeof(mid_str), "%d", mid + 1); // +1 to avoid overlapping
                execl("./part2_partitioner.out", "part2_partitioner.out", file_to_search_in, pattern_to_search_for, mid_str, argv[4], to_string(max_chunk_size).c_str(), NULL);
                cerr << "Failed to execute right partitioner.\n"; //c_str(): provides  const char* ptr to internal char array of the string; suitable for execl
                return -1;
            } else {
                int status; //parent process waits for both children to finish
                waitpid(left_pid, &status, 0);
                waitpid(right_pid, &status, 0);
            }
        }
    } else {
        char start_str[20]; //if chunk size is within limit, perform the search
        char end_str[20];
        snprintf(start_str, sizeof(start_str), "%d", search_start_position); // convert int to string
        snprintf(end_str, sizeof(end_str), "%d", search_end_position);
        execl("./part2_searcher.out", "part2_searcher.out", file_to_search_in, pattern_to_search_for, start_str, end_str, NULL);
        cerr << "Failed to execute searcher.\n";
        return -1;
    }

    return 0;
}
