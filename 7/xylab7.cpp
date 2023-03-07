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

using namespace std;

const char fname[] = "/home/fac/gordon/public_html/3350/dictionary.txt";

bool check_letter (string s, char c);
bool check_word (string word, string username = "mkausch");
void sort_letters (string & s);
void remove_duplicates(vector<string> &v);

int main (int argc, char * argv[])
{
    //string file_name = "dict.txt";
    ifstream fin(fname);
    ofstream fout("dict_sorted.txt");
    string s_word;
    string uname = "gzepeda";
    int dict_count = 0;
    int name_count = 0;
    vector<string> test = {"mike", "mike"};

    vector<string> words;
    vector<string> sorted_words;


    if (argc > 1)
        uname = argv[1];

    if (!fin) {
        cerr << "ERROR opening file" << endl;
        return 1;
    }

    //cout << "file opened successfully\n";

    while (fin >> s_word) {
        words.push_back(s_word);
    }

    //cout << "There are " << words.size() << " words in the vector.\n";

    /*
       string word = "maam";

       cout << "maam is in mkausch" << boolalpha << check_word(word, uname) << endl;
       */

    //    sort_letters(word);


    for (auto it2 = words.begin(); it2 != words.end(); it2++) {
        dict_count++;

        if ( check_word(*it2, uname) ) {
            name_count++;
            sorted_words.push_back(*it2);
        }
    }

    sort(sorted_words.begin(), sorted_words.end());
    cout << "there were " << sorted_words.size() << " words in the dict.\n";
    cout << "sorted words are: " << endl;

    for (auto it2 = sorted_words.begin(); it2 != sorted_words.end(); it2++) {
            cout << *it2 << " ";
    }

    cout << endl << endl;


    for (auto it2 = sorted_words.begin(); it2 != sorted_words.end(); it2++) {
        sort_letters(*it2);
    }
    
    sort(sorted_words.begin(), sorted_words.end());

    remove_duplicates(sorted_words);

    /*
    cout << "sorted words with sorted letters without duplicates are: " << endl;
    for (auto it2 = sorted_words.begin(); it2 != sorted_words.end(); it2++) {
        sort_letters(*it2);
        cout << *it2 << " ";
        fout << *it2 << endl;
    }
    */

    remove_duplicates(sorted_words);

    /*
    for (auto it2 = sorted_words.begin(); it2 != sorted_words.end(); it2++) {
        cout << *it2 << endl;
    }
    */
    
    sort( sorted_words.begin(), sorted_words.end() );
    sorted_words.erase( unique( sorted_words.begin(), sorted_words.end() ), sorted_words.end() );

    for (auto it2 = sorted_words.begin(); it2 != sorted_words.end(); it2++) {
        cout << *it2 << " ";
    }

    cout << "\n\ndict_sorted has been written..." << endl;

    //cout << "there were " << words.size() << " words in the dict.\n";
    cout << "there were " << sorted_words.size() << " words with " << uname  
        << " in it.\n";

    


    fin.close();
    fout.close();

    return 0;
}

bool check_letter (string s, char c)
{
    bool result = false;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == c) {
            result = true;
            break;
        }
    }

    return result;
}

bool check_word (string word, string username)
{
    bool result = true;
    bool is_found = true;

    for (size_t i = 0; i < word.length(); i++) {
        is_found = check_letter(username, word[i]);
        if (!is_found) {
            result = false;
            break;
        }
    }

    return result;
}

void sort_letters (string & s)
{
    int n = s.length();
    //char * cstr = s.c_str();    
    char tmp;
    /*
       cout << "starting out with " << s << endl;
       cout << s << " is now ";
       */

    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-1; j++) {
            if (s[j] > s[j+1]) {
                // swap
                tmp = s[j];
                s[j] = s[j+1];
                s[j+1] = tmp;
                //strcpy(tmp, s[j]);
                //strcpy(s[j], s[j+1]);
                //strcpy(s[j+1], tmp);
            }
        }
    }

    /*
       cout << s << endl;
       */

}


bool is_duplicate(string s1, string s2)
{
    return (s1 == s2);
}

void remove_duplicates(vector<string> &v)
{
    size_t size = v.size();
    for (size_t i = 0; i < size; i++) {
       // cout << "looking at " << v[i] << " and " << v[i+1] << endl;
        //if ( v[i] == "aeln" )
            //cin.get();

        if (v[i] == v[i+1]) {
           // cout << "found duplicate and removing " << v[i] << endl;
            auto it = v.begin() + i;
            v.erase(it);
            size = v.size();
        }
    }
}
