/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Storage.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <string>


void runBenchmark(int iterations) {
    snapdb::StorageEngine db("./bench_data");
    
    std::cout << "Starting SnapDB Benchmark (" << iterations << " iterations)...\n";
    std::cout << "--------------------------------------------------\n";

    // 1. Write Benchmark
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        db.put("key_" + std::to_string(i), "value_data_" + std::to_string(i));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> write_elapsed = end - start;
    
    // 2. Read Benchmark
    start = std::chrono::high_resolution_clock::now();
    std::string value;
    for (int i = 0; i < iterations; ++i) {
        db.get("key_" + std::to_string(i), value);
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> read_elapsed = end - start;

    // Results
    double write_ops = iterations / write_elapsed.count();
    double read_ops = iterations / read_elapsed.count();
    double avg_write_lat = (write_elapsed.count() * 1000.0) / iterations;
    double avg_read_lat = (read_elapsed.count() * 1000.0) / iterations;

    std::cout << "WRITE PERFORMANCE:\n";
    std::cout << "  Throughput: " << write_ops << " ops/sec\n";
    std::cout << "  Avg Latency: " << avg_write_lat << " ms\n\n";

    std::cout << "READ PERFORMANCE:\n";
    std::cout << "  Throughput: " << read_ops << " ops/sec\n";
    std::cout << "  Avg Latency: " << avg_read_lat << " ms\n";
    std::cout << "--------------------------------------------------\n";
}

int main(int argc, char* argv[]) {
    int iterations = 5000;
    if (argc > 1) {
        iterations = std::stoi(argv[1]);
    }
    
    runBenchmark(iterations);
    return 0;
}
