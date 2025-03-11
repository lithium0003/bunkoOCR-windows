// OCRengine.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <sstream>
#include <fstream>

#include <windows.h>
#include <gdiplus.h>
#pragma	comment(lib,"gdiplus.lib")
#undef min
#undef max

#include "TextDetector.h"
#include "CodeDecoder.h"
#include "line_detect.h"
#include "Transformer.h"
#include "util_func.h"

#pragma	comment(lib,"onnxruntime.lib")
#pragma	comment(lib,"onnxruntime_providers_dml.lib")

extern double blank_cutoff;
extern float detect_cut_off;

extern double ruby_cutoff;
extern double rubybase_cutoff;
extern double space_cutoff;
extern double emphasis_cutoff;
extern float line_valueth;
extern float sep_valueth;
extern float sep_valueth2;
extern double allowwidth_next;
double resize = 1.0;
int sleep_wait = 0;

void make_feature_input(const std::vector<int32_t>& boxes, const std::vector<float>& glyphfeatures, std::vector<float>& features, std::vector<feature_idxinfo>& feature_idx)
{
    int count = glyphfeatures.size() / 100;
    int box_count = boxes.size() / 5;

    std::cerr << box_count << " boxes" << std::endl;

    features.clear();
    feature_idx.clear();

    int prev_block = 0;
    int prev_idx = 0;
    int vertical = 0;
    for (int i = 0; i < box_count; i++) {
        int id = boxes[i * 5 + 0];
        int block = boxes[i * 5 + 1];
        int idx = boxes[i * 5 + 2];
        int subidx = boxes[i * 5 + 3];
        int subtype = boxes[i * 5 + 4];

        if (id < 0 || id >= count) continue;

        int ruby = 0;
        int rubybase = 0;
        int space = 0;
        int emphasis = 0;


        if (prev_block != block) {
            prev_block = block;
            std::vector<float> g(100 + 6);
            g[100 + 0] = 5 * vertical;
            g[100 + 5] = 5;
            std::copy(g.begin(), g.end(), std::back_inserter(features));
            feature_idx.emplace_back(-1, -1, -1, -1, -1);
            prev_idx = -1;
        }
        if (prev_idx != idx) {
            prev_idx = idx;
            std::vector<float> g(100 + 6);
            g[100 + 0] = 5 * vertical;
            g[100 + 5] = 5;
            std::copy(g.begin(), g.end(), std::back_inserter(features));
            feature_idx.emplace_back(-1, -1, -1, -1, -1);
        }

        if ((subtype & (2 + 4)) == 2 + 4) {
            ruby = 1;
        }
        else if ((subtype & (2 + 4)) == 2) {
            rubybase = 1;
        }

        if ((subtype & 8) == 8) {
            space = 1;
        }

        if ((subtype & 16) == 16) {
            emphasis = 1;
        }

        if ((subtype & 1) == 0) {
            vertical = 0;
        }
        else {
            vertical = 1;
        }

        for (int j = 0; j < 100; j++) {
            features.push_back(float(glyphfeatures[id * 100 + j]));
        }
        features.push_back(vertical * 5);
        features.push_back(rubybase * 5);
        features.push_back(ruby * 5);
        features.push_back(space * 5);
        features.push_back(emphasis * 5);
        features.push_back(0);
        feature_idx.emplace_back(id, block, idx, subidx, subtype);
    }
}


