# bunkoOCR (windows)
findtextCenterNet https://github.com/lithium0003/findtextCenterNet で公開している機械学習モデルを、アプリとして使えるようにしたbunkoOCRのWindows版です。 
このプログラムは、画像からOCR(光学文字認識)を行い、テキストに変換します。
新しめのGPUがあると、非常に高速に実行できます。 

## Compile
### 必要なもの (onnxruntime)
- python
- cmake
- openvino https://storage.openvinotoolkit.org/repositories/openvino/packages/2025.0/windows/openvino_toolkit_windows_2025.0.0.17942.1f68be9f594_x86_64.zip
- CUDA Toolkit 12.8 https://developer.download.nvidia.com/compute/cuda/12.8.1/network_installers/cuda_12.8.1_windows_network.exe
- cuDNN https://developer.download.nvidia.com/compute/cudnn/redist/cudnn/windows-x86_64/cudnn-windows-x86_64-9.8.0.87_cuda12-archive.zip
- TensorRT https://developer.nvidia.com/downloads/compute/machine-learning/tensorrt/10.9.0/zip/TensorRT-10.9.0.34.Windows.win10.cuda-12.8.zip

インストールした場所に合わせて、make_onnx.batのパスを適宜修正してから、実行する。

## Run
実行時には、DLLが必要となります。
- onnxruntime onnxruntime/build/build/Windows/Release/Release　から取ってくる
- CUDA Toolkit
- cuDNN
- TensorRT
- openvino runtime/bin/intel64/Release と runtime/3rdparty/tbb/bin から取ってくる

実行フォルダに、onnxモデルが必要です。
https://huggingface.co/lithium0003/findtextCenterNet から、onnxモデルを5つダウンロードして、配置します。

bunkoOCR.exeがGUIの実行ファイルです。内部で、OCRengine.exeを呼び出して処理します。
