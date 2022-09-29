# CMake generated Testfile for 
# Source directory: C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests
# Build directory: C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvlibs/src/nvtt/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(NVTT.TestSuite.Kodak.cuda "nvtestsuite" "-path" "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/data/testsuite" "-set" "0" "-out" "output-cuda-kodak")
set_tests_properties(NVTT.TestSuite.Kodak.cuda PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;7;ADD_TEST;C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;0;")
add_test(NVTT.TestSuite.Waterloo.cuda "nvtestsuite" "-path" "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/data/testsuite" "-set" "1" "-out" "output-cuda-waterloo")
set_tests_properties(NVTT.TestSuite.Waterloo.cuda PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;8;ADD_TEST;C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;0;")
add_test(NVTT.TestSuite.Epic.cuda "nvtestsuite" "-path" "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/data/testsuite" "-set" "2" "-out" "output-cuda-epic")
set_tests_properties(NVTT.TestSuite.Epic.cuda PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;9;ADD_TEST;C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;0;")
add_test(NVTT.TestSuite.Kodak.nocuda "nvtestsuite" "-path" "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/data/testsuite" "-set" "0" "-nocuda" "-out" "output-nocuda-kodak")
set_tests_properties(NVTT.TestSuite.Kodak.nocuda PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;10;ADD_TEST;C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;0;")
add_test(NVTT.TestSuite.Waterloo.nocuda "nvtestsuite" "-path" "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/data/testsuite" "-set" "1" "-nocuda" "-out" "output-nocuda-waterloo")
set_tests_properties(NVTT.TestSuite.Waterloo.nocuda PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;11;ADD_TEST;C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;0;")
add_test(NVTT.TestSuite.Epic.nocuda "nvtestsuite" "-path" "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/data/testsuite" "-set" "2" "-nocuda" "-out" "output-nocuda-epic")
set_tests_properties(NVTT.TestSuite.Epic.nocuda PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;12;ADD_TEST;C:/Users/tian.xie/Desktop/vulkan-guide/third_party/nvidia-texture-tools/src/nvtt/tests/CMakeLists.txt;0;")
