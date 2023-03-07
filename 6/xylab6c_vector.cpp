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
#include <vector>
#include <algorithm>

using namespace std;

const char fname[] = "/home/fac/gordon/public_html/3350/dictionary.txt";

int main(int argc, char * argv[])
{
    //string file_name = "dict.txt";
    ifstream fin(fname);
    ofstream fout("dict_sorted.txt");
    string word;
    vector<string> words;

    if (!fin) {
        cerr << "ERROR opening file" << endl;
        return 1;
    }

    //cout << "file opened successfully\n";

    while (fin >> word) {

        words.push_back(word);
    }

    cout << "Was the file sorted? " << boolalpha
            << is_sorted(words.begin(), words.end()) << endl;

    sort(words.begin(), words.end());

    cout << "Is the file now sorted? " << boolalpha
            << is_sorted(words.begin(), words.end()) << endl;
    
    for (auto it = words.begin(); it != words.end(); it++) {
        
        fout << *it << endl;
    }
    
    cout << "dict_sorted has been written..." << endl;
    

    fin.close();
    fout.close();

    return 0;
}
