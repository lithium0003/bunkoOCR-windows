#include "TextDetector.h"
#include <onnxruntime/core/providers/dml/dml_provider_factory.h>

double blank_cutoff = 15;
float detect_cut_off = 0.35;

extern bool useCUDA;
extern bool useTensorRT;
extern bool useDirectML;
extern int useDirectML_idx;
extern bool useOpenVINO;

TextDetector::TextDetector() :
    env(ORT_LOGGING_LEVEL_FATAL),
    session(env, L"TextDetector.quant.onnx", sessionOptions),
    inputTensor(Ort::Value::CreateTensor<float>(memory_info, input.data(), input.size(), inputShape.data(), inputShape.size()))
{   
    bool success = false;
    if (!success && useTensorRT) {
        success = true;
        try {
            auto api = Ort::GetApi();
            OrtTensorRTProviderOptionsV2* tensorrt_options = nullptr;
            Ort::ThrowOnError(api.CreateTensorRTProviderOptions(&tensorrt_options));
            std::vector<const char*> keys{ "trt_max_workspace_size", "trt_fp16_enable", "trt_engine_cache_enable", "trt_engine_cache_path" };
            std::vector<const char*> values{ "4294967296", "1", "1", "cache" };
            Ort::ThrowOnError(api.UpdateTensorRTProviderOptions(tensorrt_options, keys.data(), values.data(), 4));
            sessionOptions = Ort::SessionOptions();
            sessionOptions.AppendExecutionProvider_TensorRT_V2(*tensorrt_options);
            session = Ort::Session(env, L"TextDetector.onnx", sessionOptions);
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
            session = Ort::Session(env, L"TextDetector.onnx", sessionOptions);
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
            session = Ort::Session(env, L"TextDetector.onnx", sessionOptions);
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
            session = Ort::Session(env, L"TextDetector.quant.onnx", sessionOptions);
            std::cout << "OpenVINO" << std::endl;
        }
        catch (...) {
            success = false;
        }
    }

    outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, maps.data(), maps.size(), mapsOutputShape.data(), mapsOutputShape.size()));
    outputTensors.push_back(Ort::Value::CreateTensor<float>(memory_info, feature.data(), feature.size(), featureOutputShape.data(), featureOutputShape.size()));
}

