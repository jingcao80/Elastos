
#include "elastos/droid/test/suitebuilder/CTestSuiteBuilder.h"
#include "elastos/droid/test/CAndroidTestRunner.h"
#include "elastos/droid/test/TestCaseUtil.h"
#include "elastos/droid/test/suitebuilder/CTestMethod.h"

using Elastos::Utility::CHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Eunit::Framework::ITestCase;
using Eunit::Framework::ITest;
using Eunit::Framework::CTestSuite;

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

CAR_INTERFACE_IMPL(CTestSuiteBuilder, Object, ITestSuiteBuilder)

CTestSuiteBuilder::CTestSuiteBuilder()
{
    mTestGrouping = new TestGrouping(TestGrouping::SORT_BY_FULLY_QUALIFIED_NAME);
    CHashSet::New((ISet**)&mPredicates);
}

ECode CTestSuiteBuilder::constructor(
    /* [in] */ IClassInfo* clazz)
{
    // this(clazz.getName(), clazz.getClassLoader());
    String name;
    clazz->GetName(&name);
    return constructor(name, NULL);
}

ECode CTestSuiteBuilder::constructor(
    /* [in] */ const String& name,
    /* [in] */ IClassLoader* classLoader)
{
    mSuiteName = name;
    mTestGrouping->SetClassLoader(classLoader);
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    mTestCases = IList::Probe(list);
    return AddRequirements((ArrayOf<IPredicate*>*)NULL/*REJECT_SUPPRESSED*/);
}

ECode CTestSuiteBuilder::AddTestClassByName(
    /* [in] */ const String& testPackageName,
    /* [in] */ const String& testClassName,
    /* [in] */ const String& testMethodName,
    /* [in] */ IContext* context)
{
    AutoPtr<CAndroidTestRunner> atr;
    CAndroidTestRunner::NewByFriend((CAndroidTestRunner**)&atr);
    atr->SetContext(context);
    atr->SetTestClassName(testPackageName, testClassName, testMethodName);
    AutoPtr<IList> testCases;
    atr->GetTestCases((IList**)&testCases);
    Boolean result;
    mTestCases->AddAll(ICollection::Probe(testCases), &result);
    return NOERROR;
}

ECode CTestSuiteBuilder::AddTestSuite(
    /* [in] */ ITestSuite* testSuite)
{
    AutoPtr<IList> testCases = TestCaseUtil::GetTests(ITest::Probe(testSuite), TRUE);
    if (testCases != NULL) {
        Int32 N;
        testCases->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            testCases->Get(i, (IInterface**)&obj);
            assert(ITestCase::Probe(obj) != NULL);
            Boolean result;
            mTestCases->Add(ITestCase::Probe(obj), &result);
        }
    }
    return NOERROR;
}

ECode CTestSuiteBuilder::IncludePackages(
    /* [in] */ ArrayOf<String>* packageNames)
{
    mTestGrouping->AddPackagesRecursive(packageNames);
    return NOERROR;
}

ECode CTestSuiteBuilder::ExcludePackages(
    /* [in] */ ArrayOf<String>* packageNames)
{
    mTestGrouping->RemovePackagesRecursive(packageNames);
    return NOERROR;
}

ECode CTestSuiteBuilder::AddRequirements(
    /* [in] */ IList* predicates)
{
    Boolean result;
    mPredicates->AddAll(ICollection::Probe(predicates), &result);
    return NOERROR;
}

ECode CTestSuiteBuilder::IncludeAllPackagesUnderHere()
{
    assert(0);
//    StackTraceElement[] stackTraceElements = Thread.currentThread().getStackTrace();
//
//    String callingClassName = null;
//    String thisClassName = TestSuiteBuilder.class.getName();
//
//    // We want to get the package of this method's calling class. This method's calling class
//    // should be one level below this class in the stack trace.
//    for (int i = 0; i < stackTraceElements.length; i++) {
//        StackTraceElement element = stackTraceElements[i];
//        if (thisClassName.equals(element.getClassName())
//                && "includeAllPackagesUnderHere".equals(element.getMethodName())) {
//            // We've found this class in the call stack. The calling class must be the
//            // next class in the stack.
//            callingClassName = stackTraceElements[i + 1].getClassName();
//            break;
//        }
//    }
//
//    String packageName = parsePackageNameFromClassName(callingClassName);
//    return includePackages(packageName);
    return E_NOT_IMPLEMENTED;
}

