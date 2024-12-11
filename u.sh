g++ -fopenmp -std=c++11 -Wno-deprecated-declarations main.cpp terrain_and_trees.cpp noise.cpp terrain.cpp trees.cpp camera.cpp Cloud.cpp -o cloud
echo "compiled"
./cloud