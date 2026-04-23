/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "SnapDB.hpp"
#include "WAL.hpp"
#include <string>

namespace snapdb {

class StorageEngine {
public:
    StorageEngine(const std::string& baseDir);
    void put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value);
    void remove(const std::string& key);
    
    // Batch operations
    void startBatch();
    void endBatch();
    
    // WAL Management
    void compact();
    std::map<std::string, std::string> getSnapshot(); // Helper for compaction

private:
    void recover();
    void persist();

    std::string baseDir_;
    WALManager wal_;
    BPlusTree<std::string, std::string> memIndex_;
    bool isInBatch_ = false;
};

} // namespace snapdb
