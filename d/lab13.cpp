// Michael Kausch
// Code during class on 4/25/23
// lab13.cpp

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

typedef float Myfloat;

string replaceSubstring(string original, string substring, string replacement)
{
    // 012345678901234
    //      i
    // salt and pepper
    // salt & pepper
    // start of string, end of string, 

    // if (i == 0 || (i == (original.size() - substring.size())) || (original[i-1] = " " && original[i+substring.size()] == " ") )

    string new_string = "";
    bool will_replace = true;

    // do whole string
    for (size_t i = 0; i < original.size(); i++) {
        // check if current area is substring
        for (size_t j = 0; j < substring.size(); j++) {
            // cout << "comparing " << original[i+j] << " with " << substring[j] << endl;
            if (original[i+j] != substring[j]) {
                // cout << "\tnot a match" << endl;
                // if it doesn't match move to the next postion
                will_replace = false;
                break;
            }
        }

        if (will_replace) {
            for (size_t k = 0; k < replacement.size(); k++) {
                new_string += replacement[k];
            }
            
            // cout << "new string " << new_string << endl;
            i += substring.size() - 1;
        } else {
            new_string += original[i];
            will_replace = true;
        }
    }


    return new_string;
}

int main(int argc, char* argv[])
{
    string phrase;
    string new_phrase;

    cout << "Enter a phrase: " << endl;
    getline(cin, phrase);

    // Search and replace "and" with "&"
    
    new_phrase = replaceSubstring(phrase, "and", "&");

    cout << "new phrase: ";
    cout << "**" << new_phrase << "**" << endl;
    
    return 0;
}