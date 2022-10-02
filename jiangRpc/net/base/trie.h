#pragma once
#include <string>
#include <limits.h>
#include <utility>


namespace jiangRpc {
template <typename T>
struct TrieNode {
    char c;
    int flag = 0;
    T val;
    TrieNode* child[CHAR_MAX];

    TrieNode()
    {
        for (int i = 0; i < CHAR_MAX; i++) {
            child[i] = nullptr;
        }
    }
};


template <typename T>
class Trie {
public:
    Trie() { root = new TrieNode<T>(); root->c = '/';}
    void insert(std::string path, T val);
	std::pair<T, bool> search(std::string path);

	~Trie() {release(root); root = nullptr;}
private:
	void release(TrieNode<T>* node);
private:
    TrieNode<T>* root;
};

template <typename T>
void Trie<T>::insert(std::string path, T val)
{
    TrieNode<T>* node = root;
	int pathSize = path.size();
	int i = 0;
	if (pathSize > 0 && path[0] == '/') {
		i = 1;
	}
    for (; i < pathSize; i++) {
		int c = path[i];
        if (node->child[c] == nullptr) {
            node->child[c] = new TrieNode<T>;
            node->c = path[i];
        }
		node = node->child[c];
    }
	node->flag = 1;
	node->val = val;
}

template <typename T>
std::pair<T, bool> Trie<T>::search(std::string path)
{
    TrieNode<T>* node = root;
	int pathSize = path.size();
	int i = 0;
	if (pathSize > 0 && path[0] == '/') {
		i = 1;
	}
    for (; i < pathSize; i++){
		int c = path[i];
        if(node->child[c] == nullptr){
            return std::make_pair((T)0, false);
        }
        node = node->child[c];
    }
    if (node->flag == 0) 
        return std::make_pair((T)0, false);
    return std::make_pair(node->val, true);
}


template <typename T>
void Trie<T>::release(TrieNode<T>* node)
{
	if (node == nullptr)
		return;
	for (int i = 0; i < CHAR_MAX; i++) {
		release(node->child[i]);
	}
	delete node;
}

}
