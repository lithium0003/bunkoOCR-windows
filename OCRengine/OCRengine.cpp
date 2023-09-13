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

#pragma	comment(lib,"onnxruntime.lib")
#pragma	comment(lib,"onnxruntime_providers_dml.lib")

extern double blank_cutoff;
extern double ruby_cutoff;
extern double rubybase_cutoff;
extern double space_cutoff;
extern float line_valueth;
extern float detect_cut_off;
double resize = 1.0;
int sleep_wait = 0;

std::vector<float> make_feature_input(const std::vector<int>& boxes, std::vector<float>& glyphfeatures)
{
    int count = glyphfeatures.size() / 64;
    int box_count = boxes.size() / 5;

    std::cerr << box_count << " boxes" << std::endl;

    std::vector<float> features;
    int prev_block = 0;
    int prev_idx = 0;
    std::vector<float> cr(64 + 4);
    cr[64 + 3] = 1;
    for (int i = 0; i < box_count; i++) {
        int id = boxes[i * 5 + 0];
        int block = boxes[i * 5 + 1];
        int idx = boxes[i * 5 + 2];
        int subidx = boxes[i * 5 + 3];
        int subtype = boxes[i * 5 + 4];

        if (id < 0 || id >= count) continue;

        float ruby = 0;
        float rubybase = 0;
        float space = 0;

        if (prev_block != block) {
            prev_block = block;
            std::copy(cr.begin(), cr.end(), std::back_inserter(features));
            std::copy(cr.begin(), cr.end(), std::back_inserter(features));
        }
        if (prev_idx != idx) {
            prev_idx = idx;
            std::copy(cr.begin(), cr.end(), std::back_inserter(features));
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

        std::copy(glyphfeatures.begin() + id * 64, glyphfeatures.begin() + (id + 1) * 64, std::back_inserter(features));
        features.push_back(space);
        features.push_back(ruby);
        features.push_back(rubybase);
        features.push_back(0);
    }
    std::copy(cr.begin(), cr.end(), std::back_inserter(features));

    return features;
}

std::string UTF32toUTF8(unsigned int code)
{
    if (code < 0) {
        return "";
    }
    std::stringstream ss;
    if (code <= 0x7F) {
        ss << unsigned char(code);
        return ss.str();
    }
    if (code <= 0x07FF) {
        //                    110x xxxx
        unsigned char byte1 = 0xC0 | ((code & 0x07C0) >> 6);
        //                    10xx xxxx
        unsigned char byte2 = 0x80 | (code & 0x003F);
        ss << byte1 << byte2;
        return ss.str();
    }
    if (code <= 0xFFFF) {
        //                    1110 xxxx
        unsigned char byte1 = 0xE0 | ((code & 0xf000) >> 12);
        //                    10xx xxxx
        unsigned char byte2 = 0x80 | ((code & 0x0FC0) >> 6);
        //                    10xx xxxx
        unsigned char byte3 = 0x80 | (code & 0x003F);
        ss << byte1 << byte2 << byte3;
        return ss.str();
    }
    if (code <= 0x10FFFF) {
        //                    1111 0xxx
        unsigned char byte1 = 0xF0 | ((code & 0x1C0000) >> 18);
        //                    10xx xxxx
        unsigned char byte2 = 0x80 | ((code & 0x3F000) >> 12);
        //                    10xx xxxx
        unsigned char byte3 = 0x80 | ((code & 0x0FC0) >> 6);
        //                    10xx xxxx
        unsigned char byte4 = 0x80 | (code & 0x003F);
        ss << byte1 << byte2 << byte3 << byte4;
        return ss.str();
    }
    return "";
}

std::string escape_char(unsigned int c)
{
    if (c >= 0x20 && c <= 0x21) {
        return UTF32toUTF8(c);
    }
    if (c >= 0x23 && c <= 0x5B) {
        return UTF32toUTF8(c);
    }
    if (c >= 0x5D && c <= 0x10FFFF) {
        return UTF32toUTF8(c);
    }
    switch (c) {
    case 0x22:
        return "\\\"";
    case 0x5C:
        return "\\\\";
    case 0x2F:
        return "\\/";
    case 0x08:
        return "\\b";
    case 0x0C:
        return "\\f";
    case 0x0A:
        return "\\n";
    case 0x0D:
        return "\\r";
    case 0x09:
        return "\\t";
    }
    const char* table = "0123456789ABCDEF";
    std::string s = "\\u";
    s += table[(c & 0xF000) >> 12];
    s += table[(c & 0x0F00) >> 8];
    s += table[(c & 0x00F0) >> 4];
    s += table[c & 0x000F];
    return s;
}

std::string escape_char(std::string str)
{
    std::string result = "";
    for (unsigned char *p = (unsigned char*)str.data(); *p != 0; p++) {
        unsigned int c = *p;
        if (c >= 0x20 && c <= 0x21) {
            result += *p;
            continue;
        }
        else if (c >= 0x23 && c <= 0x5B) {
            result += *p;
            continue;
        }
        else if (c >= 0x5D && c <= 0x10FFFF) {
            result += *p;
            continue;
        }
        switch (c) {
        case 0x22:
            result += "\\\"";
            continue;
        case 0x5C:
            result += "\\\\";
            continue;
        case 0x2F:
            result += "\\/";
            continue;
        case 0x08:
            result += "\\b";
            continue;
        case 0x0C:
            result += "\\f";
            continue;
        case 0x0A:
            result += "\\n";
            continue;
        case 0x0D:
            result += "\\r";
            continue;
        case 0x09:
            result += "\\t";
            continue;
        }
        const char* table = "0123456789ABCDEF";
        std::string s = "\\u";
        s += table[(c & 0xF000) >> 12];
        s += table[(c & 0x0F00) >> 8];
        s += table[(c & 0x00F0) >> 4];
        s += table[c & 0x000F];
        result += s;
    }
    return result;
}

void WriteJson(std::ostream& stream, const std::vector<int>& boxes, const std::vector<float>& locations, const std::vector<unsigned int>& codes, const std::string& txt)
{
    int box_count = boxes.size() / 5;
    int count = std::min(codes.size(), locations.size() / 9);

    stream << "{" << std::endl;

    stream << " \"boxes\": [" << std::endl;
    for (int i = 0; i < box_count; i++) {
        int id = boxes[i * 5 + 0];
        int block = boxes[i * 5 + 1];
        int idx = boxes[i * 5 + 2];
        int subidx = boxes[i * 5 + 3];
        int subtype = boxes[i * 5 + 4];

        if (id < 0 || id >= count) continue;

        auto c = codes[id];
        if (c > 0x10FFFF) c = 0xFFFD;
        auto s = escape_char(c);

        int direction = 0;
        int ruby = 0;
        int rubybase = 0;
        int space = 0;

        if ((subtype & (1)) == 1) {
            direction = 1;
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

        float cx = locations[id * 9 + 1] / resize;
        float cy = locations[id * 9 + 2] / resize;
        float w = locations[id * 9 + 3] / resize;
        float h = locations[id * 9 + 4] / resize;

        stream << "  {" << std::endl;

        stream << "   \"block\": " << block << "," << std::endl;
        stream << "   \"line\": " << idx << "," << std::endl;
        stream << "   \"index\": " << subidx << "," << std::endl;
        stream << "   \"direction\": " << direction << "," << std::endl;
        stream << "   \"ruby\": " << ruby << "," << std::endl;
        stream << "   \"rubybase\": " << rubybase << "," << std::endl;
        stream << "   \"space\": " << space << "," << std::endl;
        stream << "   \"cx\": " << cx << "," << std::endl;
        stream << "   \"cy\": " << cy << "," << std::endl;
        stream << "   \"w\": " << w << "," << std::endl;
        stream << "   \"h\": " << h << "," << std::endl;
        stream << "   \"character\": \"" << s << "\"" << std::endl;

        if (i < box_count - 1) {
            stream << "  }," << std::endl;
        }
        else {
            stream << "  }" << std::endl;
        }
    }
    stream << " ]," << std::endl;

    stream << " \"text\": \"" << escape_char(txt) << "\"" << std::endl;

    stream << "}" << std::endl;
}

std::wstring ToUnicodeStr(const std::string &utf8str)
{
    int needLength = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
    std::wstring result;
    result.resize(needLength - 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, &result[0], needLength);
    return result;
}

int process(std::string input_filename, TextDetector& model1, CodeDecoder& model2, Transformer& model3)
{
    int width = 0;
    int height = 0;
    std::wstring wfilename = ToUnicodeStr(input_filename);
    auto bitmap = Gdiplus::Bitmap::FromFile(wfilename.c_str(), true);
    if (bitmap == nullptr) {
        std::cerr << "faild to load image" << std::endl;
        return 1;
    }
    if (bitmap->GetLastStatus() != Gdiplus::Ok) {
        delete bitmap;
        std::cerr << "faild to load image" << std::endl;
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
    auto boxes = line_detect(model1.im_width, model1.im_heiht, locations, model1.lineimage, model1.sepimage);

    auto feature = make_feature_input(boxes, glyphfeatures);

    std::vector<unsigned int> code;
    ret = model3.run_model(code, feature);
    if (ret != 0) return ret;

    std::string txt;
    for (auto i : code) {
        if (i > 0x10FFFF) i = 0xFFFD;
        txt += UTF32toUTF8(i);
    }

    std::ofstream ofs(input_filename + ".json");
    WriteJson(ofs, boxes, locations, codes, txt);

    std::cout << "done: " << input_filename << std::endl;

    return 0;
}

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_4.h>
#pragma comment(lib, "dxgi.lib")

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

Gdiplus::GdiplusStartupInput gdiSI;
ULONG_PTR gdiToken;

bool useDirectML = false;

int main(int argc, char **argv)
{
    if (Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, NULL) != Gdiplus::Ok) {
        std::cerr << "GdiplusStartup() faild" << std::endl;
        return 1;
    }

    auto providers = Ort::GetAvailableProviders();
    for (auto s : providers) {
        std::cout << s << std::endl;
    }

    ComPtr<IDXGIFactory4> pFactory;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&pFactory)))) {
        IDXGIAdapter3* pAdapter;
        if (pFactory->EnumAdapters1(0, reinterpret_cast<IDXGIAdapter1**>(&pAdapter)) == S_OK) {
            DXGI_QUERY_VIDEO_MEMORY_INFO info;
            pAdapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info);

            std::cout << info.AvailableForReservation / 1024.0 / 1024 << "MiB meomry" << std::endl;

            if (info.AvailableForReservation > (UINT64)2 * 1024 * 1024 * 1024) {
                useDirectML = true;
            }
        }
    }

    bool stdin_filename = false;
    if (argc < 2) {
        stdin_filename = true;
    }
    std::cout << "process start" << std::endl;

    static TextDetector model1;
    static CodeDecoder model2;
    static Transformer model3;

    std::cout << "ready" << std::endl;

    std::string input_filename;
    if (!stdin_filename) {
        input_filename = argv[1];
        int ret = process(input_filename, model1, model2, model3);
        Gdiplus::GdiplusShutdown(gdiToken);
        return ret;
    }

    std::vector<std::string> params = {
        "blank_cutoff:",
        "ruby_cutoff:",
        "rubybase_cutoff:",
        "space_cutoff:",
        "line_valueth:",
        "detect_cut_off:",
        "resize:",
        "sleep_wait:",
    };

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