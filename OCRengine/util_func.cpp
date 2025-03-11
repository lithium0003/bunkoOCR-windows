#include "util_func.h"

#include <algorithm>
#include <numeric>

#include <onnxruntime_cxx_api.h>

template<typename T>
int argmax(const std::vector<T>& logits)
{
    float maxval = -INFINITY;
    int maxidx = 0;
    for (int i = 0; i < logits.size(); i++) {
        float value = float(logits[i]);
        if (value > maxval) {
            maxval = value;
            maxidx = i;
        }
    }
    return maxidx;
}

template int argmax(const std::vector<float>& logints);
template int argmax(const std::vector<Ort::Float16_t>& logints);

std::vector<float> softmax(const std::vector<float>& logits, int last_dim)
{
    int len_dim = logits.size() / last_dim;
    std::vector<float> mx(len_dim, -INFINITY);
    std::vector<float> numerator(logits.size());
    std::vector<float> denominator(len_dim, 0);
    auto it = logits.begin();
    for (int i = 0; i < len_dim; i++) {
        for (int j = 0; j < last_dim; j++, ++it) {
            if (mx[i] < *it) {
                mx[i] = *it;
            }
        }
    }
    auto src = logits.begin();
    auto dst = numerator.begin();
    for (int i = 0; i < len_dim; i++) {
        for (int j = 0; j < last_dim; j++, ++src, ++dst) {
            *dst = expf(*src - mx[i]);
            denominator[i] += *dst;
        }
    }
    auto it2 = numerator.begin();
    for (int i = 0; i < len_dim; i++) {
        for (int j = 0; j < last_dim; j++, ++it2) {
            *it2 /= denominator[i];
        }
    }
    return numerator;
}

std::vector<int> top_k(const std::vector<float>& probs, int last_dim, int topk)
{
    std::vector<int> idx(probs.size());
    std::iota(idx.begin(), idx.end(), 0);
    int len_dim = probs.size() / last_dim;
    auto it = idx.begin();
    for (int i = 0; i < len_dim; i++, it+=last_dim) {
        std::partial_sort(it, it + topk, it + last_dim, [&](const auto a, const auto b) { return probs[a] > probs[b]; });
    }
    for (auto& x : idx) {
        x %= last_dim;
    }
    return idx;
}


int modulo(int a, int b, int n)
{
    int x = 1;
    int y = a;
    while (b > 0) {
        if ((b % 2) == 1) {
            x = (x * y) % n;
        }
        y = (y * y) % n;
        b /= 2;
    }
    return x % n;
}

unsigned int decode_id(const std::array<int, 3>& b)
{
    std::array<int, 3> t;
    for (int k = 0; k < 3; k++) {
        int u = 0;
        for (int j = 0; j < k; j++) {
            int w = t[j];
            for (int i = 0; i < j; i++) {
                w *= modulo_list[i];
            }
            u += w;
        }
        int tk = (b[k] - u) % modulo_list[k];
        for (int j = 0; j < k; j++) {
            tk *= modulo(modulo_list[j], modulo_list[k] - 2, modulo_list[k]);
        }
        tk = tk % modulo_list[k];
        t[k] = tk;
    }
    unsigned int x = 0;
    for (int k = 0; k < 3; k++) {
        int w = t[k];
        for (int i = 0; i < k; i++) {
            w *= modulo_list[i];
        }
        x += w;
    }
    int mk = 1;
    for (auto mi : modulo_list) {
        mk *= mi;
    }
    x = x % mk;
    return x;
}

std::string UTF32toUTF8(unsigned int code)
{
    if (code < 0) {
        return "";
    }
    std::stringstream ss;
    if (code <= 0x7F) {
        ss << unsigned char(code);
        return ss.str();
    }
    if (code <= 0x07FF) {
        //                    110x xxxx
        unsigned char byte1 = 0xC0 | ((code & 0x07C0) >> 6);
        //                    10xx xxxx
        unsigned char byte2 = 0x80 | (code & 0x003F);
        ss << byte1 << byte2;
        return ss.str();
    }
    if (code <= 0xFFFF) {
        //                    1110 xxxx
        unsigned char byte1 = 0xE0 | ((code & 0xf000) >> 12);
        //                    10xx xxxx
        unsigned char byte2 = 0x80 | ((code & 0x0FC0) >> 6);
        //                    10xx xxxx
        unsigned char byte3 = 0x80 | (code & 0x003F);
        ss << byte1 << byte2 << byte3;
        return ss.str();
    }
    if (code <= 0x10FFFF) {
        //                    1111 0xxx
        unsigned char byte1 = 0xF0 | ((code & 0x1C0000) >> 18);
        //                    10xx xxxx
        unsigned char byte2 = 0x80 | ((code & 0x3F000) >> 12);
        //                    10xx xxxx
        unsigned char byte3 = 0x80 | ((code & 0x0FC0) >> 6);
        //                    10xx xxxx
        unsigned char byte4 = 0x80 | (code & 0x003F);
        ss << byte1 << byte2 << byte3 << byte4;
        return ss.str();
    }
    return "";
}

