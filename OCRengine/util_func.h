#pragma once

#include <vector>
#include <array>

#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#undef min
#undef max

struct feature_idxinfo {
    int id;
    int block;
    int idx;
    int subidx;
    int subtype;
};

struct boxinfo {
    int blockidx;
    int lineidx;
    int subidx;
    std::vector<unsigned int> text;
    int ruby;
    int rubybase;
    int emphasis;
    int vertical;
    float cx;
    float cy;
    float w;
    float h;
};

struct blockinfo {
    int blockidx;
    std::vector<unsigned int> text;
    int vertical;
    float x1;
    float y1;
    float x2;
    float y2;
};

struct lineinfo {
    int blockidx;
    int lineidx;
    std::vector<unsigned int> text;
    int vertical;
    float x1;
    float y1;
    float x2;
    float y2;
};

struct OCR_info {
    std::vector<boxinfo> box;
    std::vector<blockinfo> block;
    std::vector<lineinfo> line;
    std::vector<unsigned int> text;
};

const std::array<int, 3> modulo_list = { 1091, 1093, 1097 };

template<typename T> int argmax(const std::vector<T>& logits);

std::vector<float> softmax(const std::vector<float>& logits, int last_dim);
std::vector<int> top_k(const std::vector<float>& probs, int last_dim, int topk);

int modulo(int a, int b, int n);
unsigned int decode_id(const std::array<int, 3>& b);

void WriteJson(std::ostream& stream, const OCR_info& info);
std::wstring ToUnicodeStr(const std::string& utf8str);
