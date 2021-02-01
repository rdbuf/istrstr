#include <fmt/core.h>
#include <array>
#include <immintrin.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <x86intrin.h>
#include <cstdlib>
#include <chrono>

// Algorithm:
// 1. Take chunk of 4*m
// 2. Make it lowercase
// 3. Compare 4-byte sequences
std::vector<uint64_t> find_all(const char *text, const char target[4]) {
    const uint64_t text_n = strlen(text);

    std::vector<uint64_t> result;

    // Prepare target match vector
    std::array<uint8_t, 32> tgt_struct{};
    memcpy(&tgt_struct[0], target, 4);
    memcpy(&tgt_struct[4], target, 4);
    memcpy(&tgt_struct[8], target, 4);
    memcpy(&tgt_struct[12], target, 4);
    memcpy(&tgt_struct[16], target, 4);
    memcpy(&tgt_struct[20], target, 4);
    memcpy(&tgt_struct[24], target, 4);
    memcpy(&tgt_struct[28], target, 4);
    __m256i tgt = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&tgt_struct));

    // after each `i` we must have at least 35 bytes available
    uint64_t loop_n = (text_n >= 35) * (((text_n - 35) / 29) * 29);
    uint64_t remainder_n = text_n - loop_n;

    for (int i = 0; i < loop_n; i += 29) {
        // Prepare 4 source match vectors w/ different offsets
        __m256i src0 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&text[i + 0]));
        __m256i res0 = _mm256_cmpeq_epi32(src0, tgt);

        __m256i src1 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&text[i + 1]));
        __m256i res1 = _mm256_cmpeq_epi32(src1, tgt);

        __m256i src2 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&text[i + 2]));
        __m256i res2 = _mm256_cmpeq_epi32(src2, tgt);

        __m256i src3 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&text[i + 3]));
        __m256i res3 = _mm256_cmpeq_epi32(src3, tgt);

        if (!_mm256_testz_si256(res0, res0) ||
            !_mm256_testz_si256(res1, res1) ||
            !_mm256_testz_si256(res2, res2) ||
            !_mm256_testz_si256(res3, res3)) {
            result.push_back(i);
        }
    }
    {
        std::array<uint8_t, 32> src_struct_0{};
        memcpy(&src_struct_0[0], &text[loop_n + 0], std::min<uint64_t>(32, remainder_n));
        __m256i src0 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&src_struct_0));
        __m256i res0 = _mm256_cmpeq_epi32(src0, tgt);
        std::array<uint8_t, 32> src_struct_1{};
        memcpy(&src_struct_1[0], &text[loop_n + 1], std::min<uint64_t>(28, remainder_n - 1));
        __m256i src1 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&src_struct_1));
        __m256i res1 = _mm256_cmpeq_epi32(src1, tgt);
        std::array<uint8_t, 32> src_struct_2{};
        memcpy(&src_struct_2[0], &text[loop_n + 2], std::min<uint64_t>(28, remainder_n - 2));
        __m256i src2 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&src_struct_2));
        __m256i res2 = _mm256_cmpeq_epi32(src2, tgt);
        std::array<uint8_t, 32> src_struct_3{};
        memcpy(&src_struct_3[0], &text[loop_n + 3], std::min<uint64_t>(28, remainder_n - 3));
        __m256i src3 = _mm256_loadu_si256(reinterpret_cast<const __m256i_u *>(&src_struct_3));
        __m256i res3 = _mm256_cmpeq_epi32(src3, tgt);

        if (!_mm256_testz_si256(res0, res0) ||
            !_mm256_testz_si256(res1, res1) ||
            !_mm256_testz_si256(res2, res2) ||
            !_mm256_testz_si256(res3, res3)) {
            result.push_back(loop_n);
        }
    }

    return result;
}

int main(int argc, const char **argv) {
    if (argc < 3) {
        fmt::print("usage: {} text target [n_iters]\n", argv[0]);
        return 1;
    }
    const char *text_filename = argv[1];
    const char *target = argv[2];
    assert((strlen(target) == 4));

    int n_iters = 5;
    if (argc == 4) {
        n_iters = atoi(argv[3]);
    }


    std::vector<char> buffer;
    std::ifstream f(text_filename);
    f.seekg(0, std::ios::end);
    buffer.resize(f.tellg());
    f.seekg(0);
    f.read(buffer.data(), buffer.size());

    fmt::print(stderr, "Searching... n_iters={}\n", n_iters);

    auto start_time = std::chrono::high_resolution_clock::now();
    uint64_t start = __rdtsc();
    std::vector<uint64_t> result;
    for (int iter = 0; iter < n_iters; ++iter) {
        result = find_all(buffer.data(), target);
    }
    uint64_t end = __rdtsc();
    auto end_time = std::chrono::high_resolution_clock::now();

    double bytes_per_cycle = 1. * (n_iters * buffer.size()) / (end - start);
    double gb_per_second = (buffer.size() / 1e9) * n_iters /
                           std::chrono::duration_cast<std::chrono::seconds>(
                                   std::chrono::duration<double>(end_time - start_time)).count();

    fmt::print(stderr, "Found {} results, {} bytes/cycle, {} Gb/s\n",
               result.size(), bytes_per_cycle, gb_per_second);
    for (auto pos : result) {
        fmt::print("found at around {}\n", pos);
    }
}