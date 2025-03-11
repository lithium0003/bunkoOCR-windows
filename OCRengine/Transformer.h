#pragma once

#include <onnxruntime_cxx_api.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iterator>

#include "util_func.h"

class Transformer {
    const static int decoder_SOT = 1;
    const static int decoder_EOT = 2;
    const static int decoder_MSK = 3;

    const static int max_encoder_len = 100;
    const static int encoder_dim = 100 + 6;
    const static int max_decoder_len = 100;

    const std::array<int64_t, 2> decoder_inputShape = { 1, max_decoder_len };
    const std::array<int64_t, 3> encoder_outputShape = { 1, max_encoder_len, 512 };
    const std::array<int64_t, 3> encoder_inputShape = { 1, max_encoder_len, encoder_dim };
    const std::array<int64_t, 4> encoder_keymaskShape = { 1, 1, 1, max_encoder_len };

    const std::array<int64_t, 3> mod1091outputShape = { 1, max_decoder_len, 1091 };
    const std::array<int64_t, 3> mod1093outputShape = { 1, max_decoder_len, 1093 };
    const std::array<int64_t, 3> mod1097outputShape = { 1, max_decoder_len, 1097 };

    const std::array<const char*, 2> encoder_inputNames = { "encoder_input", "key_mask" };
    const std::array<const char*, 1> encoder_outputNames = { "encoder_output" };
    const std::array<const char*, 3> decoder_inputNames = { "encoder_output", "decoder_input", "key_mask" };
    const std::array<const char*, 3> decoder_outputNames = { "modulo_1091", "modulo_1093", "modulo_1097" };


    Ort::Env env;
    Ort::SessionOptions sessionOptions;
    Ort::Session session_encoder;
    Ort::Session session_decoder;
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    Ort::Value encoder_outputTensor;
    std::vector<Ort::Value> encoder_inputTensors;
    std::vector<Ort::Value> decoder_inputTensors;
    std::vector<Ort::Value> decoder_outputTensors;

    std::array<float, max_encoder_len * encoder_dim> encoder_input;
    std::array<float, max_encoder_len * 512> encoder_output;
    std::array<float, max_encoder_len> encoder_keymask;
    std::array<int64_t, max_decoder_len> decoder_input;

    std::vector<float> mod1091;
    std::vector<float> mod1093;
    std::vector<float> mod1097;

    int run_encoder();
    int run_decoder();

public:
    Transformer();
    int run_model(OCR_info& code, const std::vector<float>& features, const std::vector<float>& locations, std::vector<feature_idxinfo>& feature_idx);
};
