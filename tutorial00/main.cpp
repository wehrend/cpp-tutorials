#include <iostream>
#include <limits>
using namespace std;

int main() {
    cout << "Limits for integer types:" << "\n";
    cout << "int minimum = " << numeric_limits<int>::min() << "\n";
    cout << "int maximum = " <<  numeric_limits<int>::max() << "\n";
    cout << "long minimum = " << numeric_limits<long>::min() << "\n";
    cout << "long minimum = " << numeric_limits<long>::max()  << "\n";
    cout << "long long minimum = " << numeric_limits<long long>::min() << "\n"; 
    cout << "long long maximum = " << numeric_limits<long long>::max() << "\n";
    cout << "unsigned maxima (minimum is 0):\n";
    cout << "unsigned int = " << numeric_limits<unsigned int>::max() << "\n";
    cout << "unsigned long = " << numeric_limits<unsigned long>::max() << "\n";
    cout << "unsigned long long = " << numeric_limits<unsigned long long>::max() << "\n";
    cout << "\nNumber of bytes for:\n";
    cout << "int        " << sizeof(int) << "\n";
    cout << "long        " << sizeof(long) << "\n";
    cout << "long long   " << sizeof(long long) << "\n";
}