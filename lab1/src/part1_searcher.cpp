#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>

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

	//TODO
	int pattern_len = strlen(pattern_to_search_for);
	ifstream file(file_to_search_in);
	//to start the file reading from  given search_start_position not from start of the file 
    file.seekg(search_start_position);

	char c;
	int i = search_start_position;
	int j = 0;
	while(file.get(c) && i<=search_end_position){
		if(c==pattern_to_search_for[j]){
			j++;
			if(j==pattern_len){
				cout << "[" << getpid() << "] " << "found at " << (i-pattern_len+1)<<endl;
				return 1;
			}
		}
		else{
			j=0;
			if(c==pattern_to_search_for[j]){
			j++;
		}
	}
		i++;
	}
	cout << "["<<getpid()<<"]"<<" didn't find"<<endl;
	return 0;
}
