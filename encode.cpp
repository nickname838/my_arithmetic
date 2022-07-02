#include <iostream>
#include <vector>
#include <algorithm>
#include <sys/stat.h>

using namespace std;
#define codebits 16
typedef unsigned int ui;
typedef pair<char, ui> para;

// для сортировки
bool comp(para &l, para &r) {
    if(l.second != r.second) return l.second >= r.second;
    return l.first < r.first;
}

void encode(string file) {
    ui *alph = new ui[256] {};
    FILE *in = fopen(file.c_str(), "rb");
    if(!in) {puts("input doesn't exist"); return;}

    // алфавит
    char ch = 0;
    while(!feof(in)) {
        ch = fgetc(in);
        if(!feof(in)) alph[ch]++;
    }
    fclose(in);

    // вектор частот
    vector<para> vec;
    for(int i = 0; i < 256; i++)
        if(alph[i]) vec.push_back(make_pair((char)i, alph[i]));
    sort(vec.begin(), vec.end(), comp);

    for(auto pair: vec) cout << pair.first << " " << pair.second << endl;

    // границы
    unsigned short *inter = new unsigned short[vec.size() + 2];
    inter[0] = 0;
    inter[1] = 1;
    for(int i = 0; i < vec.size(); i++) {
        ui a = vec[i].second;
        for(int j = 0; j < i; j++) a += vec[j].second;
        inter[i + 2] = a;
    }

    if(inter[vec.size()] > (1 << (codebits - 2) - 1)) {puts("freq error"); return;}

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