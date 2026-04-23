/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

#include "WAL.hpp"
#include <iostream>
#include <algorithm>
#include <fcntl.h>
#include <sys/stat.h>
#include <map>

#if defined(_WIN32) || defined(_WIN64)
    #include <io.h>     // Windows
#else
    #include <unistd.h> // Linux/Mac
#endif



namespace snapdb {

std::string LogRecord::serialize() const {
    std::stringstream ss;
    char t = 'P';
    if (type == Type::DELETE) t = 'D';
    else if (type == Type::BATCH_START) t = 'S';
    else if (type == Type::BATCH_END) t = 'E';
    
    ss << t << "|" << key << "|" << value << "\n";
    return ss.str();
}

LogRecord LogRecord::deserialize(const std::string& line, bool& success) {
    success = false;
    if (line.size() < 2) return {};
    
    char typeChar = line[0];
    Type t;
    if (typeChar == 'P') t = Type::PUT;
    else if (typeChar == 'D') t = Type::DELETE;
    else if (typeChar == 'S') t = Type::BATCH_START;
    else if (typeChar == 'E') t = Type::BATCH_END;
    else return {}; // Corruption

    if (line[1] != '|') return {}; // Corruption

    std::string key, value;
    size_t deviderPipe = line.find('|', 2);
    if (deviderPipe == std::string::npos) return {}; // Corruption
    
    key = line.substr(2, deviderPipe - 2);
    value = line.substr(deviderPipe + 1);
    
    success = true;
    return { t, key, value };
}

WALManager::WALManager(const std::string& filename) : filename_(filename) {
}

void WALManager::logPut(const std::string& key, const std::string& value) {
    LogRecord record{ LogRecord::Type::PUT, key, value };
    write(record.serialize());
}

void WALManager::logDelete(const std::string& key) {
    LogRecord record{ LogRecord::Type::DELETE, key, "" };
    write(record.serialize());
}

void WALManager::logBatchStart() {
    LogRecord record{ LogRecord::Type::BATCH_START, "", "" };
    write(record.serialize());
}

void WALManager::logBatchEnd() {
    LogRecord record{ LogRecord::Type::BATCH_END, "", "" };
    write(record.serialize());
}

std::vector<LogRecord> WALManager::readAll() {
    std::vector<LogRecord> records;
    std::ifstream reader(filename_);
    std::string line;
    while (std::getline(reader, line)) {
        if (!line.empty()) {
            bool success;
            LogRecord rec = LogRecord::deserialize(line, success);
            if (success) {
                records.push_back(rec);
            } else {
                std::cerr << "Warning: Detected corruption in WAL. Skipping entry." << std::endl;
            }
        }
    }
    return records;
}

void WALManager::compact(const std::map<std::string, std::string>& snapshot) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string tempFile = filename_ + ".tmp";
    std::ofstream writer(tempFile, std::ios::trunc | std::ios::out);
    
    for (const auto& [k, v] : snapshot) {
        LogRecord rec{ LogRecord::Type::PUT, k, v };
        writer << rec.serialize();
    }
    writer.close();
    
    // Atomically replace
    std::remove(filename_.c_str());
    std::rename(tempFile.c_str(), filename_.c_str());
}

void WALManager::clear() {
    std::ofstream(filename_, std::ios::trunc).close();
}

void WALManager::write(const std::string& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    int fd = open(filename_.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd != -1) {
        size_t written = ::write(fd, data.c_str(), data.size());
        if (written == data.size()) {
           // Force hardware flush
#ifdef _WIN32
            _commit(fd);
#else
            fdatasync(fd);
#endif
        }
        close(fd);
    }
}

} // namespace snapdb
