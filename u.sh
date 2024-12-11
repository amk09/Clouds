clang++ -fopenmp -std=c++17 -Wno-deprecated-declarations main.cpp camera.cpp Cloud.cpp -o cloud
echo "compiled"
./cloud