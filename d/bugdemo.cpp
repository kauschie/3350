// Michael Kausch
// Code during class on 4/24/23
// bugdemo.cpp

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

typedef float Myfloat;


int main(int argc, char* argv[])
{
    Myfloat val = 0.0;
    val += 0.1;
    val += 0.1;
    val += 0.1;
    val += 0.1;
    val += 0.1;
    val += 0.1;
    val += 0.1;
    val += 0.1;
    val += 0.1;
    val += 0.1;

    Myfloat tolerance = 0.0001;
    Myfloat diff = fabs(val - 1.0);

    cout << setprecision(20) << "val: " << val << endl;
    if (diff <= tolerance) {
        cout << "Yes, equal to 0.1" << endl;
    } else {
        cout << "ERROR - not qual to 0.1" << endl;
    }

    return 0;
}