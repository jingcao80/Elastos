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

#include <elastos/droid/ext/frameworkdef.h>
#include <_Eunit.h>
#include "elastos/droid/test/suitebuilder/CTestMethod.h"

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

CAR_INTERFACE_IMPL(CTestMethod, Object, ITestMethod)

CAR_OBJECT_IMPL(CTestMethod)

ECode CTestMethod::constructor(
    /* [in] */ IMethodInfo* method,
    /* [in] */ IClassInfo* enclosingClass)
{
    String methodName;
    method->GetName(&methodName);
    return constructor(methodName, enclosingClass);
}

ECode CTestMethod::constructor(
    /* [in] */ const String& methodName,
    /* [in] */ IClassInfo* enclosingClass)
{
    mEnclosingClass = enclosingClass;
    String className;
    enclosingClass->GetName(&className);
    mEnclosingClassname = className;
    mTestMethodName = methodName;
    return NOERROR;
}

ECode CTestMethod::constructor(
    /* [in] */ ITestCase* testCase)
{
    String name;
    testCase->GetName(&name);
    AutoPtr<IClassInfo> enclosingClass;
    _CObject_ReflectClassInfo(testCase, (IClassInfo**)&enclosingClass);
    return constructor(name, enclosingClass);
}

ECode CTestMethod::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mTestMethodName;
    return NOERROR;
}

ECode CTestMethod::GetEnclosingClassname(
    /* [out] */ String* classname)
{
    VALIDATE_NOT_NULL(classname);
    *classname = mEnclosingClassname;
    return NOERROR;
}

ECode CTestMethod::GetEnclosingClass(
    /* [out] */ IClassInfo** clazz)
{
    VALIDATE_NOT_NULL(clazz);
    *clazz = mEnclosingClass;
    REFCOUNT_ADD(*clazz);
    return NOERROR;
}

ECode CTestMethod::CreateTest(
    /* [out] */ ITestCase** test)
{
    VALIDATE_NOT_NULL(test);
    return InstantiateTest(mEnclosingClass, mTestMethodName, test);
}

ECode CTestMethod::InstantiateTest(
    /* [in] */ IClassInfo* testCaseClass,
    /* [in] */ const String& testName,
    /* [out] */ ITestCase** test)
{
    Int32 N;
    testCaseClass->GetConstructorCount(&N);
    if (N == 0) {
        // return instantiateTest(testCaseClass.getSuperclass(), testName);
        assert(0);
        return NOERROR;
    }
    else {
        AutoPtr< ArrayOf<IConstructorInfo*> > constructors = ArrayOf<IConstructorInfo*>::Alloc(N);
        testCaseClass->GetAllConstructorInfos(constructors);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IConstructorInfo> constructor = (*constructors)[i];
            Int32 count;
            constructor->GetParamCount(&count);
            AutoPtr< ArrayOf<IParamInfo*> > params = ArrayOf<IParamInfo*>::Alloc(count);
            constructor->GetAllParamInfos(params);
            if (NoargsConstructor(params)) {
                AutoPtr<IInterface> obj;
                FAIL_RETURN(constructor->CreateObject(NULL, (IInterface**)&obj));
                AutoPtr<ITestCase> testCase =  ITestCase::Probe(obj);
                assert(testCase != NULL);
                testCase->SetName(testName);
                *test = testCase;
                REFCOUNT_ADD(*test);
                return NOERROR;
            }
            else if (SingleStringConstructor(params)) {
                AutoPtr<IArgumentList> args;
                constructor->CreateArgumentList((IArgumentList**)&args);
                args->SetInputArgumentOfString(0, testName);
                AutoPtr<IInterface> obj;
                FAIL_RETURN(constructor->CreateObject(args, (IInterface**)&obj));
                AutoPtr<ITestCase> testCase =  ITestCase::Probe(obj);
                assert(testCase != NULL);
                *test = testCase;
                REFCOUNT_ADD(*test);
                return NOERROR;
            }
        }
    }
    // throw new RuntimeException("Unable to locate a constructor for "
    //         + testCaseClass.getName());
    *test = NULL;
    return E_RUNTIME_EXCEPTION;
}

Boolean CTestMethod::SingleStringConstructor(
    /* [in] */ ArrayOf<IParamInfo*>* params)
{
    if (params->GetLength() != 1) return FALSE;
    AutoPtr<IParamInfo> param = (*params)[0];
    AutoPtr<IDataTypeInfo> typeInfo;
    param->GetTypeInfo((IDataTypeInfo**)&typeInfo);
    CarDataType type;
    typeInfo->GetDataType(&type);
    if (type == CarDataType_String) return TRUE;
    else return FALSE;
}

Boolean CTestMethod::NoargsConstructor(
    /* [in] */ ArrayOf<IParamInfo*>* params)
{
    return (params->GetLength() == 0);
}

ECode CTestMethod::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (TO_IINTERFACE(this) == obj->Probe(EIID_IInterface)) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    ClassID clsid;
    IObject::Probe(obj)->GetClassID(&clsid);
    if (clsid != ECLSID_CTestMethod) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ITestMethod> that = ITestMethod::Probe(obj);

    String thatEnclosingClassname;
    that->GetEnclosingClassname(&thatEnclosingClassname);
    if (!mEnclosingClassname.IsNull()
            ? !mEnclosingClassname.Equals(thatEnclosingClassname)
            : !thatEnclosingClassname.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }
    String thatTestMethodName;
    that->GetName(&thatTestMethodName);
    if (!mTestMethodName.IsNull()
            ? !mTestMethodName.Equals(thatTestMethodName)
            : !thatTestMethodName.IsNull()) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CTestMethod::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 result;
    result = (!mEnclosingClassname.IsNull() ? mEnclosingClassname.GetHashCode() : 0);
    result = 31 * result + (!mTestMethodName.IsNull() ? mTestMethodName.GetHashCode() : 0);
    *hashCode = result;
    return NOERROR;
}

ECode CTestMethod::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = mEnclosingClassname + "." + mTestMethodName;
    return NOERROR;
}

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos
