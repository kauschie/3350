/* 
 * Name: Michael Kausch
 * Assignment: xylab6c
 * Class: CMPS 3350
 * Professor: Gordon
 * Date: 4/28/23
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include <algorithm>
#include <cstring>

using namespace std;

const char fname[] = "/home/fac/gordon/public_html/3350/dictionary.txt";
char ** arr;

int main(int argc, char * argv[])
{
    //string file_name = "dict.txt";
    int output = 0;
    int sort = 0;
    if (argc > 2) {
        output = 1;
        sort = 1;
    }

    ifstream fin(fname);
    ofstream fout("dict_sorted.txt");
    char line[100], prevline[100];
    int count = 0;

    fin >> line;
    ++count;

    if (!fin) {
        cerr << "ERROR opening file" << endl;
        return 1;
    }

    //cout << "file opened successfully\n";

    while (!fin.eof()) {
        if (count > 1) {
            if (strcmp(line, prevline) < 0) {
                if (output == 1) {
                    cout << "out of order " << endl; 
                    cout << line << " !< " << prevline << endl;
                }
            }
        }
        strcpy(prevline, line);
        fin >> line;
        ++count;
    }

    fin.clear();
    int n = 0;
    arr = new char * [count];
    fin.seekg(0,ios::beg);

    while (!fin.eof()) {

        arr[n] = new char[strlen(line)+1];
        strcpy(arr[n++], line);
        fin >> line;
    }

    // bubble sort
    if (sort) {
        for (int i = 0; i < n-1; i++) {
            for (int j = 0; j < n-1; j++) {
                if (strcmp(arr[j],arr[j+1]) > 0) {
                    // swap
                    char tmp[100];
                    strcpy(tmp, arr[j]);
                    strcpy(arr[j], arr[j+1]);
                    strcpy(arr[j+1], tmp);
                }
            }
        }

    }

    for (int i = 0; i < n; i++) {
        if (output == 1) {
            cout << arr[i] << " ";
        }
        fout << arr[i] << endl;
    }


    /*
       cout << "Was the file sorted? " << boolalpha
       << is_sorted(words.begin(), words.end()) << endl;


       cout << "Is the file now sorted? " << boolalpha
       << is_sorted(words.begin(), words.end()) << endl;


       cout << "dict_sorted has been written..." << endl;
       */

    for (int i = 0; i < n; i++) {
        delete [] arr[n];
    }
    
    
    delete [] arr;

    fin.close();
    fout.close();

    return 0;
}
