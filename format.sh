# run this script to format all the source code
find . \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) \
    -not -path "./third_party/*" | xargs clang-format -i