int TextDetector::run() {
    Ort::RunOptions runOptions;
    try {
        session.Run(runOptions, inputNames.data(), &inputTensor, 1, outputNames.data(), outputTensors.data(), 2);
    }
    catch (Ort::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void TextDetector::load_im(int xi, int yi, int width, int height, const Gdiplus::BitmapData& data)
{
    int s = 768 * 768;
    int stride = data.Stride;
    auto p = (unsigned char*)data.Scan0;
    for (int y = 0; y < 768; y++) {
        for (int x = 0; x < 768; x++) {
            float* im = &input[y * 768 + x];
            if (yi + y >= height || xi + x >= width) {
                *im = 1.0f;
                im += s;
                *im = 1.0f;
                im += s;
                *im = 1.0f;
                continue;
            }
            unsigned char* input_im = &p[(yi + y) * stride + (xi + x) * 3];
            *im = (float)*input_im++ / 255.0f;
            im += s;
            *im = (float)*input_im++ / 255.0f;
            im += s;
            *im = (float)*input_im++ / 255.0f;
        }
    }
}

void TextDetector::find_peaks(std::vector<int>& peaks, std::vector<float>& keymap_p, int xi, int yi, int width, int height)
{
    int s = 768 / 4;
    for (int y = 0; y < s; y++) {
        if (yi + y * 4 >= height) continue;

        // skip edge if block is not edge block
        if (yi > 0 && y < s * (1 - step_ratio) / 2) continue;
        if (height > 768 && yi <= height - 768 && y > s * (1- (1 - step_ratio) / 2)) continue;

        for (int x = 0; x < s; x++) {
            if (xi + x * 4 >= width) continue;

            // skip edge if block is not edge block
            if (xi > 0 && x < s * (1 - step_ratio) / 2) continue;
            if (width > 768 && xi <= width - 768 && x > s * (1 - (1 - step_ratio) / 2)) continue;

            float line_p = float(maps[4 * s * s + y * s + x]);
            float seps_p = float(maps[5 * s * s + y * s + x]);
            int x_is = xi / 4 + x;
            int y_is = yi / 4 + y;
            lineimage[y_is * im_width + x_is] = std::max(lineimage[y_is * im_width + x_is], 1 / (1 + exp(-line_p)));
            sepimage[y_is * im_width + x_is] = std::max(sepimage[y_is * im_width + x_is], 1 / (1 + exp(-seps_p)));

            float peak_p = float(maps[1 * s * s + y * s + x]);
            peak_p = 1 / (1 + exp(-peak_p));
            if (peak_p > detect_cut_off) {
                peaks.push_back(y * s + x);
                keymap_p.push_back(peak_p);
            }
        }
    }
}

void TextDetector::process_peaks(const std::vector<int>& peaks, const std::vector<float>& keymap_p, int xi, int yi, int width, int height)
{
    std::vector<int> idx(peaks.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&keymap_p](int a, int b) { return keymap_p[a] > keymap_p[b]; });
    int s = 768 / 4;

    for (const auto i : idx) {
        int j = peaks[i];
        if (keymap_p[i] < detect_cut_off) break;

        float w = exp((float)maps[2 * s * s + j] - 3) * 1024;
        float h = exp((float)maps[3 * s * s + j] - 3) * 1024;
        if (w * h <= 0) continue;

        int x = j % s;
        int y = j / s;

        float cx = x * 4 + xi;
        float cy = y * 4 + yi;

        float code1_p = float(maps[6 * s * s + j]);
        float code2_p = float(maps[7 * s * s + j]);
        float code4_p = float(maps[8 * s * s + j]);
        float code8_p = float(maps[9 * s * s + j]);
        code1_p = 1 / (1 + exp(-code1_p));
        code2_p = 1 / (1 + exp(-code2_p));
        code4_p = 1 / (1 + exp(-code4_p));
        code8_p = 1 / (1 + exp(-code8_p));


        locations.push_back(keymap_p[i]);
        locations.push_back(cx);
        locations.push_back(cy);
        locations.push_back(w);
        locations.push_back(h);
        locations.push_back(code1_p);
        locations.push_back(code2_p);
        locations.push_back(code4_p);
        locations.push_back(code8_p);

        for (int k = 0; k < 100; k++) {
            glyphfeatures.push_back(feature[k * s * s + j]);
        }
    }
}

void TextDetector::filter_boxes(std::vector<int>& selected_idx)
{
    std::vector<int> idx = selected_idx;
    selected_idx.clear();
    std::sort(idx.begin(), idx.end(), [this](int a, int b) { return locations[a * 9] > locations[b * 9]; });

    for (const auto i : idx) {
        if (locations[i * 9] < detect_cut_off) break;

        float cx = locations[i * 9 + 1];
        float cy = locations[i * 9 + 2];
        float w = locations[i * 9 + 3];
        float h = locations[i * 9 + 4];
        float area0_vol = w * h;

        bool pass = true;
        std::vector<bool> fill_map(int(w + 1) * int(h + 1));
        for (const auto j : selected_idx) {
            float area1_vol = locations[j * 9 + 3] * locations[j * 9 + 4];
            float cx1 = locations[j * 9 + 1];
            float cy1 = locations[j * 9 + 2];
            float w1 = locations[j * 9 + 3];
            float h1 = locations[j * 9 + 4];

            float inter_xmin = std::max(cx - w / 2, cx1 - w1 / 2);
            float inter_ymin = std::max(cy - h / 2, cy1 - h1 / 2);
            float inter_xmax = std::min(cx + w / 2, cx1 + w1 / 2);
            float inter_ymax = std::min(cy + h / 2, cy1 + h1 / 2);
            float inter_w = std::max(inter_xmax - inter_xmin, 0.0f);
            float inter_h = std::max(inter_ymax - inter_ymin, 0.0f);

            float inter_vol = inter_w * inter_h;
            float union_vol = area0_vol + area1_vol - inter_vol;
            float iou = 0;
            if (union_vol > 0) iou = inter_vol / union_vol;

            if (iou > 0.5) {
                pass = false;
                break;
            }
            if (inter_vol > area0_vol * 0.75) {
                pass = false;
                break;
            }
            if (iou > 0) {
                int p1x = std::max(cx1 - w1 / 2, cx - w / 2) - (cx - w / 2);
                int p2x = std::min(cx1 + w1 / 2, cx + w / 2) - (cx - w / 2) + 1;
                int p1y = std::max(cy1 - h1 / 2, cy - h / 2) - (cy - h / 2);
                int p2y = std::min(cy1 + h1 / 2, cy + h / 2) - (cy - h / 2) + 1;
                for (int yi = std::max(0, p1y); yi < std::min(int(h+1), p2y); yi++){
                    for (int xi = std::max(0, p1x); xi < std::min(int(w+1), p2x); xi++) {
                        fill_map[yi * int(w + 1) + xi] = true;
                    }
                }
                if ((double)std::count(fill_map.begin(), fill_map.end(), true) / (int(w + 1) * int(h + 1)) > 0.5) {
                    pass = false;
                    break;
                }
            }
        }

        if (pass) {
            int x = cx / 4;
            int y = cy / 4;
            if (x >= 0 && x < im_width && y >= 0 && y < im_height) {
                if (sepimage[y * im_width + x] > 0.5) {
                    pass = false;
                }
            }
        }

        if (pass) {
            selected_idx.push_back(i);
        }
    }

    std::cerr << "result: " << selected_idx.size() << std::endl;
}

std::vector<std::vector<int>> calc_hist(int width, int height, const Gdiplus::BitmapData& data, int minx, int maxx, int miny, int maxy)
{
    std::vector<int> rHist(256);
    std::vector<int> gHist(256);
    std::vector<int> bHist(256);

    int stride = data.Stride;
    auto p = (unsigned char*)data.Scan0;

    for (int y = miny; y <= maxy; y++) {
        if (y < 0 || y >= height) continue;
        for (int x = minx; x < maxx; x++) {
            if (x < 0 || x >= width) continue;

            unsigned char* input_im = &p[y * stride + x * 3];
            int r = input_im[0];
            int g = input_im[1];
            int b = input_im[2];
            rHist[r]++;
            gHist[g]++;
            bHist[b]++;
        }
    }

    return { rHist, gHist, bHist };
}

double dist_hist(const std::vector<int>& hist)
{
    int sum = std::accumulate(hist.begin(), hist.end(), 0);
    if (sum == 0) return 0;

    double mean = 0;
    for (int i = 0; i < hist.size(); i++) {
        mean += i * hist[i];
    }
    mean /= sum;

    int k1_sum = 0;
    int k2_sum = 0;
    double k1_mean = 0;
    double k2_mean = 0;
    for (int i = 0; i < hist.size(); i++) {
        if (i < mean) {
            k1_sum += hist[i];
            k1_mean += i * hist[i];
        }
        else {
            k2_sum += hist[i];
            k2_mean += i * hist[i];
        }
    }
    if (k1_sum == 0) return 0;
    if (k2_sum == 0) return 0;

    k1_mean /= k1_sum;
    k2_mean /= k2_sum;

    double k1 = k1_mean;
    double k2 = k2_mean;
    double dist1 = 256;
    double dist2 = fabs(k1 - k2);

    while (dist1 != dist2) {
        dist1 = dist2;

        int k1_sum = 0;
        int k2_sum = 0;
        double k1_mean = 0;
        double k2_mean = 0;

        for (int i = 0; i < hist.size(); i++) {
            if (fabs(i - k1) < fabs(i - k2)) {
                k1_sum += hist[i];
                k1_mean += i * hist[i];
            }
            else {
                k2_sum += hist[i];
                k2_mean += i * hist[i];
            }
        }
        if (k1_sum == 0) return 0;
        if (k2_sum == 0) return 0;

        k1_mean /= k1_sum;
        k2_mean /= k2_sum;
        k1 = k1_mean;
        k2 = k2_mean;
        dist2 = fabs(k1 - k2);
    }

    return dist1;
}

std::vector<int> remove_background(int width, int height, const Gdiplus::BitmapData& data, const std::vector<float>& locations)
{
    int count = locations.size() / 9;
    std::vector<int> valid_idx;

    for (int i = 0; i < count; i++) {
        float cx = locations[i * 9 + 1];
        float cy = locations[i * 9 + 2];
        float w = locations[i * 9 + 3];
        float h = locations[i * 9 + 4];
        
        auto hists = calc_hist(width, height, data, cx - w / 2, cx + w / 2, cy - h / w, cy + h / 2);
        double max_dist = 0;
        for (const auto& hist : hists) {
            max_dist = std::max(max_dist, dist_hist(hist));
        }

        if (max_dist > blank_cutoff) {
            valid_idx.push_back(i);
        }
    }

    return valid_idx;
}


int TextDetector::run_model(std::vector<float>& loc, std::vector<float>& features, int width, int height, const Gdiplus::BitmapData& data)
{
    locations.clear();
    glyphfeatures.clear();

    step = 768 * step_ratio;
    if (width < 768) {
        im_width = 768 / 4;
    }
    else {
        im_width = (((width + 768) / step + 1) * step) / 4;
    }
    if (height < 768) {
        im_height = 768 / 4;
    }
    else {
        im_height = (((height + 768) / step + 1) * step) / 4;
    }
    lineimage.resize(im_width * im_height);
    sepimage.resize(im_width * im_height);
    std::fill(lineimage.begin(), lineimage.end(), 0);
    std::fill(sepimage.begin(), sepimage.end(), 0);

    int total_count = ((std::max(step, height) + step - 1) / step) * ((std::max(step, width) + step - 1) / step);
    int loop_count = 0;
    for (int yi = 0; yi < std::max(step, height); yi += step) {
        for (int xi = 0; xi < std::max(step, width); xi += step) {
            loop_count++;
            std::cout << "detector " << (double)loop_count / total_count * 100 << "% " << loop_count << "/" << total_count << std::endl;

            load_im(xi, yi, width, height, data);

            if (run() != 0) {
                return 1;
            }

            std::vector<int> peaks;
            std::vector<float> keymap_p;
            find_peaks(peaks, keymap_p, xi, yi, width, height);

            if (peaks.empty()) continue;

            process_peaks(peaks, keymap_p, xi, yi, width, height);
        }
    }

    std::vector<int> selected_idx = remove_background(width, height, data, locations);
    filter_boxes(selected_idx);

    for (auto i : selected_idx) {
        std::copy(locations.begin() + (i * 9), locations.begin() + ((i+1) * 9), std::back_inserter(loc));
        std::copy(glyphfeatures.begin() + (i * 100), glyphfeatures.begin() + ((i + 1) * 100), std::back_inserter(features));
    }

    return 0;
}
