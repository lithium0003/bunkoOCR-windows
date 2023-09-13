#pragma once

#include <onnxruntime_cxx_api.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>

class CodeDecoder {
    const std::array<int64_t, 2> inputShape = { 1, 64 };
    const std::array<int64_t, 2> mod1091outputShape = { 1, 1091 };
    const std::array<int64_t, 2> mod1093outputShape = { 1, 1093 };
    const std::array<int64_t, 2> mod1097outputShape = { 1, 1097 };

    const std::array<const char*, 1> inputNames = { "feature_input" };
    const std::array<const char*, 3> outputNames = { "mod_1091", "mod_1093", "mod_1097" };

    Ort::Env env;
    Ort::SessionOptions sessionOptions;
    Ort::Session session;
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value inputTensor;
    std::vector<Ort::Value> outputTensors;

    std::array<float, 64> input;
    std::vector<float> mod1091;
    std::vector<float> mod1093;
    std::vector<float> mod1097;
    
    int run();
public:
    CodeDecoder();
    int run_model(std::vector<unsigned int>& code, const std::vector<float>& features);
};
