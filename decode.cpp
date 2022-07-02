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

// чтение бита
int getbit(char &inbit, ui &bitl, FILE *in, ui &ubit) {
    if(bitl == 0) {
        inbit = fgetc(in);
        if(feof(in)) {
            ubit++;
            if(ubit > 14) {
                puts("can't decode1");
                exit(1);
            }
        }
        bitl = 8;
    }
    int res = inbit & 1;
    inbit >>= 1;
    bitl--;
    return res;
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

    ui low = 0;
    ui high = ((ui)1 << codebits) - 1;
    ui div = inter[vec.size() + 1];
    ui qtr1 = (high + 1) / 4;
    ui half = qtr1 * 2;
    ui qtr3 = qtr1 * 3;
    ui bitl = 0;
    char inbit = 0;
    ui ubit = 0;
    unsigned short codev = 0;
    int tmp = 0;

    FILE *out = fopen((file.erase(file.find(".encoded")) + ".decoded").c_str(), "wb");
    
    for(int i = 1; i <= 16; i++) {
        tmp = getbit(inbit, bitl, in, ubit);
        codev = 2 * codev + tmp;
    }
    ui diff = high - low + 1;
    while(1) {
        ui freq = (ui)((((ui)codev - low + 1) * div - 1) / diff);
        int i;
        for(i = 1; inter[i] <= freq; i++);
        high = low + inter[i] * diff / div - 1;
        low = low + inter[i - 1] * diff / div;
        while(1) {
            if(high < half);
            else if(low >= half) {
                low -= half;
                high -= half;
                codev -= half;
            } else if(low >= qtr1 && high < qtr3) {
                low -= qtr1;
                high -= qtr1;
                codev -=qtr1;
            } else break;
            low += low;
            high += high + 1;
            tmp = 0;
            tmp = getbit(inbit, bitl, in, ubit);
            codev += codev + tmp;
        }
        if(i == 1) break;
        fputc(vec[i - 2].first, out);
        diff = high - low + 1;
    }
    fclose(in);
    fclose(out);

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