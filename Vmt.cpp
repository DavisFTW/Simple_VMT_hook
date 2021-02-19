#include <iostream>
#include "windows.h"

#define LOG(x) { std::cout << x << std::endl; }

using VirtualFn1_t = void(__thiscall*)(void* thisptr, int a, int b);
VirtualFn1_t orig_VirtualFn1;

class VirtualClass
{
public:

    int number;

    virtual void VirtualFn1(int a, int b) //This is the virtual function that will be hooked.
    {
        std::cout << a + b << std::endl;
    }
    virtual void VirtualFn2(int a, int b) 
    {
        std::cout << a - b << std::endl;
    }

    virtual void getBase() { std::cout << (this) << std::endl; }
};

void __fastcall hkVirtualFn1(int a, int b, std::uintptr_t* thisptr, int edx) // hook function
{
    a = 9999;
    b = 9999;

    orig_VirtualFn1(thisptr, a , b); //Call the original function.
}

int main()
{
    DWORD oldProtection = 0;

    VirtualClass* vClass = new VirtualClass();

    // get our class pointer and our vmt pointer
    auto class_pointer = reinterpret_cast<std::uintptr_t**>(vClass);   // class ptr
    auto vmt_pointer = reinterpret_cast<std::uintptr_t*>(*class_pointer); // class_vmt pointer


    std::cout << (void*)class_pointer << std::endl;
    std::cout << (void*)*vmt_pointer << std::endl;  // dereferenced vmt pointer leads us to 


    VirtualProtect(vmt_pointer, 4, PAGE_EXECUTE_READWRITE, &oldProtection);  // 

    orig_VirtualFn1 = reinterpret_cast<VirtualFn1_t>(*vmt_pointer);   // store our origal function pointer 
    *vmt_pointer = reinterpret_cast<std::uintptr_t>(&hkVirtualFn1);   // redirect our pointers

    VirtualProtect(vmt_pointer, 4, oldProtection, NULL);    // set the old protection back
    vClass->VirtualFn1(5, 5);
    vClass->VirtualFn1(5, 5);

    std::cout << "----------------------------------" << std::endl;
    *vmt_pointer = reinterpret_cast<std::uintptr_t>(orig_VirtualFn1);
    vClass->VirtualFn1(5, 5);
    delete vClass;
    return 0;
}
