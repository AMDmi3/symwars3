#include "binheap.h"
#include "pathnode.h"
#include "../utils.h"

Heap::Heap()
{
#if 0
	heap = NULL;
	allocsz = 0;
	nelements = 0;
#endif
}

Heap::~Heap()
{
	freemem();
}

void Heap::alloc(int ncells)
{
#if 0
	freemem();

	heap = new PathNode* [ ncells ];
	allocsz = ncells;
	nelements = 0;
#endif
}

void Heap::freemem()
{
#if 0
	if(heap)
	{
		delete [] heap;
		heap = NULL;
	}

	allocsz = 0;
	nelements = 0;
#else
	heap.clear();
#endif
}

void Heap::resetelems()
{
#if 0
	nelements = 0;
#else
	heap.clear();
#endif
}

bool Heap::insert(PathNode* element)
{
#if 0

	if(nelements >= allocsz)
		return false;

	heap[nelements] = element;
	nelements++;
	heapifyup(nelements - 1);
#else
	heap.push_back(element);
	heapifyup(heap.size() - 1);
#endif

	return true;
}

bool Heap::hasmore()
{
#if 0
	return nelements > 0;
#else
	return heap.size() > 0;
#endif
}

PathNode* Heap::deletemin()
{
#if 0
	PathNode* pmin = heap[0];
	heap[0] = heap[nelements - 1];
	nelements--;
	heapifydown(0);
	return pmin;
#else
	PathNode* pmin = heap.front();
	heap[0] = heap.at(heap.size() - 1);
	heap.pop_back();
	heapifydown(0);
	return pmin;
#endif
}

void Heap::heapify(PathNode* element)
{
#if 1
	if(heap.size() <= 0)
		return;

	if(element)
	{
		bool found = false;
		int i = 0;

		for(auto iter = heap.begin(); iter != heap.end(); iter++, i++)
			if(*iter == element)
			{
				found = true;
				break;
			}

		if(found)
		{
			heapifydown(i);
			heapifydown(i);
		}
		return;
	}
	for(int i = floor(heap.size()/2); i; i--)
	{
		heapifydown(i);
	}
	return;
#endif
}

#if 0
void Heap::print()
{
	std::vector<int>::iterator pos = heap.begin();
	cout << "Heap = ";
	while ( pos != heap.end() )
	{
		cout << *pos << " ";
		++pos;
	}
	cout << std::endl;
}
#endif

void Heap::heapifyup(int index)
{
	//cout << "index=" << index << std::endl;
	//cout << "parent(index)=" << parent(index) << std::endl;
	//cout << "heap[parent(index)]=" << heap[parent(index)] << std::endl;
	//cout << "heap[index]=" << heap[index] << std::endl;
	while ( ( index > 0 ) && ( parent(index) >= 0 ) &&
			( heap[parent(index)]->F > heap[index]->F ) )
	{
		PathNode* tmp = heap[parent(index)];
		heap[parent(index)] = heap[index];
		heap[index] = tmp;
		index = parent(index);
	}
}

void Heap::heapifydown(int index)
{
	//cout << "index=" << index << std::endl;
	//cout << "left(index)=" << left(index) << std::endl;
	//cout << "right(index)=" << right(index) << std::endl;
	int child = left(index);
	if ( ( child > 0 ) && ( right(index) > 0 ) &&
			( heap[child]->F > heap[right(index)]->F ) )
	{
		child = right(index);
	}
	if ( child > 0 )
	{
		PathNode* tmp = heap[index];
		heap[index] = heap[child];
		heap[child] = tmp;
		heapifydown(child);
	}
}

int Heap::left(int parent)
{
	int i = ( parent << 1 ) + 1; // 2 * parent + 1
#if 0
	return ( i < nelements ) ? i : -1;
#else
	return ( i < heap.size() ) ? i : -1;
#endif
}

int Heap::right(int parent)
{
	int i = ( parent << 1 ) + 2; // 2 * parent + 2
#if 0
	return ( i < nelements ) ? i : -1;
#else
	return ( i < heap.size() ) ? i : -1;
#endif
}

int Heap::parent(int child)
{
	if (child != 0)
	{
		int i = (child - 1) >> 1;
		return i;
	}
	return -1;
}

#if 0
int main()
{
	// Create the heap
	Heap* myheap = new Heap();
	myheap->insert(700);
	myheap->print();
	myheap->insert(500);
	myheap->print();
	myheap->insert(100);
	myheap->print();
	myheap->insert(800);
	myheap->print();
	myheap->insert(200);
	myheap->print();
	myheap->insert(400);
	myheap->print();
	myheap->insert(900);
	myheap->print();
	myheap->insert(1000);
	myheap->print();
	myheap->insert(300);
	myheap->print();
	myheap->insert(600);
	myheap->print();

	// Get priority element from the heap
	int heapSize = myheap->size();
	for ( int i = 0; i < heapSize; i++ )
		cout << "Get std::min element = " << myheap->deletemin() << std::endl;

	// Cleanup
	delete myheap;
}
#endif
