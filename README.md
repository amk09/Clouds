# Clouds

Current output : 

<img src="output.png"></img>
```
mkdir build
cd build
cmake ..
cmake --build .
./Clouds
```


```bash
clang++ -fopenmp --std=c++11 -stdlib=libc++ main.cpp camera.cpp -o main
./main
```