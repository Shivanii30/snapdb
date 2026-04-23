/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

#include "Storage.hpp"

namespace snapdb {
static std::string stripQuotes(const std::string& val) {
    if (val.size() >= 2 && val.front() == '"' && val.back() == '"') {
        return val.substr(1, val.size() - 2);
    }
    return val;
}

StorageEngine::StorageEngine(const std::string& baseDir) 
    : baseDir_(baseDir), wal_(baseDir + "/snap.wal") {
    recover();
}

void StorageEngine::put(const std::string& key, const std::string& value) {
    wal_.logPut(key, stripQuotes(value));
    memIndex_.insert(key, stripQuotes(value));
    if (!isInBatch_) {
        persist();
    }
}

bool StorageEngine::get(const std::string& key, std::string& value) {
    return memIndex_.get(key, value);
}

void StorageEngine::remove(const std::string& key) {
    wal_.logDelete(key);
    memIndex_.remove(key);
    if (!isInBatch_) {
        persist();
    }
}

void StorageEngine::startBatch() {
    isInBatch_ = true;
    wal_.logBatchStart();
}

void StorageEngine::endBatch() {
    isInBatch_ = false;
    wal_.logBatchEnd();
    persist();
}

void StorageEngine::compact() {
    std::map<std::string, std::string> snapshot;
    memIndex_.dump(snapshot);
    wal_.compact(snapshot);
}

std::map<std::string, std::string> StorageEngine::getSnapshot() {
    std::map<std::string, std::string> snapshot;
    memIndex_.dump(snapshot);
    return snapshot;
}

void StorageEngine::recover() {
    auto records = wal_.readAll();
    bool inBatch = false;
    for (const auto& rec : records) {
        if (rec.type == LogRecord::Type::BATCH_START) {
            inBatch = true;
        } else if (rec.type == LogRecord::Type::BATCH_END) {
            inBatch = false;
        } else if (rec.type == LogRecord::Type::PUT) {
            memIndex_.insert(rec.key, rec.value);
        } else if (rec.type == LogRecord::Type::DELETE) {
            memIndex_.remove(rec.key);
        }
    }
}

void StorageEngine::persist() {
    // Page management logic would go here in a full implementation
}

} // namespace snapdb
