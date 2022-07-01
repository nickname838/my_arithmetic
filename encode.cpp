#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

using namespace std;

void encode(string file) {
    

    // сравнение размеров
    struct stat sb; int bsize = 0;
    struct stat se; int esize = 0;

    stat(file.c_str(), &sb); bsize = sb.st_size;
    stat((file + ".encoded").c_str(), &se); esize = se.st_size;

    cout << "compression: " << (bsize + 0.0) / esize << endl;
}

int main() {
    string file;
    cout << "Enter file's name: ";
    cin >> file;
    encode(file);
    system("pause");
}