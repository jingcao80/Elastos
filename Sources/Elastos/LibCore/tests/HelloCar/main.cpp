//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using namespace Elastos;

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
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
    {    AutoLock syncLock(lock->mLock);
        Int32 i = 0;
        ++i;
    }
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

ECode ReflectionTestElastosCore()
{
    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IConstructorInfo> constructorInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;
    AutoPtr<IArgumentList> argumentList;

    const String moduleName("/system/lib/Elastos.Droid.Core.eco");
    const String klassName("LElastos/Droid/Widget/CLinearLayout;");
    const String constructorSignature(
    "CtxAttrs(LElastos/Droid/Content/IContext;*LElastos/Droid/Utility/IAttributeSet;*LIInterface;**)E");

    ECode ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        PFL_EX("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    PFL_EX("moduleInfo: %p", moduleInfo.Get())
    ec = moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        PFL_EX("Acquire \"%s\" class info failed!\n", klassName.string());
        return ec;
    }
    PFL_EX("classInfo: %p", classInfo.Get())

    String ns;
    ec = classInfo->GetNamespace(&ns);
    if (FAILED(ec)) {
        PFL_EX("GetNamespace failed!\n");
        return ec;
    }
    PFL_EX("namespace: %s", ns.string())

    Int32 ctorCount;
    ec = classInfo->GetConstructorCount(&ctorCount);
    if (FAILED(ec)) {
        PFL_EX("GetConstructorCount failed! ec=%08x\n", ec);
        return ec;
    }
    PFL_EX("classInfo: %d constructor.", ctorCount);

    AutoPtr< ArrayOf<IConstructorInfo *> > allInfos = ArrayOf<IConstructorInfo *>::Alloc(ctorCount);
    ec = classInfo->GetAllConstructorInfos(allInfos.Get());
    if (FAILED(ec)) {
        PFL_EX("GetAllConstructorInfos failed!\n");
        return ec;
    }

    for (Int32 i = 0; i < allInfos->GetLength(); ++i) {
        IConstructorInfo* ci = (*allInfos)[i];
        String name;
        ci->GetName(&name);
        Int32 paramCount;
        ci->GetParamCount(&paramCount);
        PFL_EX(" constructor: %s, paramCount: %d", name.string(), paramCount);
    }

    ec = classInfo->GetConstructorInfoByParamNames(constructorSignature, (IConstructorInfo**)&constructorInfo);
    if (FAILED(ec)) {
        PFL_EX("GetConstructorInfoByParamNames failed!\n");
        return ec;
    }

    Int32 paramCount;
    ec = constructorInfo->GetParamCount(&paramCount);
    if (FAILED(ec)) {
        PFL_EX("GetParamCount failed!\n");
        return ec;
    }
    PFL_EX("GetParamCount %d\n", paramCount);

    ec = constructorInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        PFL_EX("CreateArgumentList failed!\n");
        return ec;
    }

    ec = argumentList->SetInputArgumentOfObjectPtr(0, NULL);
    if (FAILED(ec)) {
        PFL_EX("SetInputArgumentOfObjectPtr failed!\n");
        return ec;
    }

    ec = argumentList->SetInputArgumentOfObjectPtr(1, NULL);
    if (FAILED(ec)) {
        PFL_EX("SetInputArgumentOfObjectPtr failed!\n");
        return ec;
    }

    AutoPtr<IInterface> obj;
    constructorInfo->CreateObject(argumentList, (IInterface**)&obj);
    if (FAILED(ec)) {
        PFL_EX("CreateObject failed!\n");
        return ec;
    }
    return NOERROR;
}

