#include "Transformer.h"
#include <onnxruntime/core/providers/dml/dml_provider_factory.h>

#include "util_func.h"

extern bool useCUDA;
extern bool useTensorRT;
extern bool useDirectML;
extern int useDirectML_idx;
extern bool useOpenVINO;

Transformer::Transformer():
    env(ORT_LOGGING_LEVEL_FATAL),
    session_encoder(env, L"TransformerEncoder.onnx", sessionOptions),
    session_decoder(env, L"TransformerDecoder.onnx", sessionOptions),
    encoder_outputTensor(Ort::Value::CreateTensor<float>(memory_info, encoder_output.data(), encoder_output.size(), encoder_outputShape.data(), encoder_outputShape.size())),
    mod1091(1091 * max_decoder_len), mod1093(1093 * max_decoder_len), mod1097(1097 * max_decoder_len)
{
    bool success = false;
    if (!success && useTensorRT) {
        success = true;
        try {
            auto api = Ort::GetApi();
            OrtTensorRTProviderOptionsV2* tensorrt_options = nullptr;
            Ort::ThrowOnError(api.CreateTensorRTProviderOptions(&tensorrt_options));
            std::vector<const char*> keys{ "trt_max_workspace_size", "trt_engine_cache_enable", "trt_engine_cache_path" };
            std::vector<const char*> values{ "4294967296", "1", "cache" };
            Ort::ThrowOnError(api.UpdateTensorRTProviderOptions(tensorrt_options, keys.data(), values.data(), 3));
            sessionOptions = Ort::SessionOptions();
            sessionOptions.AppendExecutionProvider_TensorRT_V2(*tensorrt_options);
            session_encoder = Ort::Session(env, L"TransformerEncoder.onnx", sessionOptions);
            session_decoder = Ort::Session(env, L"TransformerDecoder.onnx", sessionOptions);
            std::cout << "tensorRT" << std::endl;
            api.ReleaseTensorRTProviderOptions(tensorrt_options);
        }
        catch (...) {
            success = false;
        }
    }

    if (!success && useCUDA) {
        success = true;
        try {
            auto api = Ort::GetApi();
            OrtCUDAProviderOptionsV2* cuda_options = nullptr;
            Ort::ThrowOnError(api.CreateCUDAProviderOptions(&cuda_options));
            sessionOptions = Ort::SessionOptions();
            sessionOptions.AppendExecutionProvider_CUDA_V2(*cuda_options);
            session_encoder = Ort::Session(env, L"TransformerEncoder.onnx", sessionOptions);
            session_decoder = Ort::Session(env, L"TransformerDecoder.onnx", sessionOptions);
            std::cout << "CUDA" << std::endl;
            api.ReleaseCUDAProviderOptions(cuda_options);
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
            session_encoder = Ort::Session(env, L"TransformerEncoder.onnx", sessionOptions);
            session_decoder = Ort::Session(env, L"TransformerDecoder.onnx", sessionOptions);
            std::cout << "DirectML" << std::endl;
        }
        catch (...) {
            success = false;
        }
    }

    if (!success && useOpenVINO) {
        success = true;
        try {
            std::unordered_map<std::string, std::string> options;
            options["device_type"] = "CPU";
            options["precision"] = "FP32";
            options["cache_dir"] = "cache";
            sessionOptions = Ort::SessionOptions();
            sessionOptions.AppendExecutionProvider("OpenVINO", options);
            session_encoder = Ort::Session(env, L"TransformerEncoder.onnx", sessionOptions);
            session_decoder = Ort::Session(env, L"TransformerDecoder.onnx", sessionOptions);
            std::cout << "OpenVINO" << std::endl;
        }
        catch (...) {
            success = false;
        }
    }

    encoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, encoder_input.data(), encoder_input.size(), encoder_inputShape.data(), encoder_inputShape.size()));
    encoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, encoder_keymask.data(), encoder_keymask.size(), encoder_keymaskShape.data(), encoder_keymaskShape.size()));

    decoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, encoder_output.data(), encoder_output.size(), encoder_outputShape.data(), encoder_outputShape.size()));
    decoder_inputTensors.push_back(Ort::Value::CreateTensor<int64_t>(memory_info, decoder_input.data(), decoder_input.size(), decoder_inputShape.data(), decoder_inputShape.size()));
    decoder_inputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, encoder_keymask.data(), encoder_keymask.size(), encoder_keymaskShape.data(), encoder_keymaskShape.size()));

    decoder_outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1091.data(), mod1091.size(), mod1091outputShape.data(), mod1091outputShape.size()));
    decoder_outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1093.data(), mod1093.size(), mod1093outputShape.data(), mod1093outputShape.size()));
    decoder_outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, mod1097.data(), mod1097.size(), mod1097outputShape.data(), mod1097outputShape.size()));
}

