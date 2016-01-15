
#include <elastos/core/Object.h>
#include <stdio.h>

using namespace Elastos;
using namespace Elastos::Core;

class C1
    : public Object
{
public:
    C1()
    {
        printf(" == Create %p...\n", this);
    }

    ~C1()
    {
        printf(" == Delete %p...\n", this);
    }

    void Print(const char* msg = NULL)
    {
        if (NULL != msg) {
            printf(" print object: %s\n", msg);
        }
        else {
            printf(" print object\n");
        }
    }
};

DEFINE_CONVERSION_FOR(C1, IInterface)

void f1()
{
    printf("==== enter f1 ====\n");
    AutoPtr<C1> v1 = new C1();
    printf("==== return from f1 ====\n");
}

AutoPtr<C1> f2()
{
    printf("==== enter f2 ====\n");
    AutoPtr<C1> v1 = new C1();
    printf("==== return from f2 ====\n");
    return v1;
}

void f3(AutoPtr<C1> v1)
{
    printf("==== enter f3 ====\n");
    C1* v2 = v1;
    v2->Print();
    printf("==== return from f3 ====\n");
}

void f4(AutoPtr<C1> & v1)
{
    printf("==== enter f4 ====\n");
    C1* v2 = v1;
    v2->Print();
    printf("==== return from f4 ====\n");
}

void testAutoPtr()
{
    printf("==== call f1 ====\n");
    {
        f1();
    }
    printf("==== call f1 end ====\n");

    printf("\n==== call f2 ====\n");
    {
        AutoPtr<C1> v = f2();
        v->Print();
    }

    printf("==== call f2 end ====\n");

    printf("\n==== call f2 again ====\n");
    {
        // It's dangerous!
        C1* v = f2();
        v->Print("This is dangerous!");
    }
    printf("==== call f2 end ====\n");

    printf("\n==== call f3 ====\n");
    {
        f3(f2());
    }
    printf("==== call f3 end ====\n");

    printf("\n==== call f4 ====\n");
    {
        AutoPtr<C1> v = f2();
        f4(v);
    }
    printf("==== call f4 end ====\n");
}

void testArrayOf()
{
    {
        printf("==== ArrayOf<C1> ====\n");
        ArrayOf<C1*>* arr = ArrayOf<C1*>::Alloc(2);
        for (Int32 i = 0; i < arr->GetLength(); ++i) {
            arr->Set(i, new C1());
        }

        arr->Release();
    }

    {
        printf("\n==== AutoPtr<ArrayOf<C1 *> > ====\n");
        AutoPtr<ArrayOf<C1 *> > arr = ArrayOf<C1 *>::Alloc(2);
        for (Int32 i = 0; i < arr->GetLength(); ++i) {
            arr->Set(i, new C1());
        }
    }
}

int main(int argc, char *argv[])
{
    printf("==== call testAutoPtr ====\n");
    testAutoPtr();
    printf("==== call testAutoPtr end ====\n");

    printf("==== call testArrayOf ====\n");
    testArrayOf();
    printf("==== call testArrayOf end ====\n");

    return 0;
}
