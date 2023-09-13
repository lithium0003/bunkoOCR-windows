#include "Transformer.h"
#include <onnxruntime/core/providers/dml/dml_provider_factory.h>

#include "util_func.h"

extern bool useDirectML;

Transformer::Transformer():
    env(ORT_LOGGING_LEVEL_FATAL),
    session_encoder(env, L"TransformerEncoder.onnx", sessionOptions),
    session_decoder(env, L"TransformerDecoder.onnx", sessionOptions),
    encoder_inputTensor(Ort::Value::CreateTensor<float>(memory_info, encoder_input.data(), encoder_input.size(), encoder_inputShape.data(), encoder_inputShape.size())),
    encoder_outputTensor(Ort::Value::CreateTensor<float>(memory_info, encoder_output.data(), encoder_output.size(), encoder_outputShape.data(), encoder_outputShape.size())),
    mod1091(1091 * max_decoder_len), mod1093(1093 * max_decoder_len), mod1097(1097 * max_decoder_len)
{
    bool success = false;
    if (!success) {
        success = true;
        try {
            auto api = Ort::GetApi();
            OrtTensorRTProviderOptionsV2* tensorrt_options = nullptr;
            Ort::ThrowOnError(api.CreateTensorRTProviderOptions(&tensorrt_options));
            std::vector<const char*> keys{ "trt_max_workspace_size", "trt_engine_cache_enable", "trt_engine_cache_path" };
            std::vector<const char*> values{ "4294967296", "true", "cache" };
            Ort::ThrowOnError(api.UpdateTensorRTProviderOptions(tensorrt_options, keys.data(), values.data(), 3));
            OrtCUDAProviderOptionsV2* cuda_options = nullptr;
            Ort::ThrowOnError(api.CreateCUDAProviderOptions(&cuda_options));
            sessionOptions = Ort::SessionOptions();
            sessionOptions.AppendExecutionProvider_TensorRT_V2(*tensorrt_options);
            sessionOptions.AppendExecutionProvider_CUDA_V2(*cuda_options);
            session_encoder = Ort::Session(env, L"TransformerEncoder.onnx", sessionOptions);
            session_decoder = Ort::Session(env, L"TransformerDecoder.onnx", sessionOptions);
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
            Ort::ThrowOnError(OrtSessionOptionsAppendExecutionProvider_DML(sessionOptions, 0));
            session_encoder = Ort::Session(env, L"TransformerEncoder.onnx", sessionOptions);
            session_decoder = Ort::Session(env, L"TransformerDecoder.onnx", sessionOptions);
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
            session_encoder = Ort::Session(env, L"TransformerEncoder.onnx", sessionOptions);
            session_decoder = Ort::Session(env, L"TransformerDecoder.onnx", sessionOptions);
            std::cout << "OpenVINO" << std::endl;
        }
        catch (...) {
            success = false;
        }
    }

    decoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, decoder_input.data(), decoder_input.size(), decoder_inputShape.data(), decoder_inputShape.size()));
    decoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, encoder_output.data(), encoder_output.size(), encoder_outputShape.data(), encoder_outputShape.size()));
    decoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, encoder_input.data(), encoder_input.size(), encoder_inputShape.data(), encoder_inputShape.size()));
    decoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, encoder_input.data(), encoder_input.size(), encoder_inputShape.data(), encoder_inputShape.size()));

    decoder_outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1091.data(), mod1091.size(), mod1091outputShape.data(), mod1091outputShape.size()));
    decoder_outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1093.data(), mod1093.size(), mod1093outputShape.data(), mod1093outputShape.size()));
    decoder_outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1097.data(), mod1097.size(), mod1097outputShape.data(), mod1097outputShape.size()));
}