int process(std::string input_filename, TextDetector& model1, CodeDecoder& model2, Transformer& model3)
{
    int width = 0;
    int height = 0;
    std::wstring wfilename = ToUnicodeStr(input_filename);
    auto bitmap = Gdiplus::Bitmap::FromFile(wfilename.c_str(), true);
    if (bitmap == nullptr) {
        std::cerr << "faild to load image " << input_filename << std::endl;
        return 1;
    }
    if (bitmap->GetLastStatus() != Gdiplus::Ok) {
        delete bitmap;
        std::cerr << "faild to load image status " << input_filename << std::endl;
        return 1;
    }
    auto rotation = Gdiplus::RotateFlipType::RotateNoneFlipNone;
    int prop_count = bitmap->GetPropertyCount();
    if (prop_count > 0) {
        PROPID* propIDs = new PROPID[prop_count];
        bitmap->GetPropertyIdList(prop_count, propIDs);

        for (int j = 0; j < prop_count; ++j) {
            if (propIDs[j] != PropertyTagOrientation) {
                continue;
            }

            auto size = bitmap->GetPropertyItemSize(PropertyTagOrientation);
            auto propertyItem = (Gdiplus::PropertyItem*)malloc(size);
            if (propertyItem) {
                bitmap->GetPropertyItem(PropertyTagOrientation, size, propertyItem);

                if (propertyItem->type == PropertyTagTypeShort) {
                    auto value = *(unsigned short*)propertyItem->value;
                    switch (value) {
                    case 2:
                        rotation = Gdiplus::RotateFlipType::RotateNoneFlipX;
                        break;
                    case 3:
                        rotation = Gdiplus::RotateFlipType::RotateNoneFlipXY;
                        break;
                    case 4:
                        rotation = Gdiplus::RotateFlipType::RotateNoneFlipY;
                        break;
                    case 5:
                        rotation = Gdiplus::RotateFlipType::Rotate90FlipX;
                        break;
                    case 6:
                        rotation = Gdiplus::RotateFlipType::Rotate90FlipNone;
                        break;
                    case 7:
                        rotation = Gdiplus::RotateFlipType::Rotate270FlipX;
                        break;
                    case 8:
                        rotation = Gdiplus::RotateFlipType::Rotate270FlipNone;
                        break;
                    }
                }

                free(propertyItem);
            }
            break;
        }
        delete[] propIDs;
    }
    bitmap->RotateFlip(rotation);

    int newWidth = resize * bitmap->GetWidth();
    int newHeight = resize * bitmap->GetHeight();
    if (newWidth != bitmap->GetWidth() || newHeight != bitmap->GetHeight()) {
        auto img = new Gdiplus::Bitmap(newWidth, newHeight, PixelFormat24bppRGB);
        img->SetResolution(bitmap->GetHorizontalResolution(), bitmap->GetVerticalResolution());
        Gdiplus::Graphics g(img);
        g.ScaleTransform(resize, resize);
        g.DrawImage(bitmap, 0, 0);

        delete bitmap;
        bitmap = img;
    }

    width = bitmap->GetWidth();
    height = bitmap->GetHeight();
    std::cout << width << " x " << height <<  std::endl;

    Gdiplus::Rect srcRect(0, 0, width, height);
    Gdiplus::BitmapData data;
    bitmap->LockBits(&srcRect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, &data);

    std::vector<float> locations;
    std::vector<float> glyphfeatures;

    int ret = model1.run_model(locations, glyphfeatures, width, height, data);
    bitmap->UnlockBits(&data);
    delete bitmap;
    if (ret != 0) return ret;

    std::cout << "decode" << std::endl;
    std::vector<unsigned int> codes;
    ret = model2.run_model(codes, glyphfeatures);
    if (ret != 0) return ret;

    std::cout << "linedetect" << std::endl;
    auto boxes = line_detect(model1.im_width, model1.im_height, locations, model1.lineimage, model1.sepimage);

    std::vector<float> features;
    std::vector<feature_idxinfo> feature_idx;
    make_feature_input(boxes, glyphfeatures, features, feature_idx);
    
    OCR_info info;
    ret = model3.run_model(info, features, locations, feature_idx);
    if (ret != 0) return ret;

    for (auto& block : info.block) {
        block.x1 /= resize;
        block.x2 /= resize;
        block.y1 /= resize;
        block.y2 /= resize;
    }
    for (auto& line : info.line) {
        line.x1 /= resize;
        line.x2 /= resize;
        line.y1 /= resize;
        line.y2 /= resize;
    }
    for (auto& box : info.box) {
        box.cx /= resize;
        box.cy /= resize;
        box.w /= resize;
        box.h /= resize;
    }

    std::ofstream ofs(wfilename + TEXT(".json"));
    WriteJson(ofs, info);

    std::cout << "done: " << input_filename << std::endl;

    return 0;
}

