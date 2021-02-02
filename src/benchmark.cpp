//
// Created by Ilya Pikulin on 02.02.2021.
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <fmt/core.h>

#include "istrstr.h"

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
        result = find_all(buffer.data(), buffer.size(), target);
    }
    uint64_t end = __rdtsc();
    auto end_time = std::chrono::high_resolution_clock::now();

    double bytes_per_cycle = 1. * (n_iters * buffer.size()) / (end - start);
    double gb_per_second = (buffer.size() / 1e9) * n_iters /
                           std::chrono::duration_cast<std::chrono::seconds>(
                                   std::chrono::duration<double>(end_time - start_time)).count();

    fmt::print(stderr, "Found {} results, {} bytes/cycle, {} GB/s\n",
               result.size(), bytes_per_cycle, gb_per_second);
    for (auto pos : result) {
        fmt::print("found at around {}\n", pos);
    }
}