ECode ReflectionTests()
{
    // return ReflectionTestElastosCore();

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IConstructorInfo> constructorInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;
    AutoPtr<IArgumentList> argumentList;

    const String moduleName("/system/lib/Elastos.HelloCar.eco");
    const String klassName("LElastos/HelloCar/CCat;");

    ECode ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        PFL_EX("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    PFL_EX("moduleInfo: %p", moduleInfo.Get())
    ec = moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        PFL_EX("Acquire \"%s\" class info failed!\n", klassName.string());
        return ec;
    }
    PFL_EX("classInfo: %p", classInfo.Get())

    String ns;
    ec = classInfo->GetNamespace(&ns);
    if (FAILED(ec)) {
        PFL_EX("GetNamespace failed!\n");
        return ec;
    }
    PFL_EX("namespace: %s", ns.string())

    Int32 ctorCount;
    ec = classInfo->GetConstructorCount(&ctorCount);
    if (FAILED(ec)) {
        PFL_EX("GetConstructorCount failed! ec=%08x\n", ec);
        return ec;
    }
    PFL_EX("classInfo: %d constructor.", ctorCount);

    AutoPtr< ArrayOf<IConstructorInfo *> > allInfos = ArrayOf<IConstructorInfo *>::Alloc(ctorCount);
    ec = classInfo->GetAllConstructorInfos(allInfos.Get());
    if (FAILED(ec)) {
        PFL_EX("GetAllConstructorInfos failed!\n");
        return ec;
    }

    for (Int32 i = 0; i < allInfos->GetLength(); ++i) {
        IConstructorInfo* ci = (*allInfos)[i];
        String name;
        ci->GetName(&name);
        Int32 paramCount;
        ci->GetParamCount(&paramCount);
        PFL_EX(" constructor: %s, paramCount: %d", name.string(), paramCount);
    }

    ec = classInfo->GetConstructorInfoByParamNames(String("CtxAttrs(I32LElastos/String;LIInterface;**)E"), (IConstructorInfo**)&constructorInfo);
    if (FAILED(ec)) {
        PFL_EX("GetConstructorInfoByParamNames failed!\n");
        return ec;
    }

    Int32 paramCount;
    ec = constructorInfo->GetParamCount(&paramCount);
    if (FAILED(ec)) {
        PFL_EX("GetParamCount failed!\n");
        return ec;
    }
    PFL_EX("GetParamCount %d\n", paramCount);

    ec = constructorInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        PFL_EX("CreateArgumentList failed!\n");
        return ec;
    }

    ec = argumentList->SetInputArgumentOfInt32(0, 5);
    if (FAILED(ec)) {
        PFL_EX("SetInputArgumentOfInt32 failed!\n");
        return ec;
    }

    String str("Kitty");
    ec = argumentList->SetInputArgumentOfString(1, str);
    if (FAILED(ec)) {
        PFL_EX("SetInputArgumentOfString failed!\n");
        return ec;
    }

    AutoPtr<IInterface> obj;
    constructorInfo->CreateObject(argumentList, (IInterface**)&obj);
    if (FAILED(ec)) {
        PFL_EX("CreateObject failed!\n");
        return ec;
    }

    IAnimal* animal = IAnimal::Probe(obj);
    String name;
    animal->GetName(&name);
    PFL_EX(" >>>> animal name is %s", name.string());
    return NOERROR;
}

void otherTests()
{
    //testThread
    //testEtl()
    //testCoreExports();
    //testArrays();
    //lockTest();
    //loadFrameworkModule();
    ReflectionTests();
}

void testHelloCar()
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

void testTimeConsuming()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    Int64 begin, end;
    system->GetNanoTime(&begin);
    system->GetNanoTime(&end);
    Int64 timeConsumingOfGetNanoTime = end - begin;

    AutoPtr<ICharSequence> csq;
    system->GetNanoTime(&begin);
    CString::New(String("Hello World!"), (ICharSequence**)&csq);
    system->GetNanoTime(&end);
    Int64 timeConsumingOfFirstNewCString = end - begin - timeConsumingOfGetNanoTime;

    csq = NULL;
    system->GetNanoTime(&begin);
    CString::New(String("Hello World!"), (ICharSequence**)&csq);
    system->GetNanoTime(&end);
    Int64 timeConsumingOfSecondNewCString = end - begin - timeConsumingOfGetNanoTime;

    system->GetNanoTime(&begin);
    csq->AddRef();
    system->GetNanoTime(&end);
    Int64 timeConsumingOfAddRef = end - begin - timeConsumingOfGetNanoTime;

    printf("timeConsumingOfGetNanoTime: %f, timeConsumingOfFirstNewCString: %f, timeConsumingOfSecondNewCString: %f,"
            "timeConsumingOfAddRef: %f\n",
            (double)timeConsumingOfGetNanoTime / 1e6, ((double)timeConsumingOfFirstNewCString) / 1e6,
            ((double)timeConsumingOfSecondNewCString) / 1e6,
            ((double)timeConsumingOfAddRef) / 1e6);
}

int main(int argc, char *argv[])
{
    printf("==================================\n");
    printf("=========== Hello Car ============\n");
    printf("==================================\n");

    // testHelloCar();

    // other tests
    // otherTests();

    testTimeConsuming();

    printf("=========== Exit Hello Car ============\n");
    return 0;
}