ECode CTestSuiteBuilder::Named(
    /* [in] */ const String& newSuiteName)
{
    mSuiteName = newSuiteName;
    return NOERROR;
}

ECode CTestSuiteBuilder::Build(
    /* [out] */ ITestSuite** suite)
{
    mRootSuite = NULL;
    CTestSuite::New(GetSuiteName(), (ITestSuite**)&mRootSuite);

    // Keep track of current class so we know when to create a new sub-suite.
    mCurrentClassname = NULL;
    // try {
    AutoPtr<IList> tests = mTestGrouping->GetTests();
    Int32 N;
    tests->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> test;
        tests->Get(i, (IInterface**)&obj);
        ITestMethod* test = ITestMethod::Probe(obj);
        if (SatisfiesAllPredicates(test)) {
            AddTest(test);
        }
    }
    if (mTestCases->GetSize(&N), N > 0) {
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mTestCases->Get(i, (IInterface**)&obj);
            ITestCase* testCase = ITestCase::Probe(obj);
            AutoPtr<ITestMethod> testMethod;
            CTestMethod::New(testCase, (ITestMethod**)&testMethod);
            if (SatisfiesAllPredicates(testMethod)) {
                AddTest(ITest::Probe(testCase));
            }
        }
    }
    // } catch (Exception exception) {
    //     Log.i("TestSuiteBuilder", "Failed to create test.", exception);
    //     TestSuite suite = new TestSuite(getSuiteName());
    //     suite.addTest(new FailedToCreateTests(exception));
    //     return suite;
    // }
    // return rootSuite;
    *suite = mRootSuite;
    REFCOUNT_ADD(*suite);
    return NOERROR;
}

String CTestSuiteBuilder::GetSuiteName()
{
    return mSuiteName;
}

ECode CTestSuiteBuilder::AddRequirements(
    /* [in] */ ArrayOf<IPredicate*>* predicates)
{
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    if (predicates != NULL) {
        for (Int32 i = 0; i < predicates->GetLength(); ++i) {
            Boolean result;
            list->Add((*predicates)[i], &result);
        }
    }
    return AddRequirements(IList::Probe(list));
}

Boolean CTestSuiteBuilder::SatisfiesAllPredicates(
    /* [in] */ ITestMethod* test)
{
    AutoPtr<IIterator> it;
    mPredicates->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> predicate;
        it->GetNext((IInterface**)&obj);
        IPredicate* predicate = IPredicate::Probe(obj);
        Boolean result;
        if (predicate->Apply(test, &result), !result) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode CTestSuiteBuilder::AddTest(
    /* [in] */ ITestMethod* testMethod)
{
    String parentClassname;
    testMethod->GetEnclosingClassname(&parentClassname);
    AddSuiteIfNecessary(parentClassname);
    AutoPtr<ITestCase> testCase;
    FAIL_RETURN(testMethod->CreateTest((ITestCase**)&testCase))
    mSuiteForCurrentClass->AddTest(ITest::Probe(testCase));
    return NOERROR;
}

ECode CTestSuiteBuilder::AddTest(
    /* [in] */ ITest* test)
{
    AutoPtr<IClassInfo> clsInfo;
    _CObject_ReflectClassInfo(test, (IClassInfo**)&clsInfo);
    String clsName;
    clsInfo->GetName(&clsName);
    AddSuiteIfNecessary(clsName);
    mSuiteForCurrentClass->AddTest(ITest::Probe(test));
    return NOERROR;
}

void CTestSuiteBuilder::AddSuiteIfNecessary(
    /* [in] */ const String& parentClassname)
{
    if (!parentClassname.Equals(mCurrentClassname)) {
        mCurrentClassname = parentClassname;
        mSuiteForCurrentClass = NULL;
        CTestSuite::New(parentClassname, (ITestSuite**)&mSuiteForCurrentClass);
        mRootSuite->AddTest(ITest::Probe(mSuiteForCurrentClass));
    }
}

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos
