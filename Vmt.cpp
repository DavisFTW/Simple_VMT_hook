#include <iostream>
#include "windows.h"

#define LOG(x) { std::cout << x << std::endl; }

class VirtualClass
{
public:

    int number;

    virtual void VirtualFn1() //This is the virtual function that will be hooked.
    {
        std::cout << "VirtualFn1 called " << number++ << "\n\n";
    }

    virtual void getBase() { std::cout << (this) << std::endl; }
};




using VirtualFn1_t = void(__thiscall*)(void* thisptr);
using funcType = void(__fastcall)(void*, int);
VirtualFn1_t orig_VirtualFn1;


std::uintptr_t __fastcall hkVirtualFn1(std::uintptr_t* thisptr, int edx) // hook function
{
    LOG("Hook function called");

    orig_VirtualFn1(thisptr); //Call the original function.

    return 0;
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


    VirtualProtect(vmt_pointer, 4, PAGE_EXECUTE_READWRITE, &oldProtection);  

    orig_VirtualFn1 = reinterpret_cast<VirtualFn1_t>(*vmt_pointer);   // store our origal function pointer 
    *vmt_pointer = reinterpret_cast<std::uintptr_t>(&hkVirtualFn1);   // redirect our pointers

    VirtualProtect(vmt_pointer, 4, oldProtection, 0);    // set the old protection back
    vClass->VirtualFn1();
    vClass->VirtualFn1();
    delete vClass;
    return 0;
}

