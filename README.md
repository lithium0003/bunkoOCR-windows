# bunkoOCR (windows)
findtextCenterNet https://github.com/lithium0003/findtextCenterNet で公開している機械学習モデルを、アプリとして使えるようにしたbunkoOCRのWindows版です。 
このプログラムは、画像からOCR(光学文字認識)を行い、テキストに変換します。
新しめのGPUがあると、非常に高速に実行できます。 

## Compile
### 必要なもの (onnxruntime)
- python
- cmake
- openvino https://docs.openvino.ai/2022.3/openvino_docs_install_guides_installing_openvino_from_archive_windows.html
- CUDA Toolkit 11.8 https://developer.nvidia.com/cuda-11-8-0-download-archive
- cuDNN https://developer.nvidia.com/downloads/compute/cudnn/secure/8.9.5/local_installers/11.x/cudnn-windows-x86_64-8.9.5.29_cuda11-archive.zip/
- TensorRT https://developer.nvidia.com/downloads/compute/machine-learning/tensorrt/secure/8.6.1/zip/TensorRT-8.6.1.6.Windows10.x86_64.cuda-11.8.zip

インストールした場所に合わせて、make_onnx.batのパスを適宜修正してから、実行する。

## Run
実行時には、DLLが必要となります。
- onnxruntime onnxruntime/build/build/Windows/RelWithDebInfo/RelWithDebInfo　から取ってくる
- CUDA Toolkit
- cuDNN
- TensorRT
- openvino runtime/bin/intel64/Release と runtime/3rdparty/tbb/bin から取ってくる

実行フォルダに、onnxモデルが必要です。
https://github.com/lithium0003/findtextCenterNet のReleaseから、onnxモデルを4つダウンロードして、配置します。

bunkoOCR.exeがGUIの実行ファイルです。内部で、OCRengine.exeを呼び出して処理します。
