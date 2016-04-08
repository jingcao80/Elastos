
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/HashTable.h>
#include <stdio.h>

using namespace Elastos;

void testHashTable()
{
    HashTable<Int32, 
              Int32, 
              Hash<Int32>,
              Identity<Int32>,
              EqualTo<Int32> > 
            iht(50, Hash<Int32>(), EqualTo<Int32>());
   
    printf("%d, %d, %lu\n", iht.GetSize(), iht.GetBucketCount(), iht.GetMaxBucketCount());
   
    iht.InsertUnique(59);
    iht.InsertUnique(63);
    iht.InsertUnique(108);
    iht.InsertUnique(2);
    iht.InsertUnique(53);
    iht.InsertUnique(55);
    printf("%d\n", iht.GetSize());
   
    HashTable<Int32, 
              Int32, 
              Hash<Int32>,
              Identity<Int32>,
              EqualTo<Int32> >::Iterator
        it = iht.Begin();
        
    for(Int32 i = 0; i < iht.GetSize(); ++i, ++it) {
        printf("%d ", *it);
    }  
    printf("\n");
    
    for(Int32 i = 0; i < iht.GetBucketCount(); ++i) {
        Int32 n = iht.GetElementCountInBucket(i);
        if (n != 0) printf("bucket[%d] has %d elems.\n", i, n);
    }
    
    for(Int32 i = 0; i <= 47; i++) iht.InsertEqual(i);
    printf("%d, %d\n", iht.GetSize(), iht.GetBucketCount());
    
    for(Int32 i = 0; i < iht.GetBucketCount(); ++i) {
        Int32 n = iht.GetElementCountInBucket(i);
        if (n != 0) printf("bucket[%d] has %d elems.\n", i, n);
    }
    
    it = iht.Begin();
    for(Int32 i = 0; i < iht.GetSize(); ++i, ++it) {
        printf("%d ", *it);
    }  
    printf("\n");
    
    printf("%d %d\n", *(iht.Find(2)), iht.GetCount(2));
}

int main(int argc, char *argv[])
{
    testHashTable();
    
    return 0;
}
