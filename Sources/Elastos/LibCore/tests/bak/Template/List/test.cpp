
#define __USE_MALLOC

#include <elastos.h>
#include <elastos/Algorithm.h>
#include <elastos/List.h>
#include <stdio.h>

using namespace Elastos;

template <typename F, typename R>
class IntentResolver
{
public:
    IntentResolver();

    ~IntentResolver();

private:


    CARAPI_(Boolean) RemoveAllObjects(
        /* [in] */ List<F>* list,
        /* [in] */ F* f)
    {
        if (list != NULL) {
            typename List<F>::Iterator it1 = list->Begin();
            typename List<F>::Iterator it2 = list->End();
            for(;it1 != it2; ++it1) {
                if (*it1 == f) {
                    list->Erase(it1);

                }
            }
            return list->IsEmpty() == FALSE;
        }
        return false;
    }


private:

};




void testList()
{
    List<Int32> ilist;
    printf("size=%d\n", ilist.GetSize());

    ilist.PushBack(0);
    ilist.PushBack(1);
    ilist.PushBack(2);
    ilist.PushBack(3);
    ilist.PushBack(4);
    printf("size=%d\n", ilist.GetSize());

    List<Int32>::Iterator it;
    for (it = ilist.Begin(); it != ilist.End(); ++it) {
        printf("%d ", *it);
    }
    printf("\n");

    it = Find(ilist.Begin(), ilist.End(), 3);
    if (it != 0) ilist.Insert(it, 99);

    printf("size=%d, value=%d\n", ilist.GetSize(), *it);

    for (it = ilist.Begin(); it != ilist.End(); ++it) {
        printf("%d ", *it);
    }
    printf("\n");

    it = Find(ilist.Begin(), ilist.End(), 1);
    if (it != 0) printf("%d\n", *(ilist.Erase(it)));

    for (it = ilist.Begin(); it != ilist.End(); ++it) {
        printf("%d ", *it);
    }
    printf("\n");

}

int main(int argc, char *argv[])
{
    testList();

    return 0;
}
