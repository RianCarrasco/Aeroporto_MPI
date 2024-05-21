#include <iostream>

using namespace std;

int main() {
    int num_1 = 0;
    int num_2 = 2;
    string numero;
    int result;

    numero = to_string(num_1) + to_string(num_2);
    result = stoi(numero);
    cout << numero << endl;
    cout << result << endl;
    printf("%.2i",result);
    return 0;
}