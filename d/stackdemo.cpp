// Michael Kausch
// Code during class on 4/26/23
// stackdemo.cpp

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

int stack[1000];
size_t top = 0;

void push(int value)
{
    stack[top++] = value;
}

int pop()
{
    return stack[(top--)-1];
}

void printStack()
{
    cout << "Current stack:" << endl;
    for (int i = top-1; i >= 0; i--) {
        cout << "\t" << stack[i] << endl;
    }
}



int main(int argc, char* argv[])
{

    int data;

    push(9);
    data = pop();

    cout << "data: " << data << endl;

    int x = 35;

    // swap data and x using only push and pop

    push(data);
    push(x);
    data = pop();
    x = pop();

    cout << "should be 35, 9: " << data << ", " << x;
    cout << boolalpha << "\t(( " << (data == 35 && x == 9) << " ))" << endl;


    /*
    
    for (int i = 0; i < 10; i++) {
        push(i);
    }

    printStack();

    for (int i = 0; i < 10; i++) {
        cout << "Popping: " << pop() << endl;
        printStack();
    }
    
    */



    return 0;
}
