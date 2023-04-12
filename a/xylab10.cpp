/* 
 * Name: Michael Kausch
 * Assignment: xylab10 - Midterm
 * Class: CMPS 3350
 * Professor: Gordon
 * Date: 3/28/23
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "xyprime.h"

using namespace std;

const char fname[] = "/home/fac/gordon/public_html/3350/dictionary.txt";


int main (int argc, char * argv[])
{
    //string file_name = "dict.txt";
    ifstream fin(fname);

    // check file-path is correct
    if (!fin) {
        cout << "error locating file at path " << endl;
        cout << "(( " << fname << " ))" << endl;
        cout << "quitting now..." << endl;
        return 4;
    }

    bool DEBUG = true;  // dumps debug to log.txt file if true
    ofstream fout;

    if (DEBUG) {
        fout.open("log.txt");
        if (!fout) 
            // turn debug log off if it had a problem opening the file
            DEBUG = false;  
    }

    string word;
    string last_processed_word;
    string longest_word = "";

    size_t ascii_total = 0;
    size_t ascii_max;
    size_t num_primes = 0;
    size_t longest_prime = 0;
    size_t word_ascii_sum = 0;


    // check command-line arguments are correct
    if (argc == 2) {
        ascii_max = atoi(argv[1]);
        if (DEBUG) fout << "ascii_max set to: " << ascii_max << endl;
        if (ascii_max < 0) {
            cout << "error bad input. Please ;se a positive value or one\n"
                << "small enough to fit into an unsigned int\n"
                << "quitting now..." << endl;
            return 1;
        }

    } else {
        cout << "Improper use of program" << endl;
        cout << "correct usage: ./xylab7 <positive integer>" << endl;
        cout << "quitting now..." << endl;
        return 2;
    } 



    cout << "Program to sum ASCII values of words." << endl;
    cout << "Reading unti " << ascii_max << endl << endl;
    cout << "processing now..." << endl << endl;

    // continuously reads words from the dictionary
    while ((ascii_total <= ascii_max) && fin >> word) {
        // accumulate ascii sum after adding word

        if (DEBUG) fout << "ascii total was: " << ascii_total << endl;
        if (DEBUG) fout << "looking at word: " << word << endl;

        word_ascii_sum = word_sum(word);

        ascii_total += word_ascii_sum;

        if (DEBUG) fout << "ascii total now: " << ascii_total << endl;

        // test if prime
        if (is_prime(word_ascii_sum)) {

            num_primes++;
            if (DEBUG) fout << "num primes now: " << num_primes << endl;

            // if prime, test if it's the longest prime added word
            if (word.length() > longest_word.length()) {
                longest_word = word;
                longest_prime = word_ascii_sum;

                if (DEBUG) fout << "new longest word: " 
                                    << longest_word << endl;
                if (DEBUG) fout << "prime for it was: " 
                                    << longest_prime << endl;
            }
        }
    }


    /********************************
     *   
     *   Printing Output
     * 
     ********************************/

    int column_width = 35;
    cout << setw(column_width) << right << "Last word found: ";
    cout << left << word << endl;
    cout << setw(column_width) << right << "at total: ";
    cout << left << ascii_total << endl;
    cout << setw(column_width) << right << "Number of primes found: ";
    cout << left << num_primes << endl;
    cout << setw(column_width) << right << "Longest word producing a prime: ";
    cout << left << longest_word << endl;
    cout << setw(column_width) << right << "The prime number is: ";
    cout << left << longest_prime << endl << endl;

    cout << "program complete." << endl;

    fin.close();

    if (DEBUG) {
        fout.close();
    }


    return 0;
}
