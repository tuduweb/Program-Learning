#include <iostream>
using namespace std;

class Data
{
public:
	short a;
	short b;
};

void delete_ptr(void *p)
{
    if(p != nullptr)
    {
        //delete p; 
        p=nullptr;
    }
}

int main()
{

	long value = 0xAAAABBBB;
	Data *pData = reinterpret_cast<Data*>(&value);
	cout << hex << pData->a << endl;
	cout << hex << pData->b << endl;
	//不需要释放指针pData的空间，它的地址和变量value相同，会自动释放掉
	//delete_ptr(pData);
	return 0;
}

