
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Algorithm.h>
#include <elastos/Vector.h>
#include <stdio.h>

using namespace Elastos;

void testHeap()
{
    {
        Int32 ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
        Vector<Int32> ivec(ia, ia + 9);

        MakeHeap(ivec.Begin(), ivec.End());
        for (Int32 i = 0; i < ivec.GetSize(); ++i) {
            printf("%d ", ivec[i]);
        }
        printf("\n");

        ivec.PushBack(7);
        PushHeap(ivec.Begin(), ivec.End());
        for (Int32 i = 0; i < ivec.GetSize(); ++i) {
            printf("%d ", ivec[i]);
        }
        printf("\n");

        PopHeap(ivec.Begin(), ivec.End());
        printf("%d \n", ivec.GetBack());
        ivec.PopBack();

        for (Int32 i = 0; i < ivec.GetSize(); ++i) {
            printf("%d ", ivec[i]);
        }
        printf("\n");

        SortHeap(ivec.Begin(), ivec.End());
        for (Int32 i = 0; i < ivec.GetSize(); ++i) {
            printf("%d ", ivec[i]);
        }
        printf("\n");
    }

    {
        Int32 ia[9] = {0, 1, 2, 3, 4, 8, 9, 3, 5};
        MakeHeap(ia, ia + 9);

        SortHeap(ia, ia + 9);
        for (Int32 i = 0; i < 9; ++i) {
            printf("%d ", ia[i]);
        }
        printf("\n");

        MakeHeap(ia, ia + 9);
        PopHeap(ia, ia + 9);
        printf("%d\n", ia[8]);
    }

    {
        Int32 ia[6] = {4, 1, 7, 6, 2, 5};
        MakeHeap(ia, ia + 6);
        for (Int32 i = 0; i < 6; ++i) {
            printf("%d ", ia[i]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    testHeap();

    return 0;
}