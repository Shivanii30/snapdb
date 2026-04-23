
#pragma once

#include <string>
#include <map>
#include <memory>

namespace snapdb {

template<typename K, typename V>
class BPlusTree {
public:
    struct Node {
        bool isLeaf;
        std::map<K, V> values;
        std::map<K, std::unique_ptr<Node>> children;
        Node* next = nullptr;
        Node(bool leaf) : isLeaf(leaf) {}
    };

    BPlusTree() : root_(std::make_unique<Node>(true)) {}

    void insert(const K& key, const V& value) {
        findLeaf(root_.get(), key)->values[key] = value;
    }

    bool get(const K& key, V& value) {
        auto leaf = findLeaf(root_.get(), key);
        if (leaf->values.count(key)) {
            value = leaf->values[key];
            return true;
        }
        return false;
    }

    void remove(const K& key) {
        findLeaf(root_.get(), key)->values.erase(key);
    }

    void clear() {
        root_ = std::make_unique<Node>(true);
    }

    void dump(std::map<K, V>& out) {
        // Simplified dump for the basic B+ Tree structure provided
        dumpNode(root_.get(), out);
    }

private:
    void dumpNode(Node* node, std::map<K, V>& out) {
        if (node->isLeaf) {
            for (auto const& [k, v] : node->values) {
                out[k] = v;
            }
        } else {
            for (auto const& [k, child] : node->children) {
                dumpNode(child.get(), out);
            }
        }
    }

    Node* findLeaf(Node* current, const K& key) {
        if (current->isLeaf) return current;
        return current; 
    }
    std::unique_ptr<Node> root_;
};

} // namespace snapdb
