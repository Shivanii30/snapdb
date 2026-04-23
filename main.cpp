/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Storage.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>

void printHelp() {
    std::cout << "SnapDB CLI\n"
              << "Commands:\n"
              << "  PUT <key> <value>    - Store a key-value pair\n"
              << "  GET <key>            - Retrieve value for a key\n"
              << "  DELETE <key>         - Remove a key\n"
              << "  BATCH <ops...>       - Execute ops (e.g. PUT:k:v DELETE:k)\n"
              << "  COMPACT              - Compact WAL log\n"
              << "  BENCHMARK <n>        - Build-in simple health check\n"
              << "  EXIT                 - Quit\n"
              << "-----------------------------------\n";
}

int main() {
    snapdb::StorageEngine db("./data");
    std::string input;
    
    std::cout << "Welcome to SnapDB. Type 'HELP' for commands.\n";
    
    while (true) {
        std::cout << "snapdb> ";
        if (!std::getline(std::cin, input)) break;
        
        std::stringstream ss(input);
        std::string command;
        ss >> command;
        
        std::transform(command.begin(), command.end(), command.begin(), ::toupper);
        
        if (command == "EXIT") {
            break;
        } else if (command == "HELP") {
            printHelp();
        } else if (command == "COMPACT") {
            db.compact();
            std::cout << "WAL compacted.\n";
        } else if (command == "BENCHMARK") {
            int n = 1000;
            if (ss >> n) {}
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < n; ++i) {
                db.put("b_" + std::to_string(i), "v_" + std::to_string(i));
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            std::cout << "Benchmarked " << n << " writes in " << (elapsed.count() * 1000) << "ms\n";
        } else if (command == "BATCH") {
            db.startBatch();
            std::string opStr;
            while (ss >> opStr) {
                size_t firstColon = opStr.find(':');
                if (firstColon == std::string::npos) continue;
                std::string type = opStr.substr(0, firstColon);
                std::string rest = opStr.substr(firstColon + 1);
                
                if (type == "PUT") {
                    size_t secondColon = rest.find(':');
                    if (secondColon != std::string::npos) {
                        db.put(rest.substr(0, secondColon), rest.substr(secondColon + 1));
                    }
                } else if (type == "DELETE") {
                    db.remove(rest);
                }
            }
            db.endBatch();
            std::cout << "Batch executed.\n";
        } else if (command == "PUT") {
            std::string key, value;
            ss >> key;
            std::getline(ss, value);
            if (!value.empty() && value[0] == ' ') value.erase(0, 1);
            
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }

            if (key.empty() || value.empty()) {
                std::cout << "Error: Usage PUT <key> <value>\n";
            } else {
                db.put(key, value);
                std::cout << "OK (Stored key: \"" << key << "\", value: " << value << ")\n";
            }
        } else if (command == "GET") {
            std::string key, value;
            ss >> key;
            if (key.empty()) {
                std::cout << "Error: Usage GET <key>\n";
            } else if (db.get(key, value)) {
                std::cout << value << "\n";
            } else {
                std::cout << "(nil)\n";
            }
        } else if (command == "DELETE") {
            std::string key;
            ss >> key;
            if (key.empty()) {
                std::cout << "Error: Usage DELETE <key>\n";
            } else {
                db.remove(key);
                std::cout << "OK\n";
            }
        } else if (!command.empty()) {
            std::cout << "Unknown command: " << command << "\n";
        }
    }
    
    return 0;
}
