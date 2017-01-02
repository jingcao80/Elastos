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

#include "eunit/framework/TestCase.h"
#include "eunit/framework/CTestFailure.h"
#include "eunit/framework/CTestResult.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Eunit {
namespace Framework {

CAR_INTERFACE_IMPL_2(TestCase, Object, ITest, ITestCase)

TestCase::TestCase()
{}

TestCase::TestCase(
    /* [in] */ const String& name)
    : mName(name)
{}

ECode TestCase::Initialize()
{
    return NOERROR;
}

ECode TestCase::CountTestCases(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 1;
    return NOERROR;
}

AutoPtr<ITestResult> TestCase::CreateResult()
{
    AutoPtr<ITestResult> result;
    CTestResult::New((ITestResult**)&result);
    return result;
}

ECode TestCase::Run(
    /* [out] */ ITestResult** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ITestResult> r = CreateResult();
    Run(result);
    *result = r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TestCase::Run(
    /* [in] */ ITestResult* result)
{
    return result->Run(this);
}

ECode TestCase::IsFailed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !mFailures.IsEmpty();
    return NOERROR;
}

ECode TestCase::AddFailure(
    /* [in] */ ECode ec,
    /* [in] */ const String& message,
    /* [in] */ const String& backtrace)
{
    AutoPtr<ITestFailure> failure;
    CTestFailure::New((ITest*)Probe(EIID_ITest), ec,
            message, backtrace, (ITestFailure**)&failure);
    mFailures.PushBack(failure);
    return NOERROR;
}

ECode TestCase::GetFailures(
    /* [out, callee] */ ArrayOf<ITestFailure*>** failures)
{
    VALIDATE_NOT_NULL(failures);
    AutoPtr< ArrayOf<ITestFailure*> > failureArray;
    if (!mFailures.IsEmpty()) {
        Int32 size = mFailures.GetSize();
        failureArray = ArrayOf<ITestFailure*>::Alloc(size);
        List< AutoPtr<ITestFailure> >::Iterator it;
        Int32 i;
        for (it = mFailures.Begin(), i = 0; it != mFailures.End(); ++it, ++i) {
            failureArray->Set(i, *it);
        }
    }
    *failures = failureArray;
    REFCOUNT_ADD(*failures);
    return NOERROR;
}

ECode TestCase::RunBare()
{
    ECode ec = NOERROR;
    SetUp();
    ec = RunTest();
    ECode ec1 = TearDown();
    if (ec == NOERROR) ec = ec1;
    return ec;
}

ECode TestCase::RunTest()
{
    AutoPtr<ICharSequence> cseq;
    CString::New(mName, (ICharSequence**)&cseq);
    FAIL_RETURN(AssertNotNull(String("TestCase.mName cannot be null"), cseq)); // Some VMs crash when calling getMethod(null,null);
    AutoPtr<IMethodInfo> runMethod;
    // try {
        // use getMethod to get all public inherited
        // methods. getDeclaredMethods returns all
        // methods of this class but excludes the
        // inherited ones.
    AutoPtr<IClassInfo> clazz;
    ECode ec = _CObject_ReflectClassInfo(Probe(EIID_ITest), (IClassInfo**)&clazz);
    if (SUCCEEDED(ec)) {
        ec = clazz->GetMethodInfo(mName, String("")/*TODO*/, (IMethodInfo**)&runMethod);
    }
    if (FAILED(ec)) return Fail(String("Method \"") + mName + String("\" not found"));
    // } catch (NoSuchMethodException e) {
    //     fail("Method \""+fName+"\" not found");
    // }
    // if (!Modifier.isPublic(runMethod.getModifiers())) {
    //     fail("Method \""+fName+"\" should be public");
    // }

    // try {
    ec = runMethod->Invoke(Probe(EIID_ITest), NULL);
    return ec;
    // }
    // catch (InvocationTargetException e) {
    //     e.fillInStackTrace();
    //     throw e.getTargetException();
    // }
    // catch (IllegalAccessException e) {
    //     e.fillInStackTrace();
    //     throw e;
    // }
}

ECode TestCase::SetUp()
{
    Assert::sCurrentTest = (ITest*)Probe(EIID_ITest);
    return NOERROR;
}

ECode TestCase::TearDown()
{
    Assert::sCurrentTest = NULL;
    return NOERROR;
}

ECode TestCase::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String name;
    GetName(&name);
    AutoPtr<IClassInfo> clazz;
    _CObject_ReflectClassInfo(Probe(EIID_ITest), (IClassInfo**)&clazz);
    String clazzName;
    clazz->GetName(&clazzName);
    *str = name + "(" + clazzName + ")";
    return NOERROR;
}

ECode TestCase::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode TestCase::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

} // namespace Framework
} // namespace Eunit
