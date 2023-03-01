
/* 
 * Name: Michael Kausch
 * Assignment: xylab6b - substring.cpp
 * Class: CMPS 3350
 * Professor: Gordon
 * Date: 4/28/23
 */

//#ifndef _SUBSTRING_CPP_
//#define _SUBSTRING_CPP_

#include <iostream>
using namespace std;




bool substring_search(string s, string pattern) {

    bool ret = false;

    size_t found = s.find(pattern);
    if (found != string::npos) {
        //cout << "found " << ip << " for user " << username << endl;

        ret = true;
    }
    return ret;
}


//#endif
