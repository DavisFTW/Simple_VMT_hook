// TargetApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
class A
{
public:
	A() { data = 4; }

	int data;

	virtual int getData()
	{
		return data;
	}

	virtual void printMessage()
	{
		std::cout << "this is A\n";
	}
};

class B : public A
{
public:
	B() : A()
	{
		data = 0;
	}

	int getData()
	{
		return 6;
	}

	void printMessage()
	{
		std::cout << "Dude this is B!\n";
	}
};

void fun(A* a_instance)
{
	a_instance->printMessage();
	std::cout << a_instance->getData() << "\n\n";
}

int main()
{
	A a;
	B b;
	
	std::uintptr_t* vtable = *reinterpret_cast<std::uintptr_t**>(&a);

	std::uintptr_t* ptr = reinterpret_cast<std::uintptr_t*>((DWORD)vtable - (DWORD)GetModuleHandleA(nullptr));

	std::cout << (void*)ptr << std::endl;
	std::cout << ptr << std::endl;
	while (true)
	{
		fun(&a);
		fun(&b);
		Sleep(5000);
	}

	return 1;
}

