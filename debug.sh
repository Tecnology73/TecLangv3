echo "Building..."
clang++ ./build/out.ll -o ./build/app.exe || exit 1
echo "--- START ---"
./build/app.exe
R=$?
echo "--- END ---"
echo "Exit code: ${R}"
