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

// получение индекса
int getindex(char symbol, vector<para> vec) {
    for(int i = 0; i < vec.size(); i++)
        if(symbol == vec[i].first) return i + 2;
    return -1;
}

// запись бита
void putbit(ui bit, ui &bitl, char &fbit, FILE *out) {
    fbit >>= 1;
    if(bit) fbit |= (1 << 7);
    bitl--;
    if(bitl == 0) {
        fputc(fbit, out);
        bitl = 8;
    }
}

// запись битов для однозначного кодирования
void followbits(ui bit, ui &fbits, ui &bitl, char &wbit, FILE *out) {
    putbit(bit, bitl, wbit, out);
    while(fbits > 0) {
        putbit(!bit, bitl, wbit, out);
        fbits--;
    }
}

// кодирование
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

    in = fopen(file.c_str(), "rb");
    FILE *out = fopen((file + ".encoded").c_str(), "wb");

    char clet = vec.size();
    fputc(clet, out);

    // запись частоты
    for(int i = 0; i < 256; i++) {
        if(alph[i]) {
            fputc((char)i, out);
            fwrite((char*)&alph[i], sizeof(alph[i]), 1, out);
        }
    }

    ui low = 0; // нижняя граница
    ui high = ((ui)1 << codebits) - 1; // верхнаяя граница
    ui div = inter[vec.size() + 1]; // делитель
    ui diff = high - low + 1; // для новых границ
    ui qtr1 = (high + 1) / 4; // первая четверть
    ui half = qtr1 * 2; // вторая четверть
    ui qtr3 = qtr1 * 3; // третья четверть
    ui fbits = 0; // текущий байт
    ui bitl = 8; // длина байта
    char wbit = 0; // байт для записи

    int index = 0;
    while(!feof(in)) {
        ch = fgetc(in);
        if(!feof(in)) {
            index = getindex(ch, vec);
            high = low + inter[index] * diff / div - 1;
            low = low + inter[index - 1] * diff / div;
            while(1) {
                if(high < half) followbits(0, fbits, bitl, wbit, out);
                else if(low >= half) {
                    followbits(1, fbits, bitl, wbit, out);
                    low -= half;
                    high -= half;
                } else if(low >= qtr1 && high < qtr3) {
                    fbits++;
                    low -= qtr1;
                    high -= qtr1;
                } else break;
                low += low;
                high += high + 1;
            }
        } else {
            high = low + inter[1] * diff / div - 1;
            low = low + inter[0] * diff / div;
            while(1) {
                if(high < half) followbits(0, fbits, bitl, wbit, out);
                else if(low >= half) {
                    followbits(1, fbits, bitl, wbit, out);
                    low -= half;
                    high -= half;
                } else if(low >= qtr1 && high < qtr3) {
                    fbits++;
                    low -= qtr1;
                    high -= qtr1;
                } else break;
                low += low;
                high += high + 1;
            }
            fbits++;
            if(low < qtr1) followbits(0, fbits, bitl, wbit, out);
            else followbits(1, fbits, bitl, wbit, out);
            wbit >>= bitl;
            fputc(wbit, out);
        }
        diff = high - low + 1;
    }
    fclose(in);
    fclose(out);

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