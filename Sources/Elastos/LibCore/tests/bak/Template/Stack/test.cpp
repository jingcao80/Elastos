
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Stack.h>
#include <elastos/List.h>
#include <stdio.h>

using namespace Elastos;

void testStack1()
{
    Stack<Int32> istack;
    istack.Push(1);
    istack.Push(3);
    istack.Push(5);
    istack.Push(7);

    printf("size: %d, top: %d\n", istack.GetSize(), istack.GetTop());

    istack.Pop();
    printf("top: %d\n", istack.GetTop());
    istack.Pop();
    printf("top: %d\n", istack.GetTop());
    istack.Pop();
    printf("top: %d\n", istack.GetTop());
    printf("size: %d\n", istack.GetSize());
}

void testStack2()
{
    Stack<Int32, List<Int32> > istack;
    istack.Push(1);
    istack.Push(3);
    istack.Push(5);
    istack.Push(7);

    printf("size: %d, top: %d\n", istack.GetSize(), istack.GetTop());

    istack.Pop();
    printf("top: %d\n", istack.GetTop());
    istack.Pop();
    printf("top: %d\n", istack.GetTop());
    istack.Pop();
    printf("top: %d\n", istack.GetTop());
    printf("size: %d\n", istack.GetSize());
}

int main(int argc, char *argv[])
{
    testStack1();
    testStack2();

    return 0;
}