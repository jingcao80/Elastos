
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Queue.h>
#include <elastos/Algorithm.h>
#include <stdio.h>

using namespace Elastos;

void testPQueue()
{
    Int32 ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
    PriorityQueue<Int32> ipq(ia, ia + 9);
    printf("size: %d\n", ipq.GetSize());

    for (Int32 i = 0; i < ipq.GetSize(); ++i) {
        printf("%d ", ipq.GetTop());
    }
    printf("\n");

    while(!ipq.IsEmpty()) {
        printf("%d ", ipq.GetTop());
        ipq.Pop();
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    testPQueue();

    return 0;
}
