cd onnxruntime
call "C:\Program Files (x86)\Intel\openvino_2023\setupvars.bat"
set TensorRT_PATH=C:\TensorRT-8.6.1.6
call build.bat --config  RelWithDebInfo --build_shared_lib --use_openvino CPU_FP32 --use_dml --use_cuda --cudnn_home "%CUDA_PATH_V11_8%" --cuda_home "%CUDA_PATH_V11_8%" --use_tensorrt --tensorrt_home "%TensorRT_PATH%" --parallel --skip_test --cmake_generator "Visual Studio 17 2022"

