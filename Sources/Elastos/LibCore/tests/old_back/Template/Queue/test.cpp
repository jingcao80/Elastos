
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Queue.h>
#include <elastos/Algorithm.h>
#include <elastos/List.h>
#include <stdio.h>

using namespace Elastos;

void testQueue()
{
    Queue<Int32> q1;

    q1.Push(1);
    q1.Push(3);
    q1.Push(5);
    q1.Push(7);

    printf("q1 size: %d, front: %d \n", q1.GetSize(), q1.GetFront());

    q1.Pop(); printf("q1 front: %d \n", q1.GetFront());
    q1.Pop(); printf("q1 front: %d \n", q1.GetFront());
    q1.Pop(); printf("q1 front: %d \n", q1.GetFront());
    printf("q1 size: %d \n", q1.GetSize());

    Queue< Int32, List<Int32> > q2;

    q2.Push(1);
    q2.Push(3);
    q2.Push(5);
    q2.Push(7);

    printf("q2 size: %d, front: %d \n", q2.GetSize(), q2.GetFront());

    q2.Pop(); printf("q2 front: %d \n", q2.GetFront());
    q2.Pop(); printf("q2 front: %d \n", q2.GetFront());
    q2.Pop(); printf("q2 front: %d \n", q2.GetFront());
    printf("q2 size: %d \n", q2.GetSize());
}

int main(int argc, char *argv[])
{
    testQueue();

    return 0;
}