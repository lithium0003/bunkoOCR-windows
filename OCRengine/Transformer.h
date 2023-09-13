#pragma once

#include "onnxruntime_cxx_api.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>

class Transformer {
    const static int decoder_SOT = 1;
    const static int decoder_EOT = 2;

    const static int max_encoder_len = 128;
    const static int encoder_dim = 64 + 4;
    const static int max_decoder_len = 128;

    const std::array<int64_t, 2> decoder_inputShape = { 1, max_decoder_len };
    const std::array<int64_t, 3> encoder_outputShape = { 1, max_encoder_len, 512 };
    const std::array<int64_t, 3> encoder_inputShape = { 1, max_encoder_len, encoder_dim };

    const std::array<int64_t, 2> mod1091outputShape = { max_decoder_len, 1091 };
    const std::array<int64_t, 2> mod1093outputShape = { max_decoder_len, 1093 };
    const std::array<int64_t, 2> mod1097outputShape = { max_decoder_len, 1097 };

    const std::array<const char*, 1> encoder_inputNames = { "encoder_input" };
    const std::array<const char*, 1> encoder_outputNames = { "encoder_output" };
    const std::array<const char*, 3> decoder_inputNames = { "decoder_input", "encoder_output", "encoder_input" };
    const std::array<const char*, 3> decoder_outputNames = { "mod1091", "mod1093", "mod1097" };


    Ort::Env env;
    Ort::SessionOptions sessionOptions;
    Ort::Session session_encoder;
    Ort::Session session_decoder;
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value encoder_inputTensor;
    Ort::Value encoder_outputTensor;
    std::vector<Ort::Value> decoder_inputTensors;
    std::vector<Ort::Value> decoder_outputTensors;

    std::array<float, max_encoder_len * encoder_dim> encoder_input;
    std::array<float, max_encoder_len * 512> encoder_output;
    std::array<float, max_decoder_len> decoder_input;

    std::vector<float> mod1091;
    std::vector<float> mod1093;
    std::vector<float> mod1097;

    int run_encoder();
    int run_decoder();

    int run_model(std::vector<unsigned int>& code, const std::vector<float>::const_iterator begin, const std::vector<float>::const_iterator end);
public:
    Transformer();
    int run_model(std::vector<unsigned int>& code, const std::vector<float>& features);
};
