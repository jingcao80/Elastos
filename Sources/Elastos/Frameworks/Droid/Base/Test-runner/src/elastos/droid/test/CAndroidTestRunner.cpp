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

#include "elastos/droid/test/CAndroidTestRunner.h"
#include "elastos/droid/test/TestCaseUtil.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Eunit::Framework::ITestSuite;
using Eunit::Framework::CTestSuite;
using Eunit::Framework::CTestResult;
using Eunit::Framework::ITestCase;
using Eunit::Runner::EIID_IBaseTestRunner;

namespace Elastos {
namespace Droid {
namespace Test {

const String CAndroidTestRunner::TAG("CAndroidTestRunner");

CAndroidTestRunner::CAndroidTestRunner()
    : mSkipExecution(FALSE)
{}

CAR_INTERFACE_IMPL_2(CAndroidTestRunner, Object, IBaseTestRunner, IAndroidTestRunner)

CAR_OBJECT_IMPL(CAndroidTestRunner)

ECode CAndroidTestRunner::SetTestClassName(
    /* [in] */ const String& testPackageName,
    /* [in] */ const String& testClassName,
    /* [in] */ const String& testMethodName)
{
    AutoPtr<IClassInfo> testClass;
    FAIL_RETURN(LoadTestClass(testPackageName, testClassName, (IClassInfo**)&testClass));

    if (ShouldRunSingleTestMethod(testMethodName, testClass)) {
        AutoPtr<ITestCase> testCase = BuildSingleTestMethod(testClass, testMethodName);
        AutoPtr<IArrayList> list;
        CArrayList::New((IArrayList**)&list);
        Boolean result;
        list->Add(testCase, &result);
        mTestCases = IList::Probe(list);
        testClass->GetName(&mTestClassName);
    }
    else {
        AutoPtr<ITest> test;
        GetTest(testClass, (ITest**)&test);
        SetTest(test);
    }
    return NOERROR;
}

ECode CAndroidTestRunner::SetTest(
    /* [in] */ ITest* test)
{
    mTestCases = TestCaseUtil::GetTests(test, TRUE);
    if (ITestSuite::Probe(test) != NULL) {
        mTestClassName = TestCaseUtil::GetTestName(test);
    }
    else {
        AutoPtr<IClassInfo> clsInfo;
        _CObject_ReflectClassInfo(test, (IClassInfo**)&clsInfo);
        clsInfo->GetName(&mTestClassName);
    }
    return NOERROR;
}

ECode CAndroidTestRunner::ClearTestListeners()
{
    mTestListeners.Clear();
    return NOERROR;
}

ECode CAndroidTestRunner::AddTestListener(
    /* [in] */ ITestListener* testListener)
{
    if (testListener != NULL) {
        mTestListeners.PushBack(testListener);
    }
    return NOERROR;
}

ECode CAndroidTestRunner::LoadTestClass(
    /* [in] */ const String& testPackageName,
    /* [in] */ const String& testClassName,
    /* [out] */ IClassInfo** clazz)
{
    Int32 startIndex = testPackageName.LastIndexOf("/");
    Int32 endIndex = testPackageName.LastIndexOf("-");
    if (endIndex < 0) endIndex = testPackageName.LastIndexOf(".");
    String path = String("/data/elastos/") + testPackageName.Substring(startIndex + 1, endIndex) + String(".eco");
    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        Logger::E(TAG, "CAndroidTestRunner: Cann't Find the path is %s", path.string());
        return E_RUNTIME_EXCEPTION;
    }

    ec = moduleInfo->GetClassInfo(testClassName, clazz);
    if (FAILED(ec)) {
        Logger::E(TAG, "CAndroidTestRunner: Get class info of %s failed.", testClassName.string());
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<ITestCase> CAndroidTestRunner::BuildSingleTestMethod(
    /* [in] */ IClassInfo* testClass,
    /* [in] */ const String& testMethodName)
{
    Int32 count;
    testClass->GetConstructorCount(&count);
    if (count <= 0) return NULL;

    AutoPtr<ITestCase> testCase;
    ECode ec = NOERROR;
    AutoPtr< ArrayOf<IConstructorInfo *> > consInfos = ArrayOf<IConstructorInfo *>::Alloc(count);
    testClass->GetAllConstructorInfos(consInfos);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IConstructorInfo> constructor = (*consInfos)[i];
        Int32 paramCount;
        constructor->GetParamCount(&paramCount);
        if (paramCount == 0) {
            ec = NewSingleTestMethod(testClass, testMethodName, constructor, (ITestCase**)&testCase);
            if (SUCCEEDED(ec)) return testCase;
        }
    }

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IConstructorInfo> constructor = (*consInfos)[i];
        Int32 paramCount;
        constructor->GetParamCount(&paramCount);
        if (paramCount == 1) {
            AutoPtr< ArrayOf<IParamInfo*> > params = ArrayOf<IParamInfo*>::Alloc(1);
            constructor->GetAllParamInfos(params);
            AutoPtr<IParamInfo> param = (*params)[0];
            AutoPtr<IDataTypeInfo> typeInfo;
            param->GetTypeInfo((IDataTypeInfo**)&typeInfo);
            CarDataType type;
            typeInfo->GetDataType(&type);
            if (type == CarDataType_String) {
                ec = NewSingleTestMethod(testClass, testMethodName, constructor, testMethodName, (ITestCase**)&testCase);
                if (SUCCEEDED(ec)) return testCase;
            }
        }
    }

