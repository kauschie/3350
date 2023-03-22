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
#include <cstring>
#include <sys/ioctl.h>

using namespace std;

const char fname[] = "/home/fac/gordon/public_html/3350/dictionary.txt";

extern bool check_letter (string s, char c);
extern bool check_word (string word, string username = "mkausch");
extern void sort_letters (string & s);
extern void remove_duplicates(vector<string> &v);
void print_words(vector<string> &v);

int main (int argc, char * argv[])
{
    //string file_name = "dict.txt";
    ifstream fin(fname);
    ofstream fout("dict_sorted.txt");
    string s_word;
    string uname = "mkausch";
    int dict_count = 0;
    int name_count = 0;
    int to_print = 0;
    vector<string> test = {"mike", "mike"};

    vector<string> words;
    vector<string> sorted_words;


    if (argc > 1)
        uname = argv[1];
    if (argc > 2)
        to_print = atoi(argv[2]);
    if (!fin) {
        cerr << "ERROR opening file" << endl;
        return 1;
    }

    // add words from file to a vector that holds all dict words

    while (fin >> s_word) {
        words.push_back(s_word);
    }

    // add all words that have only letters from uname 
    for (auto it2 = words.begin(); it2 != words.end(); it2++) {
        dict_count++;

        if ( check_word(*it2, uname) ) {
            name_count++;
            sorted_words.push_back(*it2);
        }
    }


    sort(sorted_words.begin(), sorted_words.end());
    cout << "there were " << sorted_words.size() << " words with " << uname  
            << " in it.\n";

    if (to_print) {
        cout << "\nsorted words are: " << endl;
        print_words(sorted_words);

    }

    cout << endl << endl;


    // sort letters of each word
    for (auto it2 = sorted_words.begin(); it2 != sorted_words.end(); it2++) {
        sort_letters(*it2);
    }
    
    // sort vector of words that are sorted by letter
    sort(sorted_words.begin(), sorted_words.end());
    sorted_words.erase( unique( sorted_words.begin(), 
                                sorted_words.end() ), 
                                    sorted_words.end() );

    cout << "there were " << sorted_words.size() << " words sorted by " <<
            " letters that were unique with " << uname << " in it.\n";
    
    if (to_print) {
        cout << "The word were...\n\n";
        print_words(sorted_words);
    }

    cout << "\n\ndict_sorted has been written..." << endl;
    fin.close();
    fout.close();

    return 0;
}

void print_words(vector<string> &v)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    int letter_count = 0;
    for (auto it = v.begin(); it != v.end(); it++) {
        if ((letter_count + (*it).size()) + 1 <= w.ws_col) {
            cout << *it << " ";
            letter_count += (*it).size() + 1;
        }
        else {
            cout << endl << *it << " ";
            letter_count = (*it).size() + 1;
        }
    }
}
