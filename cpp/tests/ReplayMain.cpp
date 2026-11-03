#include <charconv>
#include <cstdio>
#include <string_view>

extern "C" unsigned long long A(unsigned int);
extern "C" volatile unsigned long long observedLength;

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    const std::string_view text = argv[1];
    unsigned int input = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), input);
    if (error != std::errc{} || end != text.data() + text.size()) return 2;
    A(input);
    std::printf("%llu\n", observedLength);
    return 0;
}
