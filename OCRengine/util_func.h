#pragma once

#include <vector>
#include <array>

const std::array<int, 3> modulo_list = { 1091, 1093, 1097 };

int argmax(const std::vector<float>& logits);
int argmax(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end);

int modulo(int a, int b, int n);
unsigned int decode_id(const std::array<int, 3>& b);
