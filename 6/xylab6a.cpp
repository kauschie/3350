/* 
 * Name: Michael Kausch
 * Assignment: xylab6a
 * Class: CMPS 3350
 * Professor: Gordon
 * Date: 4/28/23
 */

#include <iostream>

using namespace std;

#ifdef TESTING
int run_times = 0;
#endif


int main (int argc, char * argv[])
{

    int n = 5;
    int j = 0, k = 0;

    int test_count = 10;
    //int run_times = 0;

    // use second argument, the first is always the program name
    if (argc > 1) {
        n = atoi(argv[1]);
    }

    int sum = 0;

    for (int m = 0; m < 10; m++) {
        k = 0;
        do
        {
            j = 0;
            while (j < test_count) {

                sum = 0;
                for (int i = 1; i <=n; i++) {
                    sum += i;
                }
#ifdef TESTING
                run_times++;
#endif
                j++;
            }
            k++;

        } while (k < test_count);
    }

    cout << "The sum is " << sum << ".\n";

#ifdef TESTING
    cout << "The program ran for " << run_times << " tests.\n";
#endif

    return 0;
}
