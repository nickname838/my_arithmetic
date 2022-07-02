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
    ui *alph = new ui[256] {};
    FILE *in = fopen(file.c_str(), "rb");
    if(!in) {puts("input doesn't exist"); return;}

    // чтение частот
    char count, ch;
    count = fgetc(in);
    for(int i = 0; i < count; i++) {
        ch = fgetc(in);
        if(!feof(in)) fread((char*)&alph[ch], sizeof(alph[ch]), 1, in);
        else {puts("can't decode2"); return;}
    }

    // вектор частот
    vector<para> vec;
    for(int i = 0; i < 256; i++)
        if(alph[i]) vec.push_back(make_pair((char)i, alph[i]));
    sort(vec.begin(), vec.end(), comp);

    for(auto pair: vec) cout << pair.first << ' ' << pair.second << endl;

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