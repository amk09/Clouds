clang++ -fopenmp -std=c++11 -Wno-deprecated-declarations main.cpp camera.cpp Cloud.cpp -o cloud
echo "compiled"
./cloud