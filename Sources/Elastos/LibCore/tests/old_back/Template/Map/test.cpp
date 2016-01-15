
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Map.h>
#include <stdio.h>

using namespace Elastos;

void testMap()
{
    Map<String, Int32> simap;
    
    simap[String("jjhou")] = 1;
    simap[String("jerry")] = 2;
    simap[String("jason")] = 3;
    simap[String("jimmy")] = 4;
    
    Pair<String, Int32> value(String("david"), 5);
    simap.Insert(value);
    
    Map<String, Int32>::Iterator simap_it = simap.Begin();
    for (; simap_it != simap.End(); ++simap_it) {
        printf("%s, %d\n", simap_it->mFirst, simap_it->mSecond);
    }
    
    Int32 number = simap[String("jjhou")];
    printf("%d\n", number);
    
    Map<String, Int32>::Iterator it1;
    
    it1 = simap.Find(String("mchen"));
    if (it1 == simap.End()) printf("mchen not found\n");
    
    it1 = simap.Find(String("jerry"));
    if (it1 != simap.End()) printf("jerry found\n");
        
    it1->mSecond = 9;
    Int32 number2 = simap[String("jerry")];
    printf("%d\n", number2);
}

int main(int argc, char *argv[])
{
    testMap();
    
    return 0;
}