    return testCase;
}

ECode CAndroidTestRunner::NewSingleTestMethod(
    /* [in] */ IClassInfo* testClass,
    /* [in] */ const String& testMethodName,
    /* [in] */ IConstructorInfo* constructor,
    /* [out] */ ITestCase** testCase)
{
    AutoPtr<IInterface> object;
    ECode ec = constructor->CreateObject(NULL, (IInterface**)&object);
    if (FAILED(ec)) {
        String name;
        testClass->GetName(&name);
        Logger::E(TAG, "Constructor thew an exception. Class: %s", name.string());
        *testCase = NULL;
        return E_RUNTIME_EXCEPTION;
    }
    *testCase = ITestCase::Probe(object);
    assert(*testCase != NULL);
    (*testCase)->SetName(testMethodName);
    REFCOUNT_ADD(*testCase)
    return NOERROR;
}

ECode CAndroidTestRunner::NewSingleTestMethod(
    /* [in] */ IClassInfo* testClass,
    /* [in] */ const String& testMethodName,
    /* [in] */ IConstructorInfo* constructor,
    /* [in] */ const String& arg,
    /* [out] */ ITestCase** testCase)
{
    AutoPtr<IArgumentList> args;
    constructor->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfString(0, arg);
    AutoPtr<IInterface> object;
    ECode ec = constructor->CreateObject(args, (IInterface**)&object);
    if (FAILED(ec)) {
        String name;
        testClass->GetName(&name);
        Logger::E(TAG, "Constructor thew an exception. Class: %s", name.string());
        *testCase = NULL;
        return E_RUNTIME_EXCEPTION;
    }
    *testCase = ITestCase::Probe(object);
    assert(*testCase != NULL);
    (*testCase)->SetName(testMethodName);
    REFCOUNT_ADD(*testCase)
    return NOERROR;
}

Boolean CAndroidTestRunner::ShouldRunSingleTestMethod(
    /* [in] */ const String& testMethodName,
    /* [in] */ IClassInfo* testClass)
{
    AutoPtr<IInterfaceInfo> itfInfo;
    testClass->GetInterfaceInfo(String("ITestCase"), (IInterfaceInfo**)&itfInfo);
    return !testMethodName.IsNull() && itfInfo != NULL;
}

ECode CAndroidTestRunner::GetTest(
    /* [in] */ IClassInfo* clazz,
    /* [out] */ ITest** test)
{
    ECode ec = NOERROR;
    AutoPtr<IInterfaceInfo> itfInfo;
    clazz->GetInterfaceInfo(String("ITestSuiteProvider"), (IInterfaceInfo**)&itfInfo);
    if (itfInfo != NULL) {
        AutoPtr<IInterface> object;
        Int32 count;
        clazz->GetConstructorCount(&count);
        AutoPtr< ArrayOf<IConstructorInfo *> > consInfos = ArrayOf<IConstructorInfo *>::Alloc(count);
        clazz->GetAllConstructorInfos(consInfos);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IConstructorInfo> consInfo = (*consInfos)[i];
            Int32 paramCount;
            consInfo->GetParamCount(&paramCount);
            if (paramCount == 0) {
                ec = consInfo->CreateObject(NULL, (IInterface**)&object);
                break;
            }
        }
        if (FAILED(ec)) {
            String name;
            clazz->GetName(&name);
            Logger::E(TAG, "Could not instantiate test suite provider. Class: %s", name.string());
            return E_RUNTIME_EXCEPTION;
        }
        AutoPtr<ITestSuiteProvider> testSuiteProvider = ITestSuiteProvider::Probe(object);
        assert(testSuiteProvider != NULL);
        AutoPtr<ITestSuite> suite;
        testSuiteProvider->GetTestSuite((ITestSuite**)&suite);
        *test = ITest::Probe(suite);
        REFCOUNT_ADD(*test);
        return NOERROR;
    }
    AutoPtr<IMethodInfo> suiteMethod;
    ec = clazz->GetMethodInfo(String("suite"), String("")/*TODO*/, (IMethodInfo**)&suiteMethod);
    if (FAILED(ec)) {
        // try to extract a test suite automatically
        ClearStatus();
        AutoPtr<ITestSuite> suite;
        CTestSuite::New(clazz, (ITestSuite**)&suite);
        *test = ITest::Probe(suite);
        REFCOUNT_ADD(*test);
        return NOERROR;
    }
    assert(0);
    return NOERROR;
    // if (! Modifier.isStatic(suiteMethod.getModifiers())) {
    //     runFailed("Suite() method must be static");
    //     return null;
    // }
    // Test test= null;
    // try {
    //     test= (Test)suiteMethod.invoke(null, (Object[])new Class[0]); // static method
    //     if (test == null)
    //         return test;
    // }
    // catch (InvocationTargetException e) {
    //     runFailed("Failed to invoke suite():" + e.getTargetException().toString());
    //     return null;
    // }
    // catch (IllegalAccessException e) {
    //     runFailed("Failed to invoke suite():" + e.toString());
    //     return null;
    // }