int Transformer::run_encoder()
{
    Ort::RunOptions runOptions;
    try {
        session_encoder.Run(runOptions, encoder_inputNames.data(), &encoder_inputTensor, 1, encoder_outputNames.data(), &encoder_outputTensor, 1);
    }
    catch (Ort::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int Transformer::run_decoder()
{
    Ort::RunOptions runOptions;
    try {
        session_decoder.Run(runOptions, decoder_inputNames.data(), decoder_inputTensors.data(), 3, decoder_outputNames.data(), decoder_outputTensors.data(), 3);
    }
    catch (Ort::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int Transformer::run_model(std::vector<unsigned int>& code,const std::vector<float>::const_iterator begin,const std::vector<float>::const_iterator end)
{
    if (std::distance(begin, end) > encoder_input.size()) {
        return 2;
    }
    std::fill(encoder_input.begin(), encoder_input.end(), 0);
    auto it = std::copy(begin, end, encoder_input.begin());
    if (it != encoder_input.end()) {
        std::fill(it, encoder_input.end(), 0);
    }

    if (run_encoder() != 0) {
        return 1;
    }

    std::fill(decoder_input.begin(), decoder_input.end(), 0);
    decoder_input[0] = decoder_SOT;

    for (int count = 0; count < max_decoder_len - 1; count++) {
        if (run_decoder() != 0) {
            return 1;
        }

        int id1091 = argmax(mod1091.begin() + count * 1091, mod1091.begin() + (count + 1) * 1091);
        int id1093 = argmax(mod1093.begin() + count * 1093, mod1093.begin() + (count + 1) * 1093);
        int id1097 = argmax(mod1097.begin() + count * 1097, mod1097.begin() + (count + 1) * 1097);

        auto pred_id = decode_id({ id1091, id1093, id1097 });
        decoder_input[count + 1] = pred_id;
        if (pred_id == decoder_EOT) {
            break;
        }
    }

    for (int i = 1; i < max_decoder_len - 1; i++) {
        if (decoder_input[i] == decoder_EOT) break;
        code.push_back(decoder_input[i]);
    }

    return 0;
}

int Transformer::run_model(std::vector<unsigned int> &code, const std::vector<float>& features)
{
    auto st = features.begin();
    auto st2 = features.begin();
    auto end = features.end();
    bool overlap = false;
    while (st < end) {
        std::cout << "transformer " << std::distance(features.begin(), st) / encoder_dim << "/" << std::distance(features.begin(), end) / encoder_dim << std::endl;
        int add_len = (max_encoder_len * 7 / 8)*encoder_dim;
        if (std::distance(st, end) < add_len) {
            add_len = std::distance(st, end);
        }
        auto ed = st + add_len;
        auto ed2 = ed;
        while (ed > features.begin() && *(ed - 1) == 0) {
            if (ed == st) {
                ed = ed2;
                break;
            }
            ed -= encoder_dim;
        }
        if (overlap) {
            st = st2;
        }

        std::vector<unsigned int> tmp_code;
        if (run_model(tmp_code, st, ed) != 0) {
            return 1;
        }

        if (overlap) {
            auto it = tmp_code.begin();
            int i = 0;
            while (i < 10 && it < tmp_code.end()) {
                if (*it == 10) {
                    ++it;
                    break;
                }
                i++;
                ++it;
            }
            if (i < 10) {
                std::copy(it, tmp_code.end(), std::back_inserter(code));
            }
            else {
                std::copy(tmp_code.begin(), tmp_code.end(), std::back_inserter(code));
            }
        }
        else {
            std::copy(tmp_code.begin(), tmp_code.end(), std::back_inserter(code));
        }

        if (ed > features.begin() && *(ed - 1) > 0) {
            st = ed;
            st2 = ed - encoder_dim;
            int i = 0;
            while (st2 > features.begin() && i < 5) {
                if (*(st2 - 1) > 0) {
                    break;
                }
                st2 -= encoder_dim;
                i++;
            }
            overlap = true;
        }
        else {
            st = ed;
            st2 = st;
            overlap = false;
        }
    }

    return 0;
}
