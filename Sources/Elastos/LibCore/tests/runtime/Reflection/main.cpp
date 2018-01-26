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

#include <elautoptr.h>
#include <_Reflection.h>

using namespace Elastos;
using namespace Reflection;

void testGetSet()
{
    PEL("======== testGetSet() ======")

    AutoPtr<IFooBar> test;
    CFooBar::New((IFooBar**)&test);
    assert(test != NULL);

    test->SetValue(10);
    test->SetValue(String("Elastos"));

    Int32 value0;
    test->GetValue(&value0);
    String strValue;
    test->GetValue(&strValue);

    PFL_EX(" >> value0:%d, String:[%s]", value0, strValue.string());
}

ECode testReflection()
{
    printf("\n");
    PEL("======== testReflection() ======")

    AutoPtr<IModuleInfo> moduleInfo;
    // AutoPtr<IEnumInfo> enumInfo;
    // AutoPtr<IEnumItemInfo> emumItemInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;
    AutoPtr<IArgumentList> argumentList;

    const String moduleName("/system/lib64/Reflection.eco");
    const String klassName("Reflection.CFooBar");

    ECode ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    PFL_EX(" > moduleInfo: %p", moduleInfo.Get())
    ec = moduleInfo->GetClassInfo(klassName, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        printf("Acquire \"%s\" class info failed!, ec: 0x%08x\n", klassName.string(), ec);
        return ec;
    }

    String ns;
    classInfo->GetNamespace(&ns);
    PFL_EX(" > classInfo: %p, namespace: %s", classInfo.Get(), ns.string())

    // show class info
    //
    Int32 methodCount;
    classInfo->GetMethodCount(&methodCount);
    PFL_EX("    > methodCount: %d", methodCount)

    Int32 paramCount;
    if (methodCount > 0) {
        AutoPtr<ArrayOf<IMethodInfo *> > methods = ArrayOf<IMethodInfo *>::Alloc(methodCount);
        classInfo->GetAllMethodInfos(methods.Get());
        for (Int32 i = 0; i < methods->GetLength(); ++i) {
            IMethodInfo* method = (*methods)[i];
            String name;
            method->GetName(&name);
            method->GetParamCount(&paramCount);
            PFL_EX("    > method %d: %p, %s, paramCount %d", i, method, name.string(), paramCount)
        }
    }
    else {
        PFL_EX("    > no method found.");
    }


    // Create object
    //
    PFL_EX("Create object")
    ec = classInfo->CreateObject((IInterface**)&object);
    if (FAILED(ec)) {
        printf("Create object failed!\n");
        return ec;
    }

    AutoPtr<IInterfaceInfo> itfInfo;
    ec = classInfo->GetInterfaceInfo(String("Reflection.IFooBar"), (IInterfaceInfo**)&itfInfo);
    if (FAILED(ec)) {
        printf("Acquire \"IFooBar\" interface info failed!\n");
        return ec;
    }
    itfInfo->GetNamespace(&ns);
    PFL_EX(" > interfaceInfo: %p, namespace: %s", itfInfo.Get(), ns.string());

    // SetValue
    //
    PFL_EX("SetValue String")
    ec = classInfo->GetMethodInfo(String("SetValue"), String("(LElastos/String;)E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"SetValue\" method info failed!\n");
        return ec;
    }

    PFL_EX(" > methodInfo: %p", methodInfo.Get())
    assert(methodInfo != NULL);

    methodInfo->GetParamCount(&paramCount);
    PFL_EX(" > methodInfo: paramCount %d", paramCount)

    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Create \"SetValue\" argument list failed!\n");
        return ec;
    }

    PFL_EX(" > argumentList: %p", argumentList.Get())
    assert(argumentList != NULL);
    String strValue("Elastos");
    argumentList->SetInputArgumentOfString(0, strValue);

    PFL_EX(" > Invoke with argumentList: %p", argumentList.Get())
    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"SetValue\" method failed!\n");
        return ec;
    }

    // GetValue
    //
    PFL_EX("GetValue String")
    methodInfo = NULL;
    ec = classInfo->GetMethodInfo(String("GetValue"), String("(LElastos/String;*)E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    PFL_EX(" > methodInfo: %p", methodInfo.Get())
    assert(methodInfo != NULL);
    argumentList = NULL;
    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    PFL_EX(" > argumentList: %p", argumentList.Get())
    assert(argumentList != NULL);
    String result;
    argumentList->SetOutputArgumentOfStringPtr(0, &result);

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"GetValue\" method failed!\n");
        return ec;
    }

    printf(" >>> GetValue: [%s]\n", result.string());

    // Validate
    //
    IFooBar* foobar = IFooBar::Probe(object.Get());
    assert(foobar != NULL);
    String respect;
    foobar->GetValue(&respect);

    assert(respect == result);

    // SetValue
    //
    PFL_EX("SetValue Int32, String, Int64")
    methodInfo = NULL;
    ec = classInfo->GetMethodInfo(String("SetValue"), String("(I32LElastos/String;I64)E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"SetValue\" method info failed!\n");
        return ec;
    }

    assert(methodInfo != NULL);
    argumentList = NULL;
    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Acquire \"SetValue\" method info failed!\n");
        return ec;
    }

    assert(argumentList != NULL);
    strValue = String("-Elastos-");
    argumentList->SetInputArgumentOfInt32(0, 11);
    argumentList->SetInputArgumentOfString(1, strValue);
    argumentList->SetInputArgumentOfInt64(2, 22);

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"SetValue\" method failed!\n");
        return ec;
    }

    // GetValue
    //
    PFL_EX("GetValue Int32, String, Int64")
    methodInfo = NULL;
    ec = classInfo->GetMethodInfo(String("GetValue"), String("(I32*LElastos/String;*I64*)E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    assert(methodInfo != NULL);
    argumentList = NULL;
    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        printf("Acquire \"GetValue\" method info failed!\n");
        return ec;
    }

    String strResult;
    Int32 value0;
    Int64 value1;

    assert(argumentList != NULL);
    argumentList->SetOutputArgumentOfInt32Ptr(0, &value0);
    argumentList->SetOutputArgumentOfStringPtr(1, &strResult);
    argumentList->SetOutputArgumentOfInt64Ptr(2, &value1);

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        printf("Invoke \"GetValue\" method failed!\n");
        return ec;
    }

    printf(" >>> GetValue: [%s], value0: %d, value1: %lld\n",
        strResult.string(), value0, value1);
    return NOERROR;
}

int main(int argc, char *argv[])
{
    testGetSet();

    testReflection();
    return 0;
}
