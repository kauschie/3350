#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>

using namespace std;
string makeFileName(string f);

int main()
{
    string fnames[] = {"x.txt", "x1.txt", "x2.txt", "x3.txt"};
    for (auto fin : fnames) {
        system(makeFileName(fin).c_str());
    }
}

string makeFileName(string f)
{
    ostringstream temp;
    temp << "./lab12 " << f;
    return temp.str();
}
