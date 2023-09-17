#include "CodeDecoder.h"
#include <onnxruntime/core/providers/dml/dml_provider_factory.h>

#include "util_func.h"

extern bool useDirectML;
extern int useDirectML_idx;

CodeDecoder::CodeDecoder():
    env(ORT_LOGGING_LEVEL_FATAL),
    session(env, L"CodeDecoder.onnx", sessionOptions),
    inputTensor(Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), inputShape.data(), inputShape.size())),
    mod1091(1091), mod1093(1093), mod1097(1097)
{
    bool success = false;
    if (!success) {
        success = true;
        try {
            auto api = Ort::GetApi();
            OrtTensorRTProviderOptionsV2* tensorrt_options = nullptr;
            Ort::ThrowOnError(api.CreateTensorRTProviderOptions(&tensorrt_options));
            std::vector<const char*> keys{ "trt_max_workspace_size", "trt_fp16_enable", "trt_engine_cache_enable", "trt_engine_cache_path" };
            std::vector<const char*> values{ "4294967296", "1", "1", "cache" };
            Ort::ThrowOnError(api.UpdateTensorRTProviderOptions(tensorrt_options, keys.data(), values.data(), 4));
            OrtCUDAProviderOptionsV2* cuda_options = nullptr;
            Ort::ThrowOnError(api.CreateCUDAProviderOptions(&cuda_options));
            sessionOptions = Ort::SessionOptions();
            sessionOptions.AppendExecutionProvider_TensorRT_V2(*tensorrt_options);
            sessionOptions.AppendExecutionProvider_CUDA_V2(*cuda_options);
            session = Ort::Session(env, L"CodeDecoder.onnx", sessionOptions);
            std::cout << "tensorRT" << std::endl;
        }
        catch (...) {
            success = false;
        }
    }

    if (!success && useDirectML) {
        success = true;
        try {
            sessionOptions = Ort::SessionOptions();
            sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_ALL);
            sessionOptions.DisableMemPattern();
            sessionOptions.SetExecutionMode(ORT_SEQUENTIAL);
            Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_DML(sessionOptions, useDirectML_idx));
            session = Ort::Session(env, L"CodeDecoder.onnx", sessionOptions);
            std::cout << "DirectML" << std::endl;
        }
        catch (...) {
            success = false;
        }
    }

    if (!success) {
        success = true;
        try {
            OrtOpenVINOProviderOptions options;
            options.device_type = "CPU_FP32";
            options.cache_dir = "cache";
            sessionOptions = Ort::SessionOptions();
            sessionOptions.AppendExecutionProvider_OpenVINO(options);
            sessionOptions.SetGraphOptimizationLevel(ORT_DISABLE_ALL);
            session = Ort::Session(env, L"CodeDecoder.onnx", sessionOptions);
            std::cout << "OpenVINO" << std::endl;
        }
        catch (...) {
            success = false;
        }
    }
    outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1091.data(), mod1091.size(), mod1091outputShape.data(), mod1091outputShape.size()));
    outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1093.data(), mod1093.size(), mod1093outputShape.data(), mod1093outputShape.size()));
    outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1097.data(), mod1097.size(), mod1097outputShape.data(), mod1097outputShape.size()));
}

int CodeDecoder::run()
{
    Ort::RunOptions runOptions;
    try {
        session.Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), outputTensors.data(), 3);
    }
    catch (Ort::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int CodeDecoder::run_model(std::vector<unsigned int>& code, const std::vector<float>& features)
{
    int count = features.size() / 64;
    for (int i = 0; i < count; i++) {
        std::copy(features.begin() + (i * 64), features.begin() + ((i + 1) * 64), input.begin());

        if (run() != 0) {
            return 1;
        }

        int id1091 = argmax(mod1091);
        int id1093 = argmax(mod1093);
        int id1097 = argmax(mod1097);

        auto pred_id = decode_id({ id1091, id1093, id1097 });
        std::cerr << pred_id << std::endl;

        code.push_back(pred_id);
    }

    return 0;
}

