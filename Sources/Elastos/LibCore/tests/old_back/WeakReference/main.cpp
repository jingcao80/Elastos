
#include <WeakReference.h>
#include <elautoptr.h>
#include "ElSWRefBase.h"
#include <Elastos.CoreLibrary.h>

using namespace Elastos;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

static Int64 GetCurrentTimeMillis()
{
    AutoPtr<Elastos::Core::ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((Elastos::Core::ISystem**)&system);
    Int64 curTime;
    system->GetCurrentTimeMillis(&curTime);
    return curTime;
}

class R1 : public ElLightRefBase
{};

class R2 : public ElRefBase
{};

class R3 : public ElSWRefBase
{};

void test1()
{
    AutoPtr<IFoo> foo;
    CFooBar::New((IFoo**)&foo);
    foo->Foo();

    AutoPtr<IWeakReference> weakRef;
    IWeakReferenceSource::Probe(foo)->GetWeakReference((IWeakReference**)&weakRef);

    AutoPtr<IFooBar> foobar;
    weakRef->Resolve(EIID_IFooBar, (IInterface**)&foobar);
    if (foobar != NULL) {
        foobar->FooBar();
    }
    else {
        printf("foobar is NULL!\n");
    }
}

void test2()
{
    IFoo* foo;
    CFooBar::New(&foo);
    foo->Foo();

    AutoPtr<IWeakReference> weakRef;
    IWeakReferenceSource::Probe(foo)->GetWeakReference((IWeakReference**)&weakRef);
    foo->Release();

    AutoPtr<IFooBar> foobar;
    weakRef->Resolve(EIID_IFooBar, (IInterface**)&foobar);
    if (foobar != NULL) {
        foobar->FooBar();
    }
    else {
        printf("foobar is NULL!\n");
    }
}

void test3()
{
    Int32 N = 1000000;
    Int64 begin = 0;
    Int64 end = 0;

    R1* r1= new R1();
    begin = GetCurrentTimeMillis();
    for (Int32 i = 0; i < N; ++i) {
        r1->AddRef();
    }
    end = GetCurrentTimeMillis();
    printf("call r1->AddRef() %d times uses %lld millis\n", N, end - begin);

    R2* r2= new R2();
    begin = GetCurrentTimeMillis();
    for (Int32 i = 0; i < N; ++i) {
        r2->AddRef();
    }
    end = GetCurrentTimeMillis();
    printf("call r2->AddRef() %d times uses %lld millis\n", N, end - begin);

    R3* r3= new R3();
    begin = GetCurrentTimeMillis();
    for (Int32 i = 0; i < N; ++i) {
        r3->AddRef();
    }
    end = GetCurrentTimeMillis();
    printf("call r3->AddRef() %d times uses %lld millis\n", N, end - begin);

    begin = GetCurrentTimeMillis();
    for (Int32 i = 0; i < N; ++i) {
        r1->Release();
    }
    end = GetCurrentTimeMillis();
    printf("call r1->Release() %d times uses %lld millis\n", N, end - begin);

    begin = GetCurrentTimeMillis();
    for (Int32 i = 0; i < N; ++i) {
        r2->Release();
    }
    end = GetCurrentTimeMillis();
    printf("call r2->Release() %d times uses %lld millis\n", N, end - begin);

    begin = GetCurrentTimeMillis();
    for (Int32 i = 0; i < N; ++i) {
        r3->Release();
    }
    end = GetCurrentTimeMillis();
    printf("call r3->Release() %d times uses %lld millis\n", N, end - begin);
}

int main(int argc, char *argv[])
{
    test1();
    test2();
    test3();
    return 0;
}
