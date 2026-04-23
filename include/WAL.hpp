/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <sstream>

namespace snapdb {

struct LogRecord {
    enum class Type { PUT, DELETE, BATCH_START, BATCH_END };
    Type type;
    std::string key;
    std::string value;

    std::string serialize() const;
    static LogRecord deserialize(const std::string& line, bool& success);
};

class WALManager {
public:
    WALManager(const std::string& filename);
    void logPut(const std::string& key, const std::string& value);
    void logDelete(const std::string& key);
    void logBatchStart();
    void logBatchEnd();
    std::vector<LogRecord> readAll();
    void compact(const std::map<std::string, std::string>& snapshot);
    void clear();

private:
    void write(const std::string& data);
    std::string filename_;
    std::mutex mutex_;
};

} // namespace snapdb