std::string escape_char(unsigned int c)
{
    switch (c) {
    case '"':
        return "\\\"";
    case '\\':
        return "\\\\";
    case '/':
        return "\\/";
    case '\b':
        return "\\b";
    case '\f':
        return "\\f";
    case '\n':
        return "\\n";
    case '\r':
        return "\\r";
    case '\t':
        return "\\t";
    }
    if (c < 0x1F) {
        const char* table = "0123456789ABCDEF";
        std::string s = "\\u";
        s += table[(c & 0xF000) >> 12];
        s += table[(c & 0x0F00) >> 8];
        s += table[(c & 0x00F0) >> 4];
        s += table[c & 0x000F];
        return s;
    }
    return UTF32toUTF8(c);
}

std::string escape_char(std::vector<unsigned int> codes)
{
    std::string result = "";
    for (const auto& c : codes) {
        result += escape_char(c);
    }
    return result;
}

void WriteJson(std::ostream& stream, const OCR_info& info)
{
    stream << "{" << std::endl;

    stream << " \"block\": [" << std::endl;
    for (int i = 0; i < info.block.size(); i++) {
        auto block = info.block[i];
        stream << "  {" << std::endl;

        stream << "   \"blockidx\": " << block.blockidx << "," << std::endl;
        stream << "   \"text\": \"" << escape_char(block.text) << "\"" << "," << std::endl;
        stream << "   \"vertical\": " << block.vertical << "," << std::endl;
        stream << "   \"x1\": " << block.x1 << "," << std::endl;
        stream << "   \"x2\": " << block.x2 << "," << std::endl;
        stream << "   \"y1\": " << block.y1 << "," << std::endl;
        stream << "   \"y2\": " << block.y2 << std::endl;

        if (i < info.block.size() - 1) {
            stream << "  }," << std::endl;
        }
        else {
            stream << "  }" << std::endl;
        }
    }
    stream << " ]," << std::endl;

    stream << " \"box\": [" << std::endl;
    for (int i = 0; i < info.box.size(); i++) {
        auto box = info.box[i];
        stream << "  {" << std::endl;

        stream << "   \"blockidx\": " << box.blockidx << "," << std::endl;
        stream << "   \"lineidx\": " << box.lineidx << "," << std::endl;
        stream << "   \"subidx\": " << box.subidx << "," << std::endl;
        stream << "   \"text\": \"" << escape_char(box.text) << "\"" << "," << std::endl;
        stream << "   \"vertical\": " << box.vertical << "," << std::endl;
        stream << "   \"cx\": " << box.cx << "," << std::endl;
        stream << "   \"cy\": " << box.cy << "," << std::endl;
        stream << "   \"w\": " << box.w << "," << std::endl;
        stream << "   \"h\": " << box.h << "," << std::endl;
        stream << "   \"ruby\": " << box.ruby << "," << std::endl;
        stream << "   \"rubybase\": " << box.rubybase << "," << std::endl;
        stream << "   \"emphasis\": " << box.emphasis << std::endl;

        if (i < info.box.size() - 1) {
            stream << "  }," << std::endl;
        }
        else {
            stream << "  }" << std::endl;
        }
    }
    stream << " ]," << std::endl;

    stream << " \"line\": [" << std::endl;
    for (int i = 0; i < info.line.size(); i++) {
        auto line = info.line[i];
        stream << "  {" << std::endl;

        stream << "   \"blockidx\": " << line.blockidx << "," << std::endl;
        stream << "   \"lineidx\": " << line.lineidx << "," << std::endl;
        stream << "   \"text\": \"" << escape_char(line.text) << "\"" << "," << std::endl;
        stream << "   \"vertical\": " << line.vertical << "," << std::endl;
        stream << "   \"x1\": " << line.x1 << "," << std::endl;
        stream << "   \"x2\": " << line.x2 << "," << std::endl;
        stream << "   \"y1\": " << line.y1 << "," << std::endl;
        stream << "   \"y2\": " << line.y2 << std::endl;

        if (i < info.line.size() - 1) {
            stream << "  }," << std::endl;
        }
        else {
            stream << "  }" << std::endl;
        }
    }
    stream << " ]," << std::endl;

    stream << " \"text\": \"" << escape_char(info.text) << "\"" << std::endl;

    stream << "}" << std::endl;
}

std::wstring ToUnicodeStr(const std::string& utf8str)
{
    int needLength = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
    std::wstring result;
    if (needLength > 0) {
        result.resize(needLength - 1);
        MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, &result[0], result.size());
    }
    return result;
}
