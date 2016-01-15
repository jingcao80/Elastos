
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Vector.h>
#include <elastos/Algorithm.h>
#include <stdio.h>

using namespace Elastos;

void testVector()
{
    printf("Int32()=%d\n", int());

    Vector<Int32> v(2, 9);
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());

    v.PushBack(1);
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());

    v.PushBack(2);
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());

    v.PushBack(3);
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());

    v.PushBack(4);
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());

    Int32 i;
    for(i = 0; i < v.GetSize(); ++i) {
        printf("%d ", v[i]);
    }
    printf("\n");


    v.PushBack(5);
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());
    for(i = 0; i < v.GetSize(); ++i) {
        printf("%d ", v[i]);
    }
    printf("\n");

    v.PopBack();

    v.PopBack();
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());

    v.PopBack();
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());

    Vector<Int32>::Iterator it = Find(v.Begin(), v.End(), 1);
    if (it != v.End()) v.Erase(it);

    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());
    for(i = 0; i < v.GetSize(); ++i) {
        printf("%d ", v[i]);
    }
    printf("\n");

    it = Find(v.Begin(), v.End(), 2);
    if (it != v.End()) v.Insert(it, 3, 7);
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());
    for(i = 0; i < v.GetSize(); ++i) {
        printf("%d ", v[i]);
    }
    printf("\n");

    v.Clear();
    printf("size=%d, capacity=%d\n", v.GetSize(), v.GetCapacity());
}

int main(int argc, char *argv[])
{
    testVector();

    return 0;
}
