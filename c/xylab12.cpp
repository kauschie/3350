
//  # Name: Michael Kausch
//  # Assignment: xylab12
//  # Class: CMPS 3350
//  # Professor: Gordon
//  # Date: 4/18/23


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <iomanip>

using namespace std;

int main (int argc, char * argv[])
{
    string line, name;
    string name_prev = "";
    int count = 0;
    int name_count = 0;
    int count_hash[20]; 
    string fname;

    for (int i = 0; i < 20; i++) {
        count_hash[i] = 0;
    }

    if (argc > 1) {
        // unit test
        cout << "running unit test on " << argv[1] << endl;
        fname = argv[1];
    } else {
        // create a file by scraping
        cout << "scraping data from w command..." << endl;
        system("PROCPS_USERLEN=16 w -h | sort > x.txt");
        fname = "x.txt";
    }

    ifstream fin(fname);
    

    if (!fin) {
        cerr << "Error grabbing data and file" << endl;
        return 1;
    }

    // get number of names in list
    while (fin >> name) {
        // cout << name << endl;
        count++;
        fin.ignore(20000, '\n');
    }

    // cout << "count is: " << count << endl;

    // reset file
    fin.clear();
    fin.seekg(0, ios::beg);

    // make array
    string names[count];

    int i = 0;
    while (fin >> name) {
        // cout << "second time: " << name << endl;
        names[i++] = name;
        fin.ignore(20000, '\n');
    }
    
    name_prev = names[0]; 
    name_count = 1;
    // bool unwritten_read = false;

    int j;
    for (j = 1; j < count; j++) {
        // cout << "next: " << names[j] << endl;
        if (name_prev != names[j]) {
            count_hash[name_count] += 1;
            name_prev = names[j];
            name_count = 1;
        } else {
            name_count++;
        }
    }
    count_hash[name_count] +=1;

    cout << "\nOdin current login statistics" << endl;
    cout <<     "-----------------------------" << endl;

    for (int k = 1; k < 20; k++) {
        if (count_hash[k] != 0) {
            cout << k;
            if (k > 1) {
                cout << " logins:\t";
            } else {
                cout << " login:\t";
            }

            cout << count_hash[k] << " user";

            if (count_hash[k] > 1) {
                cout << "s\n";
            } else {
                cout << "\n";
            }
        }
    }

    cout << endl;
    fin.close();

    return 0;
}
