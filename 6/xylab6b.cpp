/* 
 * Name: Michael Kausch
 * Assignment: xylab6b
 * Class: CMPS 3350
 * Professor: Gordon
 * Date: 4/28/23
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <limits>
#include <iomanip>

using namespace std;

extern bool substring_search(string s, string pattern); 

int main(int argc, char * argv[])
{
    system("PROCPS_USERLEN=15 w > myfile.txt");
    usleep(10000); // makes sure all housekeeping is done before using the file
    ifstream fin("myfile.txt");


    if (!fin) {
        cerr << "ERROR opening file" << endl;
        return 1;
    }

    cout << "file opened successfully\n";


    char line[200];
    string username;
    string pattern;
    string ignore;
    string ip;

    fin.getline(line, 200);
    fin.getline(line, 200);

    bool is_found;

    while (fin >> username >> ignore >> ip) {

        pattern = "136.168";

        is_found = substring_search(ip, pattern);

        if (is_found) {
            cout << "found " << setw(15) << left << ip 
                << " for user " << username << endl;
        }

        // ignore the rest of the line
        fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
    }
    
    fin.close();

    unlink("myfile.txt");

    return 0;
}
