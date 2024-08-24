#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <fstream>
#include <signal.h>
#include <vector>

using namespace std;

vector<pid_t> child_processes;  // To store PIDs of child processes
volatile sig_atomic_t pattern_found = 0;  // Flag to indicate if the pattern was found

void signal_handler(int signal) {
    if (signal == SIGTERM) {
        cout << "[" << getpid() << "] received SIGTERM\n";
        exit(0);
    }
}

void run_searcher(const string& filename, const string& pattern, int start, int end) {
    pid_t searcher_pid = fork();
    if (searcher_pid == 0) {
        // Child process for running part3-searcher
        signal(SIGTERM, signal_handler);  // Set up signal handler

        const char *file_to_search_in = filename.c_str();
        const char *pattern_to_search_for = pattern.c_str();

        // Convert integer arguments to strings
        char start_str[12], end_str[12];
        snprintf(start_str, sizeof(start_str), "%d", start);
        snprintf(end_str, sizeof(end_str), "%d", end);

        // Prepare arguments for the external searcher program
        char *args[] = {
            (char*)"./part3_searcher.out",   // Path to the external searcher executable
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

        // Add child PID to the list
        child_processes.push_back(searcher_pid);

        // Wait for the searcher child process to complete
        int status;
        if (waitpid(searcher_pid, &status, 0) == -1) {
            cerr << "Error waiting for child process: " << strerror(errno) << "\n";
            exit(1);
        }

        // If the child process found the pattern
        if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
            cout << "[" << getpid() << "] searcher child found the pattern\n";
            pattern_found = 1;  // Set the flag

            // Kill all other children
            for (pid_t pid : child_processes) {
                if (pid != searcher_pid) {
                    kill(pid, SIGTERM);
                }
            }
        }

        // Log that the searcher child process has finished only if no other process has found the pattern
        if (!pattern_found) {
            cout << "[" << getpid() << "] searcher child " << searcher_pid << " returned\n";
        }
    }
}

void partition_file(const string& filename, const string& pattern, int start, int end, int max_chunk_size) {
    // Check if the pattern has already been found
    if (pattern_found) return;

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
            // Set up signal handler
            signal(SIGTERM, signal_handler);
            partition_file(filename, pattern, start, mid, max_chunk_size);
            exit(0);
        }
        else if (left_child > 0) {
            cout << "[" << getpid() << "] forked left child " << left_child << "\n";
            child_processes.push_back(left_child);

            pid_t right_child = fork();
            if (right_child == 0) {
                // Set up signal handler
                signal(SIGTERM, signal_handler);
                partition_file(filename, pattern, mid + 1, end, max_chunk_size);
                exit(0);
            }
            else if (right_child > 0) {
                cout << "[" << getpid() << "] forked right child " << right_child << "\n";
                child_processes.push_back(right_child);

                // Wait for both child processes to complete
                int status;
                pid_t terminated_child;

                // Wait for the left child
                terminated_child = waitpid(left_child, &status, 0);
                if (terminated_child == left_child) {
                    if (WIFEXITED(status)) {
                        cout << "[" << getpid() << "] left child returned\n";
                        if (WEXITSTATUS(status) == 1) {
                            // Pattern found by left child, kill the right child
                            kill(right_child, SIGTERM);
                        }
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
                        if (WEXITSTATUS(status) == 1) {
                            // Pattern found by right child, kill the left child
                            kill(left_child, SIGTERM);
                        }
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

int main(int argc, char **argv) {
    if(argc != 6) {
        cout << "usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position> <max-chunk-size>\n";
        return -1;
    }

    string file_to_search_in = argv[1];
    string pattern_to_search_for = argv[2];
    int search_start_position = atoi(argv[3]);
    int search_end_position = atoi(argv[4]);
    int max_chunk_size = atoi(argv[5]);

    // Check if the pattern has already been found before starting the partitioning
    if (!pattern_found) {
        partition_file(file_to_search_in, pattern_to_search_for, search_start_position, search_end_position, max_chunk_size);
    }

    return 0;
}
