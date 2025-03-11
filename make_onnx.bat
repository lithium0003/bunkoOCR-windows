cd onnxruntime
call "..\openvino_genai_windows_2025.0.0.0_x86_64\setupvars.bat"
set TensorRT_PATH=C:\TensorRT-10.9.0.34

call build.bat --config Release --build_shared_lib --parallel --compile_no_warning_as_error --skip_submodule_sync --skip_tests --use_openvino CPU --use_dml --use_cuda --cudnn_home "%CUDA_PATH_V12_8%" --cuda_home "%CUDA_PATH_V12_8%" --use_tensorrt --tensorrt_home "%TensorRT_PATH%"

