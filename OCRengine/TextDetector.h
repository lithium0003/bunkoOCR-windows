#pragma once

#include <onnxruntime_cxx_api.h>

#include <Windows.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
#undef min
#undef max

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>

class TextDetector {
    const std::array<int64_t, 4> inputShape = { 1, 512, 512, 3 };
    const std::array<int64_t, 4> mapsOutputShape = { 1, 256, 256, 11 };
    const std::array<int64_t, 4> featureOutputShape = { 1, 256, 256, 64 };

    const std::array<const char*, 1> inputNames = { "image_input" };
    const std::array<const char*, 2> outputNames = { "maps", "feature" };

    Ort::Env env;
    Ort::SessionOptions sessionOptions;
    Ort::Session session;
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value inputTensor;
    std::vector<Ort::Value> outputTensors;

    std::array<float, 512 * 512 * 3> input;
    std::array<float, 256 * 256 * 11> maps;
    std::array<float, 256 * 256 * 64> feature;

    std::vector<float> locations;
    std::vector<float> glyphfeatures;

    int run();
    void load_im(int xi, int yi, int width, int height, const Gdiplus::BitmapData& data);
    void find_peaks(std::vector<int>& peaks, std::vector<float>& keymap_p, int xi, int yi, int width, int height);
    void process_peaks(const std::vector<int>& peaks, const std::vector<float>& keymap_p, int xi, int yi, int width, int height);
    void filter_boxes(std::vector<int>& selected_idx);

public:
    std::vector<float> lineimage;
    std::vector<float> sepimage;
    int im_width = 0;
    int im_heiht = 0;

    TextDetector();
    int run_model(std::vector<float>& loc, std::vector<float>& features, int width, int height, const Gdiplus::BitmapData &data);
};
