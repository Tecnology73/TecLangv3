echo "Building..."
clang++ ./out/out.ll -o ./out/app.exe || exit 1
echo "--- START ---"
./out/app.exe
R=$?
echo "--- END ---"
echo "Exit code: ${R}"