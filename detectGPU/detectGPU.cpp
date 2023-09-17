// detectGPU.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>

#include <windows.h>
#include <gdiplus.h>
#pragma	comment(lib,"gdiplus.lib")

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

Gdiplus::GdiplusStartupInput gdiSI;
ULONG_PTR gdiToken;

int useDirectML_idx = -1;

int main()
{
    if (Gdiplus::GdiplusStartup(&gdiToken, &gdiSI, NULL) != Gdiplus::Ok) {
        std::cerr << "GdiplusStartup() faild" << std::endl;
        return -1;
    }

    ComPtr<IDXGIFactory6> pFactory;
    LUID device;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&pFactory)))) {
        IDXGIAdapter1* pAdapter = nullptr;
        if (SUCCEEDED(pFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter)))) {
            DXGI_ADAPTER_DESC desc;
            if (SUCCEEDED(pAdapter->GetDesc(&desc))){
                std::cout << desc.DedicatedVideoMemory / 1024.0 / 1024 << "MiB meomry" << std::endl;

                if (desc.DedicatedVideoMemory > (UINT64)1800 * 1024 * 1024) {
                    device = desc.AdapterLuid;
                }
            }
            pAdapter->Release();
        }
        for (UINT adapterIndex = 0; ; ++adapterIndex) {
            if (pFactory->EnumAdapters1(adapterIndex, &pAdapter) == DXGI_ERROR_NOT_FOUND) {
                break;
            }

            DXGI_ADAPTER_DESC desc;
            if (SUCCEEDED(pAdapter->GetDesc(&desc))) {
                if (desc.AdapterLuid.HighPart == device.HighPart && desc.AdapterLuid.LowPart == device.LowPart) {
                    useDirectML_idx = adapterIndex;
                }
            }

            pAdapter->Release();
        }
    }

    Gdiplus::GdiplusShutdown(gdiToken);
    std::cout << useDirectML_idx << std::endl;
    return useDirectML_idx;
}
