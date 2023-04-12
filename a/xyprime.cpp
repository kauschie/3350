/* 
 * Name: Michael Kausch
 * Assignment: xylab10
 * File: xyprime.cpp
 * Class: CMPS 3350
 * Professor: Gordon
 * Date: 3/28/23
 */

#include "xyprime.h"

// tests if a number is prime, returns true if it is, false otherwise
bool is_prime(size_t num)
{
    // start at 1 less
    size_t start = num-1;

    for (size_t i = start; i > 1; i--) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

// calculates sum of all ascii values in a word
int word_sum(string word)
{
    unsigned int letter_sum = 0;
    for (size_t i = 0; i < word.length(); i++) {
        letter_sum += (int)word[i];
    }

    return letter_sum;
}