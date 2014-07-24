#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <iostream>
using namespace std;

class SkipList
{
private:
	struct Node;
public:
	SkipList();
	~SkipList();

	void Insert(const int key);
	bool Contains(const int key);
	void Erase(const int key);

	class Iterator {
	public:
		explicit Iterator(const SkipList* list);
		
		// Returns true iff the iterator is positioned at a valid node
		bool Valid() const;
		
		const int key() const;
		
		void Next();
		void Prev();
		
		// Advance to the first entry with a key>= target
		void Seek(const int &target);
		// Postition at the first entry in list.
		void SeekToFirst();
		// Postition at the last entry in list
		void SeekToLast();
    private:
		const SkipList* list_;
		Node* node_;
	};

	void Debug();

private:
	Node* NewNode(const int key, int height);
	int RandomLevel();
private:
	enum { kMaxHeight = 12 };
	struct Node {
		int const key;
		Node *next[1];
		explicit Node(const int k) : key(k) { }
		
		Node* Next(int n) {
			assert(n >= 0);
			return next[n];
		}
		void SetNext(int n, Node *x) {
			assert(n >= 0);
			next[n] = x;
		}
	};
	
	Node* const head;
	int height;
};

#endif
