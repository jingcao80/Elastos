
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/HashSet.h>
#include <stdio.h>

using namespace Elastos;

struct Eqstr
{
    Boolean operator()(const char* s1, const char* s2) const
    {
        return strcmp(s1, s2) == 0;
    }
};

void Lookup(const HashSet<const char*, Hash<const char*>, Eqstr>& set,
            const char* word)
{
    HashSet<const char*, Hash<const char*>, Eqstr>::Iterator
    it = set.Find(word);
    printf("  %s: ", word);
    printf("%s\n", (it != set.End()? "present" : "not present"));
    
}

void testHashSet()
{
    HashSet<const char*, Hash<const char*>, Eqstr> set;
    set.Insert("kiwi");
    set.Insert("plum");
    set.Insert("apple");
    set.Insert("mango");
    set.Insert("apricot");
    set.Insert("banana");
    
    Lookup(set, "mango");
    Lookup(set, "apple");
    Lookup(set, "durian");
    
    HashSet<const char*, Hash<const char*>, Eqstr>::Iterator
    it1 = set.Begin();
    HashSet<const char*, Hash<const char*>, Eqstr>::Iterator
    it2 = set.End();
    
    for(; it1 != it2; ++it1) {
        printf("%s ", *it1);
    }
    printf("\n");
    
}

int main(int argc, char *argv[])
{
    testHashSet();
    
    return 0;
}