    // clearStatus();
    // return test;
}

AutoPtr<ITestResult> CAndroidTestRunner::CreateTestResult()
{
    if (mSkipExecution) {
        assert(0);
        return NULL;
    }
    AutoPtr<ITestResult> result;
    CTestResult::New((ITestResult**)&result);
    return result;
}

void CAndroidTestRunner::SetSkipExecution(
    /* [in] */ Boolean skip)
{
    mSkipExecution = skip;
}

ECode CAndroidTestRunner::GetTestCases(
    /* [out] */ IList** testCases)
{
    VALIDATE_NOT_NULL(testCases);
    *testCases = mTestCases;
    REFCOUNT_ADD(*testCases);
    return NOERROR;
}

ECode CAndroidTestRunner::GetTestClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className);
    *className = mTestClassName;
    return NOERROR;
}

ECode CAndroidTestRunner::GetTestResult(
    /* [out] */ ITestResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTestResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAndroidTestRunner::RunTest()
{
    return RunTest(CreateTestResult());
}

ECode CAndroidTestRunner::RunTest(
    /* [in] */ ITestResult* testResult)
{
    mTestResult = testResult;

    List< AutoPtr<ITestListener> >::Iterator it;
    for (it = mTestListeners.Begin(); it != mTestListeners.End(); ++it) {
        mTestResult->AddListener(*it);
    }

    AutoPtr<IContext> testContext;
    if (mInstrumentation == NULL) testContext = mContext;
    else {
        mInstrumentation->GetContext((IContext**)&testContext);
    }

    Int32 N;
    mTestCases->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mTestCases->Get(i, (IInterface**)&obj);
        ITest* testCase = ITest::Probe(obj);
        SetContextIfAndroidTestCase(testCase, mContext, testContext);
        SetInstrumentationIfInstrumentationTestCase(testCase, mInstrumentation);
        SetPerformanceWriterIfPerformanceCollectorTestCase(testCase, mPerfWriter);
        testCase->Run(mTestResult);
    }
    return NOERROR;
}

void CAndroidTestRunner::SetContextIfAndroidTestCase(
    /* [in] */ ITest* test,
    /* [in] */ IContext* context,
    /* [in] */ IContext* testContext)
{
    if (IElastosTestCase::Probe(test) != NULL) {
        IElastosTestCase::Probe(test)->SetContext(context);
        IElastosTestCase::Probe(test)->SetTestContext(testContext);
    }
}

ECode CAndroidTestRunner::SetContext(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

void CAndroidTestRunner::SetInstrumentationIfInstrumentationTestCase(
    /* [in] */ ITest* test,
    /* [in] */ IInstrumentation* instrumentation)
{
    if (IInstrumentationTestCase::Probe(test) != NULL) {
        IInstrumentationTestCase::Probe(test)->InjectInstrumentation(instrumentation);
    }
}

void CAndroidTestRunner::SetPerformanceWriterIfPerformanceCollectorTestCase(
    /* [in] */ ITest* test,
    /* [in] */ IPerformanceResultsWriter* writer)
{
    if (IPerformanceCollectorTestCase::Probe(test) != NULL) {
        IPerformanceCollectorTestCase::Probe(test)->SetPerformanceResultsWriter(writer);
    }
}

ECode CAndroidTestRunner::SetInstrumentation(
    /* [in] */ IInstrumentation* instrumentation)
{
    mInstrumentation = instrumentation;
    return NOERROR;
}

ECode CAndroidTestRunner::SetPerformanceResultsWriter(
    /* [in] */ IPerformanceResultsWriter* writer)
{
    mPerfWriter = writer;
    return NOERROR;
}

ECode CAndroidTestRunner::SetPreference(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidTestRunner::TestStarted(
    /* [in] */ const String& testName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidTestRunner::TestEnded(
    /* [in] */ const String& testName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidTestRunner::TestFailed(
    /* [in] */ Int32 status,
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidTestRunner::ElapsedTimeAsString(
    /* [in] */ Int64 runTime,
    /* [out] */ String* timeStr)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidTestRunner::SetLoading(
    /* [in] */ Boolean bEnable)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidTestRunner::ExtractClassName(
    /* [in] */ const String& className,
    /* [out] */ String* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CAndroidTestRunner::GetLoader(
    /* [out] */ ITestSuiteLoader** loader)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
