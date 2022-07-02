#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;
#define codebits 16
typedef unsigned int ui;
typedef pair<char, ui> para;

bool comp(para &l, para &r) {
    if(l.second != r.second) return l.second >= r.second;
    return l.first < r.first;
}

void decode(string file) {


    ifstream noncod(file, ios::binary);
    ifstream decode(file + ".decoded", ios::binary);

    // сравнение файлов
    char a, b; unsigned int equal = 0;
    while(noncod.get(a) && decode.get(b))
        if(a != b) equal++;

    while(noncod.get(a)) equal++;
    while(decode.get(b)) equal++;

    cout << "unequal bytes: " << equal << endl;

    noncod.close();
    decode.close();
}

int main() {
    string file;
    cout << "Enter file's name: ";
    cin >> file;
    decode(file);
    system("pause");
}