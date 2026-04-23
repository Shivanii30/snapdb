# SnapDB

A lightweight persistent key-value store written in C++.
Built as a systems programming project to explore storage engines and durability mechanisms.

## Features

- Key-value operations: `PUT`, `GET`, `DELETE`
- Write-Ahead Logging (WAL) for durability
- Crash recovery via WAL replay
- B+ Tree based in-memory indexing
- Batch operations support
- WAL compaction
- CLI interface
- Benchmark tool for performance testing


## Implementation

- WAL logs every write before applying changes
- `_commit` (Windows) used to ensure data is flushed to disk
- In-memory index (B+ Tree) used for fast lookups
- On startup, WAL is replayed to restore state
- Batch operations reduce disk flush overhead

## Build & Run

```bash
mkdir build
cd build
cmake ..
cmake --build . OR ninja
