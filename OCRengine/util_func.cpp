#include "util_func.h"

int argmax(const std::vector<float>& logits)
{
    float maxval = -INFINITY;
    int maxidx = 0;
    for (int i = 0; i < logits.size(); i++) {
        if (logits[i] > maxval) {
            maxval = logits[i];
            maxidx = i;
        }
    }
    return maxidx;
}

int argmax(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end)
{
    float maxval = -INFINITY;
    int maxidx = 0;
    int i = 0;
    for (auto it = begin; it != end; ++it, i++) {
        if (*it > maxval) {
            maxval = *it;
            maxidx = i;
        }
    }
    return maxidx;
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
