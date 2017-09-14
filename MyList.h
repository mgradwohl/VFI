#pragma once
#include "WiseFile.h"
#include <vector>
#include <memory>

using namespace std;
typedef shared_ptr<CWiseFile> spWiseFile;
typedef vector<spWiseFile> MyVector;
typedef MyVector::iterator MyListIterator;


class MyList
{
private:
	MyVector list;

public:
	MyList();
	~MyList();

	void RemoveAll();
	size_t GetCount();
	bool isEmpty();
	shared_ptr<CWiseFile> RemoveHead();
	shared_ptr<CWiseFile> GetItemAt(size_t i);
	bool Remove(shared_ptr<CWiseFile> p);
	void AddTail(shared_ptr<CWiseFile> p);
	void AddHead(shared_ptr<CWiseFile> p);
};