int Transformer::run_encoder()
{
    Ort::RunOptions runOptions;
    try {
        session_encoder.Run(runOptions, encoder_inputNames.data(), encoder_inputTensors.data(), encoder_inputTensors.size(), encoder_outputNames.data(), &encoder_outputTensor, 1);
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
        session_decoder.Run(runOptions, decoder_inputNames.data(), decoder_inputTensors.data(), decoder_inputTensors.size(), decoder_outputNames.data(), decoder_outputTensors.data(), decoder_outputTensors.size());
    }
    catch (Ort::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

int Transformer::run_model(OCR_info &info, const std::vector<float>& features, const std::vector<float> &locations, std::vector<feature_idxinfo>& feature_idx)
{
    std::vector<float> SOT(100 + 6);
    std::vector<float> EOT(100 + 6);
    for (int i = 0; i < 100; i++) {
        if (i % 2 == 0) {
            SOT[i] = 5;
            EOT[i] = -5;
        }
        else {
            SOT[i] = -5;
            EOT[i] = 5;
        }
    }

    struct linebufer {
        int prev_j;
        int cur_j;
        std::vector<unsigned int> predstr;
    };

    int cur_i = 0;
    int cur_j = 0;
    int prev_j = 0;
    int keep_back = 0;
    std::vector<linebufer> linebuf;
    std::vector<unsigned int> result_txt;
    int max_features = features.size() / encoder_dim;

    // 0-99 features
    // 100 vertical
    // 101 ruby (base)
    // 102 ruby (text)
    // 103 space
    // 104 emphasis
    // 105 newline

    while (cur_i < max_features) {
        int r = 0;
        int s = 0;
        for (int k = cur_i; k < std::min(cur_i + max_encoder_len - 3, max_features); k++) {
            // space
            if (features[k * encoder_dim + 103] > 0) {
                r += 1;
            }
            if (s == 0 && features[k * encoder_dim + 101] > 0) {
                // rubybase
                r += 3;
                s = 1;
            }
            else if (s == 1 && features[k * encoder_dim + 102] > 0) {
                s = 2;
            }
            else if (s == 2 && features[k * encoder_dim + 102] == 0) {
                s = 0;
            }
        }
        // set max size
        cur_j = std::min(max_features, cur_i + (max_encoder_len - 3 - r));

        // horizontal/vertical change point
        for (int j = cur_i + 1; j < cur_j; j++) {
            if (features[j * encoder_dim + 100] != features[cur_i * encoder_dim + 100]) {
                cur_j = j;
                break;
            }
        }
        // double newline (new block)
        if (cur_j < max_features - 1 && cur_i + 1 < cur_j - 1) {
            for (int j = cur_i + 1; j < cur_j - 1; j++) {
                if (features[j * encoder_dim + 105] > 0 && features[(j+1) * encoder_dim + 105] > 0) {
                    cur_j = j + 2;
                    break;
                }
            }
        }
        // ruby/rubybase separation check
        if (cur_j < max_features) {
            // last char is not newline
            if (cur_j > 1 && features[(cur_j - 1) * encoder_dim + 105] == 0) {
                for (int j = cur_j - 1; j >= cur_i + 1; j--) {
                    // ruby, rubybase
                    if (features[j * encoder_dim + 101] == 0 && features[j * encoder_dim + 102] == 0) {
                        cur_j = j + 1;
                        break;
                    }
                }
            }
        }

        // no progress check
        if (prev_j == cur_j) {
            keep_back = 0;
            cur_i = cur_j;
            continue;
        }

        std::cout << "transformer " << (double)cur_i / max_features * 100 << "% " << prev_j << " " << cur_i << " " << cur_j << "/" << max_features << std::endl;
        std::fill(encoder_input.begin(), encoder_input.end(), 0);
        auto it = std::copy(SOT.begin(), SOT.end(), encoder_input.begin());
        for (int i = cur_i; i < cur_j; i++) {
            it = std::copy(features.begin() + i * encoder_dim, features.begin() + (i + 1) * encoder_dim, it);
        }
        std::copy(EOT.begin(), EOT.end(), it);

        std::fill(encoder_keymask.begin(), encoder_keymask.begin() + (2 + cur_j - cur_i), 0);
        std::fill(encoder_keymask.begin() + (2 + cur_j - cur_i), encoder_keymask.end(), -INFINITY);

        if (run_encoder() != 0) {
            return 1;
        }

        std::fill(decoder_input.begin(), decoder_input.end(), decoder_MSK);
        decoder_input[0] = decoder_SOT;
        std::vector<unsigned int> pred_output(max_decoder_len);
        const int rep_count = 8;
        for (int k = 0; k < rep_count; k++) {
            if (run_decoder() != 0) {
                return 1;
            }

            auto prob_p1091 = softmax(mod1091, 1091);
            auto sort_i1091 = top_k(prob_p1091, 1091, 5);
            std::vector<std::pair<int, float>> list1091;
            for (int i = 0; i < max_decoder_len; i++) {
                for (int j = 0; j < 5; j++) {
                    int idx = sort_i1091[i * 1091 + j];
                    float p = prob_p1091[i * 1091 + idx];
                    list1091.emplace_back(idx, p);
                }
            }

            auto prob_p1093 = softmax(mod1093, 1093);
            auto sort_i1093 = top_k(prob_p1093, 1093, 5);
            std::vector<std::pair<int, float>> list1093;
            for (int i = 0; i < max_decoder_len; i++) {
                for (int j = 0; j < 5; j++) {
                    int idx = sort_i1093[i * 1093 + j];
                    float p = prob_p1093[i * 1093 + idx];
                    list1093.emplace_back(idx, p);
                }
            }

            auto prob_p1097 = softmax(mod1097, 1097);
            auto sort_i1097 = top_k(prob_p1097, 1097, 5);
            std::vector<std::pair<int, float>> list1097;
            for (int i = 0; i < max_decoder_len; i++) {
                for (int j = 0; j < 5; j++) {
                    int idx = sort_i1097[i * 1097 + j];
                    float p = prob_p1097[i * 1097 + idx];
                    list1097.emplace_back(idx, p);
                }
            }

            std::vector<std::pair<unsigned int, float>> p;
            for (int i = 0; i < max_decoder_len; i++) {
                std::vector<std::pair<unsigned int, float>> p1;
                for (int j1 = 0; j1 < 5; j1++) {
                    int id1091 = list1091[i * 5 + j1].first;
                    float p1091 = list1091[i * 5 + j1].second;
                    if (!p1.empty() && p1091 < 0.1) {
                        break;
                    }
                    for (int j2 = 0; j2 < 5; j2++) {
                        int id1093 = list1093[i * 5 + j2].first;
                        float p1093 = list1093[i * 5 + j2].second;
                        if (!p1.empty() && p1093 < 0.1) {
                            break;
                        }
                        for (int j3 = 0; j3 < 5; j3++) {
                            int id1097 = list1097[i * 5 + j3].first;
                            float p1097 = list1097[i * 5 + j3].second;
                            if (!p1.empty() && p1097 < 0.1) {
                                break;
                            }
                            float prob = expf((logf(p1091) + logf(p1093) + logf(1097)) / 3);
                            auto pred_id = decode_id({ id1091, id1093, id1097 });
                            p1.emplace_back(pred_id, prob);
                        }
                    }
                }
                std::sort(p1.begin(), p1.end(), [](const auto a, const auto b) { return a.second > b.second; });
                int code = p1[0].first;
                int p_code = p1[0].second;
                for (const auto& x : p1) {
                    if (x.first <= 0x3FFFF) {
                        code = x.first;
                        p_code = x.second;
                        break;
                    }
                }
                p.emplace_back(code, p_code);
            }

            for (int i = 0; i < max_decoder_len; i++) {
                pred_output[i] = p[i].first;
            }

            if (k > 0) {
                if (std::count_if(p.begin(), p.end(), [](const auto& x) { return x.first == 0 || x.second > 0.99; }) == max_decoder_len) {
                    //std::cout << "[" << k << " early stop]" << std::endl;
                    break;
                }
            }
            if (k < rep_count - 1) {
                float p_th = 1.0 - float(k + 1) / rep_count;
                for (int i = 0; i < max_decoder_len - 1; i++) {
                    if (p[i].second < p_th) {
                        decoder_input[i + 1] = decoder_MSK;
                    }
                    else {
                        decoder_input[i + 1] = p[i].first;
                    }
                }
            }
        }

        std::vector<unsigned int> predstr;
        for (auto p : pred_output) {
            if (p == 0 || p == decoder_EOT) {
                break;
            }
            if (p >= 0xD800 && p <= 0xdFFF) {
                predstr.push_back(0xFFFD);
            }
            else if (p <= 0x3FFFF) {
                predstr.push_back(p);
            }
            else {
                predstr.push_back(0xFFFD);
            }
        }

        std::vector<unsigned int> code;
        std::copy(predstr.begin() + keep_back, predstr.end(), std::back_inserter(code));
        linebuf.emplace_back(prev_j, cur_j, code);
        std::copy(predstr.begin() + keep_back, predstr.end(), std::back_inserter(result_txt));

        if (cur_j < max_features) {
            int k = cur_i - 1;
            prev_j = cur_j;
            keep_back = 0;
            while (cur_i < k) {
                // horizontal/vertical change point
                if (features[k * encoder_dim + 100] != features[cur_j * encoder_dim + 100]) {
                    k += 1;
                    break;
                }
                // ruby, rubybase
                if (features[k * encoder_dim + 101] > 0 || features[k * encoder_dim + 102] > 0) {
                    k += 1;
                    break;
                }
                // newline
                if (k < cur_i - 1 && features[k * encoder_dim + 105] > 0) {
                    k += 1;
                    break;
                }
                // space
                if (features[k * encoder_dim + 103] > 0) {
                    keep_back += 1;
                }
                if (k > cur_j - 3) {
                    k -= 1;
                }
                else {
                    break;
                }
            }
            if (cur_i < k) {
                cur_i = k;
                keep_back += cur_j - k;
            }
            else {
                keep_back = 0;
                cur_i = cur_j;
            }
        }
        else {
            break;
        }
    }

    float line_x1 = -2000;
    float line_x2 = -2000;
    float line_y1 = -2000;
    float line_y2 = -2000;
    int line_vertical = 0;
    int blockidx = -1;
    int lineidx = -1;
    std::vector<unsigned int> line_text;
    for (const auto& linedata : linebuf) {
        int k = linedata.prev_j;
        for (const auto& c : linedata.predstr) {
            if (c == 0xFFF9 || c == 0xFFFA || c == 0xFFFB) {
                line_text.push_back(c);
                continue;
            }

            if (feature_idx[k].id < 0 || c == '\n') {
                if (!line_text.empty()) {
                    info.line.emplace_back(blockidx, lineidx, line_text, line_vertical, line_x1, line_y1, line_x2, line_y2);
                    line_x1 = -2000;
                    line_x2 = -2000;
                    line_y1 = -2000;
                    line_y2 = -2000;
                    line_vertical = 0;
                    line_text.clear();
                }

                while (feature_idx[k].id < 0) {
                    k++;
                    if (k >= linedata.cur_j) {
                        goto strloop_end;
                    }
                }
                if (c == '\n') {
                    continue;
                }
            }

            if ((c >= 0x9 && c <= 0xD) || c == 0x20 || c == 0x85 || c == 0xa0 || c == 0x1680 || (c >= 0x2000 && c <= 0x200A) || c == 0x2028 || c == 0x2029 || c == 0x202F || c == 0x205F || c == 0x3000) {
                line_text.push_back(c);
                continue;
            }

            int id = feature_idx[k].id;
            blockidx = feature_idx[k].block;
            lineidx = feature_idx[k].idx;
            int subidx = feature_idx[k].subidx;
            int subtype = feature_idx[k].subtype;
            float cx = locations[id * 9 + 1];
            float cy = locations[id * 9 + 2];
            float w = locations[id * 9 + 3];
            float h = locations[id * 9 + 4];

            int ruby = 0;
            int rubybase = 0;
            if ((subtype & (2 + 4)) == 2 + 4) {
                ruby = 1;
            }
            else if ((subtype & (2 + 4)) == 2) {
                rubybase = 1;
            }

            int emphasis = 0;
            if ((subtype & 16) == 16) {
                emphasis = 1;
            }

            int vertical = 0;
            if ((subtype & 1) == 1) {
                vertical = 1;
            }
            line_vertical = vertical;

            if (ruby == 0) {
                if (line_x1 < -1000) {
                    line_x1 = cx - w / 2;
                }
                else {
                    line_x1 = std::min(line_x1, cx - w / 2);
                }
                if (line_x2 < -1000) {
                    line_x2 = cx + w / 2;
                }
                else {
                    line_x2 = std::max(line_x2, cx + w / 2);
                }
                if (line_y1 < -1000) {
                    line_y1 = cy - h / 2;
                }
                else {
                    line_y1 = std::min(line_y1, cy - h / 2);
                }
                if (line_y2 < -1000) {
                    line_y2 = cy + h / 2;
                }
                else {
                    line_y2 = std::max(line_y2, cy + h / 2);
                }
            }

            line_text.push_back(c);

            info.box.emplace_back(blockidx, lineidx, subidx, std::vector<unsigned int>{ c }, ruby, rubybase, emphasis, vertical, cx, cy, w, h);

            k++;
            if (k >= linedata.cur_j) {
                goto strloop_end;
            }
        }
    strloop_end:
        ;
    }
    if (!line_text.empty()) {
        info.line.emplace_back(blockidx, lineidx, line_text, line_vertical, line_x1, line_y1, line_x2, line_y2);
    }

    float block_x1 = -2000;
    float block_x2 = -2000;
    float block_y1 = -2000;
    float block_y2 = -2000;
    int block_vertical = 0;
    blockidx = -1;
    std::vector<unsigned int> block_text;
    for (const auto linedata : info.line) {
        if (blockidx != linedata.blockidx) {
            if (!block_text.empty()) {
                info.block.emplace_back(blockidx, block_text, block_vertical, block_x1, block_y1, block_x2, block_y2);
                block_x1 = -2000;
                block_x2 = -2000;
                block_y1 = -2000;
                block_y2 = -2000;
                block_vertical = 0;
                block_text.clear();
            }
            blockidx = linedata.blockidx;
        }

        if (block_x1 < -1000) {
            block_x1 = linedata.x1;
        }
        else {
            block_x1 = std::min(block_x1, linedata.x1);
        }
        if (block_x2 < -1000) {
            block_x2 = linedata.x2;
        }
        else {
            block_x2 = std::max(block_x2, linedata.x2);
        }
        if (block_y1 < -1000) {
            block_y1 = linedata.y1;
        }
        else {
            block_y1 = std::min(block_y1, linedata.y1);
        }
        if (block_y2 < -1000) {
            block_y2 = linedata.y2;
        }
        else {
            block_y2 = std::max(block_y2, linedata.y2);
        }

        std::copy(linedata.text.begin(), linedata.text.end(), std::back_inserter(block_text));
        block_text.push_back('\n');
        block_vertical = linedata.vertical;
    }
    if (!block_text.empty()) {
        info.block.emplace_back(blockidx, block_text, block_vertical, block_x1, block_y1, block_x2, block_y2);
    }

    info.text = result_txt;

    return 0;
}
