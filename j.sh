#script for jue. If this does not compile for you, check        line 776: change 'len = snprintf(buffer, sizeof(buffer), "EXPOSURE=          1.0000000000000\n\n-Y %d +X %d\n", y, x);'' into 'len = sprintf(buffer, "EXPOSURE=          1.0000000000000\n\n-Y %d +X %d\n", y, x);''
export MACOSX_DEPLOYMENT_TARGET=13.7

# Set paths for LLVM installed via Homebrew
export PATH="/opt/homebrew/Cellar/llvm/19.1.5/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/Cellar/llvm/19.1.5/lib"
export CPPFLAGS="-I/opt/homebrew/Cellar/llvm/19.1.5/include"

# Update compiler paths
export CC=/opt/homebrew/Cellar/llvm/19.1.5/bin/clang
export CXX=/opt/homebrew/Cellar/llvm/19.1.5/bin/clang++

# OpenMP flags
CFLAGS="-fopenmp"
LDFLAGS="-fopenmp"

# Use the correct clang++ from the detected LLVM path
LLVM_PATH=$(brew --prefix llvm)/bin

# Compile the code
$LLVM_PATH/clang++ $CFLAGS main.cpp Cloud.cpp Camera.cpp -o main $LDFLAGS

./main