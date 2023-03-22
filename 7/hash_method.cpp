#include <iostream>
using namespace std;

int main()
{

    int hash[128] = {0};

    char * u = username;

    while (*u) {
        hash[(int)*u] = 1;
        ++u;
    }

    char * w = line;

    int found = 1;
    while (*w) {
        if (hash[(int)*w] == 0) {
            found = 0;
            break;
        }
        ++w;
    }

    if (found) {
        // add to found array
    }

    return 0;
}