Gdiplus::GdiplusStartupInput gdiSI;
ULONG_PTR gdiToken;

bool useCUDA = false;
bool useTensorRT = false;
bool useDirectML = false;
int useDirectML_idx = 0;
bool useOpenVINO = false;

int main(int argc, char **argv)
{
    if (Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, NULL) != Gdiplus::Ok) {
        std::cerr << "GdiplusStartup() faild" << std::endl;
        return -1;
    }

    auto providers = Ort::GetAvailableProviders();
    for (auto s : providers) {
        std::cout << s << std::endl;
    }

    if (argc > 1) {
        useCUDA = false;
        useTensorRT = false;

        for (int i = 1; i < argc; i++) {
            std::string argstr(argv[i]);

            if (argstr == "CUDA") {
                useCUDA = true;
            }
            else if (argstr == "TensorRT") {
                useTensorRT = true;
            }
            else if (argstr == "OpenVINO") {
                useOpenVINO = true;
            }
            else {
                std::stringstream(argv[1]) >> useDirectML_idx;
                if (useDirectML_idx < 0 || useDirectML_idx == 255) {
                    useDirectML = false;
                }
                else {
                    useDirectML = true;
                }
            }
        }
    }
    std::cout << "process start" << std::endl;

    static TextDetector model1;
    static CodeDecoder model2;
    static Transformer model3;

    std::cout << "ready" << std::endl;

    std::vector<std::string> params = {
        "blank_cutoff:",
        "ruby_cutoff:",
        "rubybase_cutoff:",
        "space_cutoff:",
        "emphasis_cutoff:",
        "line_valueth:",
        "detect_cut_off:",
        "resize:",
        "sleep_wait:",
        "sep_valueth:",
        "sep_valueth2:",
        "allowwidth_next:",
    };

    std::string input_filename;
    while (std::getline(std::cin, input_filename)) {
        int ret;
        for (auto s : params) {
            if (input_filename.size() >= s.size() && std::equal(std::begin(s), std::end(s), std::begin(input_filename))) {
                double v;
                std::stringstream(input_filename.substr(s.size())) >> v;
                if (s == "blank_cutoff:") {
                    blank_cutoff = v;
                }
                else if (s == "ruby_cutoff:") {
                    ruby_cutoff = v;
                }
                else if (s == "rubybase_cutoff:") {
                    rubybase_cutoff = v;
                }
                else if (s == "space_cutoff:") {
                    space_cutoff = v;
                }
                else if (s == "emphasis_cutoff:") {
                    emphasis_cutoff = v;
                }
                else if (s == "line_valueth:") {
                    line_valueth = v;
                }
                else if (s == "detect_cut_off:") {
                    detect_cut_off = v;
                }
                else if (s == "resize:") {
                    resize = v;
                }
                else if (s == "sleep_wait:") {
                    sleep_wait = v;
                }
                else if (s == "sep_valueth:") {
                    sep_valueth = v;
                }
                else if (s == "sep_valueth2:") {
                    sep_valueth2 = v;
                }
                else if (s == "allowwidth_next:") {
                    allowwidth_next = v;
                }
                goto nextloop;
            }
        }
        
        ret = process(input_filename, model1, model2, model3);
        if (ret != 0) {
            std::cout << "error: " << input_filename << std::endl;
        }
        if (sleep_wait > 0) {
            Sleep(sleep_wait * 1000);
        }
    nextloop:
        ;
    }

    Gdiplus::GdiplusShutdown(gdiToken);
    return 0;
}