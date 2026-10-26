#include <iostream>
using namespace std;

char mayorLetras(char a, char b) { return (a > b ? a : b); }
int mayorNumeros(int a, int b)   { return (a > b ? a : b); }

template <typename Tipo>
Tipo mayorDe(Tipo a, Tipo b) { return (a > b ? a : b); }

int main()
{
    char x = mayorDe<char>('e', 'z');
    int y = mayorDe<int>(9, 4);
    double z = mayorDe<double>(3.5, 2.1);

    cout << "caracter mayor: " << x << endl;   // z
    cout << "numero mayor: " << y << endl;     // 9
    cout << "double mayor: " << z << endl;     // 3.5
    return 0;
}