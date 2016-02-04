/*
 * Usages:
 * compile:
 *      cd HelloCar
 *      cd eco
 *      emake
 *      cd ..
 *      emake
 *      pd @
 *      elcopy Elastos.HelloCar.eco /system/lib
 *      elcopy helloCar
 *
 * run:
        adb root
 *      adb shell
 *      cd /data/data/com.elastos.runtime/elastos
 *      ./helloCar
 *
 * output:
 *      Though cat has nine lives, but it can not fly!
 *      Kitty can not fly!
 *
 *      HuaHua : wang! wang!
 *      HuaHua can not fly!
 *
 *      Though cat has nine lives, but it can not fly!
 *      CAnimalHelper::CanFly : Kitty can not fly!
 */

#include <elautoptr.h>
#include <_Elastos.HelloCar.h>
#include <elastos/coredef.h>            // include ETL 头文件之前必须先 include 该头文件以便ETL使用其中定义的宏
#include <elastos/core/Math.h>
 #include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/Arrays.h>

#include <dlfcn.h>

using namespace Elastos;

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IThread;
using Elastos::Core::Math;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Arrays;

using Elastos::HelloCar::IAnimal;
using Elastos::HelloCar::IDog;
using Elastos::HelloCar::CDog;
using Elastos::HelloCar::CCat;
using Elastos::HelloCar::IAnimalHelper;
using Elastos::HelloCar::CAnimalHelper;
using Elastos::HelloCar::CMyThread;

void testThread()
{
    AutoPtr<IThread> thread;
    CMyThread::New((IThread**)&thread);
    assert(thread.Get() != NULL && "Thread is null!");
    thread->Start();
}

void testEtl()
{
    List<String> animals;
    animals.PushBack(String("cat"));
    animals.PushBack(String("dog"));
}

void testCoreExports()
{
    Double value = Elastos::Core::Math::Cos(90);
    printf("Math::Cos(90) %.2f\n", value);

    String str = StringUtils::ToString((Int32)100, 10);
    printf("100 StringUtils::ToString : %s\n", str.string());

    StringBuilder sb(20);
    sb.AppendChar('A');
    sb.Append(" = ");
    sb.Append(str);
    printf("StringBuilder.ToString() %s\n", sb.ToString().string());
}

void assertEquals(Int32 testValue, Int32 expectation)
{
    if (testValue != expectation) {
        printf(" >> error: got %d, but expect %d\n", testValue, expectation);
        assert(testValue == expectation);
    }
}

void testArrays()
{
    AutoPtr<ArrayOf<Int32> > int32Arr = ArrayOf<Int32>::Alloc(100);
    for (Int32 i = 0; i < int32Arr->GetLength(); ++i) {
        int32Arr->Set(i, i);
    }

    Int32 index;
    Arrays::BinarySearch(int32Arr.Get(), 0, 50, 25, &index);
    assertEquals(index, 25);

    Arrays::BinarySearch(int32Arr.Get(), 0, 50, 60, &index);
    printf("Arrays::BinarySearch(int32Arr, 0, 50, 60); result: %d\n", index);

    AutoPtr<ArrayOf<Char32> > charArr = ArrayOf<Char32>::Alloc(20);
    for (Int32 i = 0; i < charArr->GetLength(); ++i) {
        charArr->Set(i, (Char32)('a' + i));
    }
    Arrays::BinarySearch(charArr.Get(), 0, charArr->GetLength(), (Char32)('a' + 15), &index);
    assertEquals(index, 15);

    AutoPtr<ArrayOf<IInterface*> > animalArr = ArrayOf<IInterface*>::Alloc(10);
    for (Int32 i = 0; i < animalArr->GetLength(); ++i) {
        StringBuilder sb("Dog_");
        sb.Append(i);
        AutoPtr<IAnimal> animal;
        CDog::New(i, sb.ToString(), (IAnimal**)&animal);

        animalArr->Set(i, TO_IINTERFACE(animal));
    }

    AutoPtr<IAnimal> dog;
    CDog::New(4, String("Dog_4"), (IAnimal**)&dog);
    Arrays::BinarySearch(animalArr.Get(), 0, animalArr->GetLength(), TO_IINTERFACE(dog), &index);
    printf("Arrays::BinarySearch(animalArr, Dog_4); result: %d\n", index);
}

class AutoLockTest
    : public Object
{
public:
    Object mLock;
};

void lockTest()
{
    AutoPtr<AutoLockTest> lock = new AutoLockTest();
    synchronized(lock->mLock) {
        Int32 i = 0;
        ++i;
    }
}

void otherTests()
{
    //testThread
    //testEtl()
    //testCoreExports();
    //testArrays();
    //lockTest();
}

void loadFrameworkModule()
{
    void* module = dlopen("Elastos.Droid.Core.eco", RTLD_NOW);
    if (NULL == module) {
        printf("<%s, %d> dlopen '%s' failed.\n", __FILE__, __LINE__, "Elastos.Droid.Core.eco");
        printf("error: %s\n", dlerror());
    }

    module = dlopen("Elastos.Droid.Server.eco", RTLD_NOW);
    if (NULL == module) {
        printf("<%s, %d> dlopen '%s' failed.\n", __FILE__, __LINE__, "Elastos.Droid.Server.eco");
        printf("error: %s\n", dlerror());
    }
}

int main(int argc, char *argv[])
{
    printf("==================================\n");
    printf("=========== Hello Car ============\n");
    printf("==================================\n");

    {
        Boolean canFly;
        String name;

        AutoPtr<IAnimal> cat;
        CCat::New((IAnimal**)&cat);
        cat->SetName(String("Kitty"));
        cat->GetName(&name);
        cat->CanFly(&canFly);
        printf("%s %s!\n\n", name.string(), canFly ? "can fly" : "can not fly");

        AutoPtr<IDog> dog;
        CDog::New(2, String("HuaHua"), (IDog**)&dog);
        dog->Bark();

        IAnimal* da = IAnimal::Probe(dog);
        da->GetName(&name);
        da->CanFly(&canFly);
        printf("%s %s!\n\n", name.string(), canFly ? "can fly" : "can not fly");

        AutoPtr<IAnimalHelper> helper;
        CAnimalHelper::AcquireSingleton((IAnimalHelper**)&helper);
        helper->CanFly(cat, &canFly);
        cat->GetName(&name);
        printf("CAnimalHelper::CanFly : %s %s!\n\n", name.string(), canFly ? "can fly" : "can not fly");
    }

    loadFrameworkModule();

    // other tests
    otherTests();

    printf("=========== Exit Hello Car ============\n");
    return 0;
}
