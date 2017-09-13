#include "stdafx.h"
#include "MyList.h"

MyList::MyList()
{
}

MyList::~MyList()
{
	list.clear();
}

void MyList::RemoveAll()
{
	list.clear();
}

size_t MyList::GetCount()
{
	return list.size();
}

bool MyList::isEmpty()
{
	return list.empty();
}

shared_ptr<CWiseFile> MyList::RemoveHead()
{
	if (list.empty()) return nullptr;

	shared_ptr<CWiseFile> p = list.front();
	list.erase(list.begin());
	return p;
}

shared_ptr<CWiseFile> MyList::GetItemAt(size_t i)
{
	if (list.empty()) return nullptr;
	return list[i];
}

bool MyList::Remove(shared_ptr<CWiseFile> p)
{
	MyListIterator i = find(list.begin(), list.end(), p);
	if (i != list.end())
	{
		list.erase(i);
		return true;
	}

	return false;
}

void MyList::AddTail(shared_ptr<CWiseFile> p)
{
	list.push_back(p);
}

void MyList::AddHead(shared_ptr<CWiseFile> p)
{
	list.insert(list.begin(), p);
}
