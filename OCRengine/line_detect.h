#pragma once

#include <vector>

std::vector<int> line_detect(int im_width, int im_height, const std::vector<float>& loc, const std::vector<float>& lineimage, const std::vector<float>& sepimage);
