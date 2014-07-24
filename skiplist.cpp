#include <assert.h>

#include "skiplist.h"

SkipList::Node* SkipList::NewNode(const int key, int height)
{
	char *mem = (char *)malloc(sizeof(Node) + height*sizeof(Node *));
	return new (mem) Node(key);
}


SkipList::SkipList():head(NewNode(0, kMaxHeight)), height(1)
{
	for (int i = 0; i < kMaxHeight; i ++) {
		head->SetNext(i, NULL);
	}
}

SkipList::~SkipList()
{
	Node *n = head;
	Node *next;

	while (n) {
		next = n->Next(0);
		free(n);
		n = next;
	}
}

void SkipList::Insert(const int key)
{
	Node *update[kMaxHeight];
	Node *n = head;

	for (int i = height-1; i >= 0; i --) {
		while(n->Next(i) != NULL && n->Next(i)->key < key)
			n = n->Next(i);
		update[i] = n;
	}
	
	int level = RandomLevel();
	if (level > height) {
		for (int i = height; i < level; i ++) {
			update[i] = head;
		}
		height = level;
	}

	Node *node = NewNode(key, level);
	for (int i = 0; i < level; i ++) {
		node->SetNext(i, update[i]->Next(i));
		update[i]->SetNext(i, node);
	}
}

int SkipList::RandomLevel( )
{
	static const unsigned int kBranching = 4;
	int height = 1;
	while (height < kMaxHeight && ((rand() % kBranching) == 0)) {
		height++;
	}
	assert(height > 0);
	assert(height <= kMaxHeight);
	return height;
}

void SkipList::Debug() {
	for (int i = 0; i < height; i ++) {
		Node *x = head->Next(i);
		fprintf(stderr, "Level[%d]: ", i);
		while (x) {
			fprintf(stderr, "%d@%p -> ", x->key, x);
			x = x->Next(i);
		}
		fprintf(stderr, "\n");
	}
}

bool SkipList::Contains(const int key)
{
	Node *x = head;
	for (int i = height - 1; i >= 0; i --) {
		while (x->Next(i) != NULL) {
			if (x->Next(i)->key < key) x = x->Next(i);
			else if (x->Next(i)->key == key) return true;
			else break;
		}
	}
	return false;
}

void SkipList::Erase(const int key)
{
	Node *update[kMaxHeight], *x;
	x = head;
	for (int i = height-1; i >= 0; i --) {
		while (x->Next(i) && x->Next(i)->key < key) {
			x = x->Next(i);
		}
		update[i] = x;
	}
	x = x->Next(0);
	if (x && x->key == key) {
		for (int i = 0; i < height; i ++) {
			if (update[i]->Next(i) == x) {
				update[i]->SetNext(i, x->Next(i));
			}
		}
		for (int i = height-1; i >= 0; i --) {
			if (head->Next(i) == NULL) height --;
		}
		free(x);
	}
}

SkipList::Iterator::Iterator(const SkipList* list)
{
	list_ = list;
	node_ = NULL;
}

bool SkipList::Iterator::Valid() const
{
	return node_!=NULL;
}
		
const int SkipList::Iterator::key() const
{
	assert(Valid());
	return node_->key;
}
		
void SkipList::Iterator::Next()
{
	assert(Valid());
	node_ = node_->Next(0);
}

void SkipList::Iterator::Prev()
{
	// Not Supported.
	assert(false);
}

void SkipList::Iterator::Seek(const int &target)
{
	// Not Supported.
	assert(false);
}

void SkipList::Iterator::SeekToFirst()
{
	node_ = list_->head->Next(0);
}

void SkipList::Iterator::SeekToLast()
{
	// Not Supported.
	assert(false);
}
