#include <iostream>
using namespace std;

template<typename mitipo>
mitipo aritmetica(mitipo x, mitipo y) {
    return x + y;
}

int main()
{
    cout << aritmetica<int>(10, 45) << endl;           // 55
    cout << aritmetica<double>(3.45, 8.10) << endl;    // 11.55
    cout << aritmetica<float>(0.14f, 0.147f) << endl;  // 0.287
    return 0;
}
