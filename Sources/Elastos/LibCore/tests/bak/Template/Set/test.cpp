
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Algorithm.h>
#include <elastos/Set.h>
#include <stdio.h>

using namespace Elastos;

void testSet()
{
    Int32 i;
    Int32 ia[5] = { 0, 1, 2, 3, 4 };
    Set<Int32> iset(ia, ia + 5);
    
    printf("size = %d\n", iset.GetSize());
    printf("3 count = %d\n", iset.GetCount(3));
    
    iset.Insert(3);
    printf("size = %d\n", iset.GetSize());
    printf("3 count = %d\n", iset.GetCount(3));
    
    iset.Insert(5);
    printf("size = %d\n", iset.GetSize());
    printf("3 count = %d\n", iset.GetCount(3));
    
    iset.Erase(1);
    printf("size = %d\n", iset.GetSize());
    printf("3 count = %d\n", iset.GetCount(3));
    printf("1 count = %d\n", iset.GetCount(1));
    
    Set<Int32>::Iterator ite1 = iset.Begin();
    Set<Int32>::Iterator ite2 = iset.End();
    for (; ite1 != ite2; ++ite1) printf("%d ", *ite1);
    printf("\n");
    
    ite1 = Find(iset.Begin(), iset.End(), 3);
    if (ite1 != iset.End()) printf("3 found\n");
        
    ite1 = Find(iset.Begin(), iset.End(), 1);
    if (ite1 == iset.End()) printf("1 not found\n");
        
    ite1 = iset.Find(3);
    if (ite1 != iset.End()) printf("3 found\n");
        
    ite1 = iset.Find(1);
    if (ite1 == iset.End()) printf("1 not found\n");
        
//    *ite1 = 9;                   
}

int main(int argc, char *argv[])
{
    testSet();
    
    return 0;
}
