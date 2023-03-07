 //# Name: Michael Kausch
 //# Assignment: xylab7
 //# Class: CMPS 3350
 //# Professor: Gordon
 //# Date: 3/7/23
 

#include <algorithm>
#include <iostream>
using namespace std;

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

