#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <fstream>

using namespace std;

void run_searcher(const string& filename, const string& pattern, int start, int end)
{
    pid_t searcher_pid = fork();
     if (searcher_pid == 0) {
        // Child process for running part2-searcher
        const char *file_to_search_in = filename.c_str();
        const char *pattern_to_search_for = pattern.c_str();
        
        // Convert integer arguments to strings
        char start_str[12], end_str[12];
        snprintf(start_str, sizeof(start_str), "%d", start);
        snprintf(end_str, sizeof(end_str), "%d", end);
        
        // Prepare arguments for the external searcher program
        char *args[] = {
            (char*)"./part2_searcher.out",   // Path to the external searcher executable
            (char*)file_to_search_in,
            (char*)pattern_to_search_for,
            start_str,
            end_str,
            NULL
        };
        
        // Execute the external searcher program
        execvp(args[0], args);

        // If execvp fails
        cerr << "Error executing searcher: " << strerror(errno) << "\n";
        exit(1);
    }
    else if (searcher_pid < 0) {
        cerr << "Error forking searcher process: " << strerror(errno) << "\n";
        exit(1);
    }
    else {
        // Parent process logs the forked searcher PID
        cout << "[" << getpid() << "] forked searcher child " << searcher_pid << "\n";
        
        // Wait for the searcher child process to complete
        int status;
        if (waitpid(searcher_pid, &status, 0) == -1) {
            cerr << "Error waiting for child process: " << strerror(errno) << "\n";
            exit(1);
        }

        // Log that the searcher child process has finished
        cout << "[" << getpid() << "] searcher child " << " returned\n";
    }
}
void partition_file(const string& filename, const string& pattern, int start, int end, int max_chunk_size)
{
    cout << "[" << getpid() << "] start position = " << start << " ; end position = " << end << "\n";

    if (end - start + 1 <= max_chunk_size) {
        // Perform search in the current chunk
        run_searcher(filename, pattern, start, end);
    }
    else {
        // Partition further
        int mid = (start + end) / 2;
        pid_t left_child = fork();

        if (left_child == 0) {
            partition_file(filename, pattern, start, mid, max_chunk_size);
            exit(0);
        }
        else if (left_child > 0) {
            cout << "[" << getpid() << "] forked left child " << left_child << "\n";

            pid_t right_child = fork();
            if (right_child == 0) {
                partition_file(filename, pattern, mid + 1, end, max_chunk_size);
                exit(0);
            }
            else if (right_child > 0) {
                cout << "[" << getpid() << "] forked right child " << right_child << "\n";

                // Wait for both child processes to complete
                int status;
                pid_t terminated_child;

                // Wait for the left child
                terminated_child = waitpid(left_child, &status, 0);
                if (terminated_child == left_child) {
                    if (WIFEXITED(status)) {
                        cout << "[" << getpid() << "] left child returned\n";
                    } else {
                        cerr << "[" << getpid() << "] left child terminated abnormally\n";
                    }
                } else {
                    cerr << "[" << getpid() << "] Error waiting for left child: " << strerror(errno) << "\n";
                }

                // Wait for the right child
                terminated_child = waitpid(right_child, &status, 0);
                if (terminated_child == right_child) {
                    if (WIFEXITED(status)) {
                        cout << "[" << getpid() << "] right child returned\n";
                    } else {
                        cerr << "[" << getpid() << "] right child terminated abnormally\n";
                    }
                } else {
                    cerr << "[" << getpid() << "] Error waiting for right child: " << strerror(errno) << "\n";
                }
            } else {
                cerr << "[" << getpid() << "] Error forking right child process\n";
            }
        } else {
            cerr << "[" << getpid() << "] Error forking left child process\n";
        }
    }
}


int main(int argc, char **argv)
{
    if(argc != 6) {
        cout << "usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position> <max-chunk-size>\n";
        for(int i = 0; i < argc; i++)
            cout << argv[i] << "\n";
        return -1;
    }

    string file_to_search_in = argv[1];
    string pattern_to_search_for = argv[2];
    int search_start_position = atoi(argv[3]);
    int search_end_position = atoi(argv[4]);
    int max_chunk_size = atoi(argv[5]);

    partition_file(file_to_search_in, pattern_to_search_for, search_start_position, search_end_position, max_chunk_size);

    return 0;
}
