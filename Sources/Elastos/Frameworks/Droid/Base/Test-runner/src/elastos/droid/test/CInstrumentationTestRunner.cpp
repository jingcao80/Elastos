
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/test/CInstrumentationTestRunner.h"
#include "elastos/droid/test/ClassPathPackageInfoSource.h"
#include "elastos/droid/test/TestPrinter.h"
#include "elastos/droid/test/TestFilePrinter.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IPrintStream;
using Elastos::IO::CPrintStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::EIID_IInstrumentation;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::EIID_IPerformanceResultsWriter;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Internal::Utility::EIID_IPredicate;
using Elastos::Droid::Test::SuiteBuilder::ITestSuiteBuilder;
using Elastos::Droid::Test::SuiteBuilder::ITestMethod;
using Eunit::Framework::ITestCase;
using Eunit::Framework::ITestAnnotation;
using Eunit::Framework::EIID_ITestListener;

namespace Elastos {
namespace Droid {
namespace Test {

//=================================================================
// CInstrumentationTestRunner::StringResultPrinter
//=================================================================
void CInstrumentationTestRunner::StringResultPrinter::Print(
    /* [in] */ ITestResult* result,
    /* [in] */ Int64 runTime)
{
    PrintHeader(runTime);
    PrintFooter(result);
}


//=================================================================
// CInstrumentationTestRunner::SuiteAssignmentPrinter
//=================================================================
CAR_INTERFACE_IMPL(CInstrumentationTestRunner::SuiteAssignmentPrinter, Object, ITestListener)

ECode CInstrumentationTestRunner::SuiteAssignmentPrinter::StartTest(
    /* [in] */ ITest* test)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mStartTime);
    mTimingValid = TRUE;
    return NOERROR;
}

ECode CInstrumentationTestRunner::SuiteAssignmentPrinter::AddError(
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    mTimingValid = FALSE;
    return NOERROR;
}

ECode CInstrumentationTestRunner::SuiteAssignmentPrinter::AddFailure(
    /* [in] */ ITest* test,
    /* [in] */ ITestFailure* failure)
{
    mTimingValid = FALSE;
    return NOERROR;
}

ECode CInstrumentationTestRunner::SuiteAssignmentPrinter::EndTest(
    /* [in] */ ITest* test)
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    Float runTime;
    String assignmentSuite;
    system->GetCurrentTimeMillis(&mEndTime);
    mTestResult = NULL;
    CBundle::New((IBundle**)&mTestResult);

    if (!mTimingValid || mStartTime < 0) {
        assignmentSuite = "NA";
        runTime = -1;
    }
    else {
        runTime = mEndTime - mStartTime;
        if (runTime < SMALL_SUITE_MAX_RUNTIME
                && (IInstrumentationTestCase::Probe(test) == NULL)) {
            assignmentSuite = SMALL_SUITE;
        }
        else if (runTime < MEDIUM_SUITE_MAX_RUNTIME) {
            assignmentSuite = MEDIUM_SUITE;
        }
        else {
            assignmentSuite = LARGE_SUITE;
        }
    }
    // Clear mStartTime so that we can verify that it gets set next time.
    mStartTime = -1;

    StringBuilder sb;
    AutoPtr<IClassInfo> clsInfo;
    _CObject_ReflectClassInfo(test, (IClassInfo**)&clsInfo);
    String name;
    clsInfo->GetName(&name);
    sb.Append(name);
    sb.Append("#");
    ITestCase::Probe(test)->GetName(&name);
    sb.Append(name);
    sb.Append("\nin ");
    sb.Append(assignmentSuite);
    sb.Append(" suite\nrunTime: ");
    sb.Append(StringUtils::ToString(runTime));
    sb.Append("\n");
    mTestResult->PutString(IInstrumentation::REPORT_KEY_STREAMRESULT, sb.ToString());
    mTestResult->PutFloat(REPORT_KEY_RUN_TIME, runTime);
    mTestResult->PutString(REPORT_KEY_SUITE_ASSIGNMENT, assignmentSuite);

    mHost->SendStatus(0, mTestResult);
    return NOERROR;
}


//=================================================================
// CInstrumentationTestRunner::WatcherResultPrinter
//=================================================================
CInstrumentationTestRunner::WatcherResultPrinter::WatcherResultPrinter(
    /* [in] */ Int32 numTests,
    /* [in] */ CInstrumentationTestRunner* host)
    : mTestNum(0)
    , mTestResultCode(0)
    , mIsTimedTest(FALSE)
    , mIncludeDetailedStats(FALSE)
    , mHost(host)
{
    CBundle::New((IBundle**)&mResultTemplate);
    mResultTemplate->PutString(IInstrumentation::REPORT_KEY_IDENTIFIER, IInstrumentationTestRunner::REPORT_VALUE_ID);
    mResultTemplate->PutInt32(IInstrumentationTestRunner::REPORT_KEY_NUM_TOTAL, numTests);
}

CAR_INTERFACE_IMPL_2(CInstrumentationTestRunner::WatcherResultPrinter, Object, ITestListener, IPerformanceResultsWriter)

ECode CInstrumentationTestRunner::WatcherResultPrinter::StartTest(
    /* [in] */ ITest* test)
{
    AutoPtr<IClassInfo> clsInfo;
    _CObject_ReflectClassInfo(test, (IClassInfo**)&clsInfo);
    String testClass;
    clsInfo->GetName(&testClass);
    String testName;
    ITestCase::Probe(test)->GetName(&testName);
    mTestResult = NULL;
    CBundle::New(mResultTemplate, (IBundle**)&mTestResult);
    mTestResult->PutString(IInstrumentationTestRunner::REPORT_KEY_NAME_CLASS, testClass);
    mTestResult->PutString(IInstrumentationTestRunner::REPORT_KEY_NAME_TEST, testName);
    mTestResult->PutInt32(IInstrumentationTestRunner::REPORT_KEY_NUM_CURRENT, ++mTestNum);
    // pretty printing
    if (!testClass.IsNull() && !testClass.Equals(mTestClass)) {
        String formatStr;
        formatStr.AppendFormat("\n%s:", testClass.string());
        mTestResult->PutString(IInstrumentation::REPORT_KEY_STREAMRESULT,
                formatStr);
        mTestClass = testClass;
    }
    else {
        mTestResult->PutString(IInstrumentation::REPORT_KEY_STREAMRESULT, String(""));
    }

//    Method testMethod = null;
//    try {
//        testMethod = test.getClass().getMethod(testName);
//        // Report total number of iterations, if test is repetitive
//        if (testMethod.isAnnotationPresent(RepetitiveTest.class)) {
//            int numIterations = testMethod.getAnnotation(
//                RepetitiveTest.class).numIterations();
//            mTestResult.putInt(REPORT_KEY_NUM_ITERATIONS, numIterations);
//        }
//    } catch (NoSuchMethodException e) {
//        // ignore- the test with given name does not exist. Will be handled during test
//        // execution
//    }

    AutoPtr<ITestAnnotation> annotation;
    test->GetTestAnnotation((ITestAnnotation**)&annotation);
    if (annotation != NULL) {
        Int32 numIterations;
        annotation->GetNumIterations(testName, &numIterations);
        mTestResult->PutInt32(REPORT_KEY_NUM_ITERATIONS, numIterations);
    }

    // The delay_msec parameter is normally used to provide buffers of idle time
    // for power measurement purposes. To make sure there is a delay before and after
    // every test in a suite, we delay *after* every test (see endTest below) and also
    // delay *before* the first test. So, delay test1 delay test2 delay.

    // try {
    if (mTestNum == 1) {
        ECode ec = Thread::Sleep(mHost->mDelayMsec);
        if (ec == (ECode)E_INTERRUPTED_EXCEPTION) return E_ILLEGAL_STATE_EXCEPTION;
    }
    // } catch (InterruptedException e) {
    //     throw new IllegalStateException(e);
    // }

    mHost->SendStatus(IInstrumentationTestRunner::REPORT_VALUE_RESULT_START, mTestResult);
    mTestResultCode = 0;

    mIsTimedTest = FALSE;
    mIncludeDetailedStats = FALSE;
    if (annotation != NULL) {
        annotation->IsTimedTest(testName, &mIsTimedTest);
        annotation->IsIncludeDetailedStats(testName, &mIncludeDetailedStats);
    }
//    try {
//        // Look for TimedTest annotation on both test class and test method
//        if (testMethod != null && testMethod.isAnnotationPresent(TimedTest.class)) {
//            mIsTimedTest = true;
//            mIncludeDetailedStats = testMethod.getAnnotation(
//                    TimedTest.class).includeDetailedStats();
//        } else if (test.getClass().isAnnotationPresent(TimedTest.class)) {
//            mIsTimedTest = true;
//            mIncludeDetailedStats = test.getClass().getAnnotation(
//                    TimedTest.class).includeDetailedStats();
//        }
//    } catch (SecurityException e) {
//        // ignore - the test with given name cannot be accessed. Will be handled during
//        // test execution
//    }

    // TODO
    // if (mIsTimedTest && mIncludeDetailedStats) {
    //     mPerfCollector.beginSnapshot("");
    // }
    // else if (mIsTimedTest) {
    //     mPerfCollector.startTiming("");
    // }

    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::AddError(
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    assert(0);
    // mTestResult.putString(REPORT_KEY_STACK, BaseTestRunner.getFilteredTrace(t));
    mTestResultCode = IInstrumentationTestRunner::REPORT_VALUE_RESULT_ERROR;
    // pretty printing
    // mTestResult.putString(Instrumentation.REPORT_KEY_STREAMRESULT,
    //     String.format("\nError in %s:\n%s",
    //         ((TestCase)test).getName(), BaseTestRunner.getFilteredTrace(t)));
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::AddFailure(
    /* [in] */ ITest* test,
    /* [in] */ ITestFailure* failure)
{
    String message;
    failure->ExceptionMessage(&message);
    String trace;
    failure->Trace(&trace);
    message.Append("\n");
    message.Append(trace);
    trace = message;
    String prevTrace;
    mTestResult->GetString(REPORT_KEY_STACK, &prevTrace);
    if (!prevTrace.IsNull()) {
        prevTrace.Append("\n");
        prevTrace.Append(trace);
        trace = prevTrace;
    }
    mTestResult->PutString(REPORT_KEY_STACK, trace);
    mTestResultCode = IInstrumentationTestRunner::REPORT_VALUE_RESULT_FAILURE;
    // pretty printing
    String name;
    ITestCase::Probe(test)->GetName(&name);
    String result;
    result.AppendFormat("\nFailure in %s:\n%s", name.string(), trace.string());
    mTestResult->PutString(IInstrumentation::REPORT_KEY_STREAMRESULT, result);
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::EndTest(
    /* [in] */ ITest* test)
{
    // TODO:
    // if (mIsTimedTest && mIncludeDetailedStats) {
    //     mTestResult.putAll(mPerfCollector.endSnapshot());
    // } else if (mIsTimedTest) {
    //     writeStopTiming(mPerfCollector.stopTiming(""));
    // }

    if (mTestResultCode == 0) {
        mTestResult->PutString(IInstrumentation::REPORT_KEY_STREAMRESULT, String("."));
    }
    mHost->SendStatus(mTestResultCode, mTestResult);

    // try { // Sleep after every test, if specified
    ECode ec = Thread::Sleep(mHost->mDelayMsec);
    if (ec == (ECode)E_INTERRUPTED_EXCEPTION) return E_ILLEGAL_STATE_EXCEPTION;
    // } catch (InterruptedException e) {
    //     throw new IllegalStateException(e);
    // }
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::WriteBeginSnapshot(
    /* [in] */ const String& label)
{
    // Do nothing
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::WriteEndSnapshot(
    /* [in] */ IBundle* results)
{
    // Copy all snapshot data fields into mResults, which is outputted
    // via Instrumentation.finish
    mHost->mResults->PutAll(results);
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::WriteStartTiming(
    /* [in] */ const String& label)
{
    // Do nothing
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::WriteStopTiming(
    /* [in] */ IBundle* results)
{
    assert(0);
    // Copy results into mTestResult by flattening list of iterations,
    // which is outputted via WatcherResultPrinter.endTest
    // int i = 0;
    // for (Parcelable p :
    //         results.getParcelableArrayList(PerformanceCollector.METRIC_KEY_ITERATIONS)) {
    //     Bundle iteration = (Bundle)p;
    //     String index = "iteration" + i + ".";
    //     mTestResult.putString(index + PerformanceCollector.METRIC_KEY_LABEL,
    //             iteration.getString(PerformanceCollector.METRIC_KEY_LABEL));
    //     mTestResult.putLong(index + PerformanceCollector.METRIC_KEY_CPU_TIME,
    //             iteration.getLong(PerformanceCollector.METRIC_KEY_CPU_TIME));
    //     mTestResult.putLong(index + PerformanceCollector.METRIC_KEY_EXECUTION_TIME,
    //             iteration.getLong(PerformanceCollector.METRIC_KEY_EXECUTION_TIME));
    //     i++;
    // }
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::WriteMeasurement(
    /* [in] */ const String& label,
    /* [in] */ Int64 value)
{
    mTestResult->PutInt64(label, value);
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::WriteMeasurement(
    /* [in] */ const String& label,
    /* [in] */ Float value)
{
    mTestResult->PutFloat(label, value);
    return NOERROR;
}

ECode CInstrumentationTestRunner::WatcherResultPrinter::WriteMeasurement(
    /* [in] */ const String& label,
    /* [in] */ const String& value)
{
    mTestResult->PutString(label, value);
    return NOERROR;
}

//==================================================================
//     CInstrumentationTestRunner::TestMethodPredicate
//==================================================================
CAR_INTERFACE_IMPL(CInstrumentationTestRunner::TestMethodPredicate, Object, IPredicate)

ECode CInstrumentationTestRunner::TestMethodPredicate::Apply(
    /* [in] */ IInterface* t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ITestMethod> method = ITestMethod::Probe(t);
    assert(method != NULL);
    String name;
    method->GetName(&name);
    AutoPtr<IClassInfo> clazz;
    method->GetEnclosingClass((IClassInfo**)&clazz);
    AutoPtr<IMethodInfo> methodInfo;
    clazz->GetMethodInfo(name, String("")/*TODO*/, (IMethodInfo**)&methodInfo);
    Int32 count;
    methodInfo->GetParamCount(&count);
    *result = ((count == 0) && (name.StartWith("Test")));
    return NOERROR;
    // return ((method.getParameterTypes().length == 0) &&
    //         (method.getName().startsWith("test")) &&
    //         (method.getReturnType().getSimpleName().equals("void")));
}


//=================================================================
// CInstrumentationTestRunner
//=================================================================
const String CInstrumentationTestRunner::SMALL_SUITE("small");
const String CInstrumentationTestRunner::MEDIUM_SUITE("medium");
const String CInstrumentationTestRunner::LARGE_SUITE("large");

const String CInstrumentationTestRunner::ARGUMENT_LOG_ONLY("log");
const String CInstrumentationTestRunner::ARGUMENT_ANNOTATION("annotation");
const String CInstrumentationTestRunner::ARGUMENT_NOT_ANNOTATION("notAnnotation");
const Float CInstrumentationTestRunner::SMALL_SUITE_MAX_RUNTIME = 100;

const Float CInstrumentationTestRunner::MEDIUM_SUITE_MAX_RUNTIME = 1000;

const String CInstrumentationTestRunner::REPORT_KEY_RUN_TIME("runtime");
const String CInstrumentationTestRunner::REPORT_KEY_NUM_ITERATIONS("numiterations");
const String CInstrumentationTestRunner::REPORT_KEY_SUITE_ASSIGNMENT("suiteassignment");
const String CInstrumentationTestRunner::REPORT_KEY_COVERAGE_PATH("coverageFilePath");

const String CInstrumentationTestRunner::DEFAULT_COVERAGE_FILE_NAME("coverage.ec");

const String CInstrumentationTestRunner::TAG("InstrumentationTestRunner");

CInstrumentationTestRunner::CInstrumentationTestRunner()
    : mDebug(FALSE)
    , mJustCount(FALSE)
    , mSuiteAssignmentMode(FALSE)
    , mTestCount(0)
    , mCoverage(FALSE)
    , mDelayMsec(0)
{
    CBundle::New((IBundle**)&mResults);
}

CAR_INTERFACE_IMPL_2(CInstrumentationTestRunner, Instrumentation, IInstrumentationTestRunner, ITestSuiteProvider)

ECode CInstrumentationTestRunner::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    VALIDATE_NOT_NULL(pCLSID);
    *pCLSID = ECLSID_CInstrumentationTestRunner;
    return NOERROR;
}

ECode CInstrumentationTestRunner::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = this->Probe(EIID_IInterface) == other->Probe(EIID_IInterface);
    return NOERROR;
}

ECode CInstrumentationTestRunner::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)this;
    return NOERROR;
}

ECode CInstrumentationTestRunner::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "Elastos.Droid.Test.CInstrumentationTestRunner";
    return NOERROR;
}

ECode CInstrumentationTestRunner::OnCreate(
    /* [in] */ IBundle* arguments)
{
    FAIL_RETURN(Instrumentation::OnCreate(arguments));
    mArguments = arguments;

    // Apk paths used to search for test classes when using TestSuiteBuilders.
    AutoPtr< ArrayOf<String> > epkPaths = ArrayOf<String>::Alloc(2);
    AutoPtr<IContext> context;
    GetTargetContext((IContext**)&context);
    String codePath;
    context->GetPackageCodePath(&codePath);
    epkPaths->Set(0, codePath);
    context = NULL;
    GetContext((IContext**)&context);
    context->GetPackageCodePath(&codePath);
    epkPaths->Set(1, codePath);
    ClassPathPackageInfoSource::SetEpkPaths(epkPaths);

    AutoPtr<IPredicate> testSizePredicate;
    AutoPtr<IPredicate> testAnnotationPredicate;
    AutoPtr<IPredicate> testNotAnnotationPredicate;
    AutoPtr<IPredicate> testMethodPredicate = new TestMethodPredicate();
    String testClassesArg;
    Boolean logOnly = FALSE;

    if (arguments != NULL) {
        // Test class name passed as an argument should override any meta-data declaration.
        arguments->GetString(IInstrumentationTestRunner::ARGUMENT_TEST_CLASS, &testClassesArg);
        mDebug = GetBooleanArgument(arguments, String("debug"));
        mJustCount = GetBooleanArgument(arguments, String("count"));
        mSuiteAssignmentMode = GetBooleanArgument(arguments, String("suiteAssignment"));
        arguments->GetString(IInstrumentationTestRunner::ARGUMENT_TEST_PACKAGE, &mPackageOfTests);
        String value;
        arguments->GetString(IInstrumentationTestRunner::ARGUMENT_TEST_SIZE_PREDICATE, &value);
        testSizePredicate = GetSizePredicateFromArg(value);
        arguments->GetString(ARGUMENT_ANNOTATION, &value);
        testAnnotationPredicate = GetAnnotationPredicate(value);
        arguments->GetString(ARGUMENT_NOT_ANNOTATION, &value);
        testNotAnnotationPredicate = GetNotAnnotationPredicate(value);

        logOnly = GetBooleanArgument(arguments, ARGUMENT_LOG_ONLY);
        mCoverage = GetBooleanArgument(arguments, String("coverage"));
        arguments->GetString(String("coverageFile"), &mCoverageFilePath);

        // try {
        AutoPtr<IInterface> delay;
        arguments->Get(IInstrumentationTestRunner::ARGUMENT_DELAY_MSEC, (IInterface**)&delay);  // Accept either string or int
        if (delay != NULL) {
            assert(IObject::Probe(delay) != NULL);
            String delayStr;
            IObject::Probe(delay)->ToString(&delayStr);
            ECode ec = StringUtils::Parse(delayStr, &mDelayMsec);
            if (FAILED(ec)) {
                Logger::E(TAG, "Invalid delay_msec parameter");
            }
        }
        // } catch (NumberFormatException e) {
        //     Log.e(TAG, "Invalid delay_msec parameter", e);
        // }
    }

    AutoPtr<IClassInfo> clsInfo;
    _CObject_ReflectClassInfo((IObject*)this, (IClassInfo**)&clsInfo);
    String clsName;
    clsInfo->GetName(&clsName);
    context = NULL;
    GetTargetContext((IContext**)&context);
    AutoPtr<IClassLoader> loader;
    // TODO:
    // context->GetClassLoader((IClassLoader**)&loader);
    AutoPtr<CTestSuiteBuilder> testSuiteBuilder;
    CTestSuiteBuilder::NewByFriend(clsName, loader, (CTestSuiteBuilder**)&testSuiteBuilder);

    if (testSizePredicate != NULL) {
        AutoPtr< ArrayOf<IPredicate*> > predicates = ArrayOf<IPredicate*>::Alloc(1);
        predicates->Set(0, testSizePredicate);
        testSuiteBuilder->AddRequirements(predicates);
    }
    if (testAnnotationPredicate != NULL) {
        AutoPtr< ArrayOf<IPredicate*> > predicates = ArrayOf<IPredicate*>::Alloc(1);
        predicates->Set(0, testAnnotationPredicate);
        testSuiteBuilder->AddRequirements(predicates);
    }
    if (testNotAnnotationPredicate != NULL) {
        AutoPtr< ArrayOf<IPredicate*> > predicates = ArrayOf<IPredicate*>::Alloc(1);
        predicates->Set(0, testNotAnnotationPredicate);
        testSuiteBuilder->AddRequirements(predicates);
    }
    if (testMethodPredicate != NULL) {
        AutoPtr< ArrayOf<IPredicate*> > predicates = ArrayOf<IPredicate*>::Alloc(1);
        predicates->Set(0, testMethodPredicate);
        testSuiteBuilder->AddRequirements(predicates);
    }

    if (testClassesArg.IsNull()) {
        if (!mPackageOfTests.IsNull()) {
            AutoPtr< ArrayOf<String> > packages = ArrayOf<String>::Alloc(1);
            packages->Set(0, mPackageOfTests);
            testSuiteBuilder->IncludePackages(packages);
        }
        else {
            AutoPtr<ITestSuite> testSuite;
            GetTestSuite((ITestSuite**)&testSuite);
            if (testSuite != NULL) {
                testSuiteBuilder->AddTestSuite(testSuite);
            }
            else {
                // no package or class bundle arguments were supplied, and no test suite
                // provided so add all tests in application
                AutoPtr< ArrayOf<String> > packages = ArrayOf<String>::Alloc(1);
                packages->Set(0, String(""));
                testSuiteBuilder->IncludePackages(packages);
            }
        }
    }
    else {
        ParseTestClasses((*epkPaths)[1], testClassesArg, testSuiteBuilder);
    }

    AutoPtr<IList> predicates = GetBuilderRequirements();
    testSuiteBuilder->AddRequirements(predicates);

    mTestRunner = GetAndroidTestRunner();
    mTestRunner->SetContext(context);
    mTestRunner->SetInstrumentation(this);
    mTestRunner->SetSkipExecution(logOnly);
    AutoPtr<ITestSuite> testSuite;
    testSuiteBuilder->Build((ITestSuite**)&testSuite);
    mTestRunner->SetTest(ITest::Probe(testSuite));
    AutoPtr<IList> testCases;
    mTestRunner->GetTestCases((IList**)&testCases);
    testCases->GetSize(&mTestCount);
    if (mSuiteAssignmentMode) {
        AutoPtr<ITestListener> listener = new SuiteAssignmentPrinter(this);
        mTestRunner->AddTestListener(listener);
    }
    else {
        AutoPtr<WatcherResultPrinter> resultPrinter = new WatcherResultPrinter(mTestCount, this);
        AutoPtr<TestPrinter> testPrinter = new TestPrinter(String("TestRunner"), FALSE);
        AutoPtr<TestFilePrinter> testFilePrinter = new TestFilePrinter(String("TestRunner"), (*epkPaths)[1], FALSE);
        mTestRunner->AddTestListener(testFilePrinter.Get());
        mTestRunner->AddTestListener(testPrinter.Get());
        mTestRunner->AddTestListener(resultPrinter.Get());
        mTestRunner->SetPerformanceResultsWriter(resultPrinter.Get());
    }
    Start();

    return NOERROR;
}

ECode CInstrumentationTestRunner::GetBundle(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    *bundle = mArguments;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

void CInstrumentationTestRunner::AddTestListener(
    /* [in] */ ITestListener* listener)
{
    if (mTestRunner != NULL && listener != NULL){
        mTestRunner->AddTestListener(listener);
    }
}

AutoPtr<IList> CInstrumentationTestRunner::GetBuilderRequirements()
{
    AutoPtr<IArrayList> requirements;
    CArrayList::New((IArrayList**)&requirements);
    return (IList*)requirements.Get();
}

void CInstrumentationTestRunner::ParseTestClasses(
    /* [in] */ const String& testPackage,
    /* [in] */ const String& testClassArg,
    /* [in] */ CTestSuiteBuilder* testSuiteBuilder)
{
    AutoPtr< ArrayOf<String> > testClasses;
    StringUtils::Split(testClassArg, String(","), (ArrayOf<String>**)&testClasses);
    if (testClasses != NULL) {
        for (Int32 i = 0; i < testClasses->GetLength(); i++) {
            String testClass = (*testClasses)[i];
            ParseTestClass(testPackage, testClass, testSuiteBuilder);
        }
    }
}

void CInstrumentationTestRunner::ParseTestClass(
    /* [in] */ const String& testPackage,
    /* [in] */ const String& className,
    /* [in] */ CTestSuiteBuilder* testSuiteBuilder)
{
    Int32 methodSeparatorIndex = className.IndexOf('#');
    String testMethodName, testClassName;

    testClassName = className;
    if (methodSeparatorIndex > 0) {
        testMethodName = testClassName.Substring(methodSeparatorIndex + 1);
        testClassName = testClassName.Substring(0, methodSeparatorIndex);
    }
    AutoPtr<IContext> context;
    GetTargetContext((IContext**)&context);
    testSuiteBuilder->AddTestClassByName(testPackage, testClassName, testMethodName, context);
}

AutoPtr<CAndroidTestRunner> CInstrumentationTestRunner::GetAndroidTestRunner()
{
    AutoPtr<CAndroidTestRunner> runner;
    CAndroidTestRunner::NewByFriend((CAndroidTestRunner**)&runner);
    return runner;
}

Boolean CInstrumentationTestRunner::GetBooleanArgument(
    /* [in] */ IBundle* arguments,
    /* [in] */ const String& tag)
{
    String tagString;
    arguments->GetString(tag, &tagString);
    return !tagString.IsNull() && tagString.EqualsIgnoreCase("TRUE");
}

AutoPtr<IPredicate> CInstrumentationTestRunner::GetSizePredicateFromArg(
    /* [in] */ const String& sizeArg)
{
    // TODO:
    // if (SMALL_SUITE.equals(sizeArg)) {
    //     return TestPredicates.SELECT_SMALL;
    // } else if (MEDIUM_SUITE.equals(sizeArg)) {
    //     return TestPredicates.SELECT_MEDIUM;
    // } else if (LARGE_SUITE.equals(sizeArg)) {
    //     return TestPredicates.SELECT_LARGE;
    // } else {
    //     return null;
    // }
    return NULL;
}

AutoPtr<IPredicate> CInstrumentationTestRunner::GetAnnotationPredicate(
    /* [in] */ const String& annotationClassName)
{
    // TODO:
    // Class<? extends Annotation> annotationClass = getAnnotationClass(annotationClassName);
    // if (annotationClass != null) {
    //     return new HasAnnotation(annotationClass);
    // }
    // return null;
    return NULL;
}

AutoPtr<IPredicate> CInstrumentationTestRunner::GetNotAnnotationPredicate(
    /* [in] */ const String& annotationClassName)
{
    // TODO:
    // Class<? extends Annotation> annotationClass = getAnnotationClass(annotationClassName);
    // if (annotationClass != null) {
    //     return Predicates.not(new HasAnnotation(annotationClass));
    // }
    // return null;
    return NULL;
}

// private Class<? extends Annotation> getAnnotationClass(String annotationClassName) {
//     if (annotationClassName == null) {
//         return null;
//     }
//     try {
//        Class<?> annotationClass = Class.forName(annotationClassName);
//        if (annotationClass.isAnnotation()) {
//            return (Class<? extends Annotation>)annotationClass;
//        } else {
//            Log.e(TAG, String.format("Provided annotation value %s is not an Annotation",
//                    annotationClassName));
//        }
//     } catch (ClassNotFoundException e) {
//         Log.e(TAG, String.format("Could not find class for specified annotation %s",
//                 annotationClassName));
//     }
//     return null;
// }

void CInstrumentationTestRunner::PrepareLooper()
{
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    helper->Prepare();
}

ECode CInstrumentationTestRunner::OnStart()
{
    PrepareLooper();

    if (mJustCount) {
        mResults->PutString(IInstrumentation::REPORT_KEY_IDENTIFIER, IInstrumentationTestRunner::REPORT_VALUE_ID);
        mResults->PutInt32(IInstrumentationTestRunner::REPORT_KEY_NUM_TOTAL, mTestCount);
        Finish(IActivity::RESULT_OK, mResults);
    }
    else {
        if (mDebug) {
            // TODO:
            // Debug.waitForDebugger();
        }

        AutoPtr<IByteArrayOutputStream> byteArrayOutputStream;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&byteArrayOutputStream);
        AutoPtr<IPrintStream> writer;
        CPrintStream::New(IOutputStream::Probe(byteArrayOutputStream), (IPrintStream**)&writer);
        // try {
        AutoPtr<StringResultPrinter> resultPrinter = new StringResultPrinter(writer);

        mTestRunner->AddTestListener(resultPrinter);

        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 startTime;
        system->GetCurrentTimeMillis(&startTime);
        ECode ec = mTestRunner->RunTest();
        if (FAILED(ec)) {
            // TODO:
            // writer.println(String.format("Test run aborted due to unexpected exception: %s",
            //             t.getMessage()));
            // t.printStackTrace(writer);
        }
        Int64 runTime;
        system->GetCurrentTimeMillis(&runTime);
        runTime = runTime - startTime;

        AutoPtr<ITestResult> testResult;
        mTestRunner->GetTestResult((ITestResult**)&testResult);
        resultPrinter->Print(testResult, runTime);
        // } catch (Throwable t) {
        //     // catch all exceptions so a more verbose error message can be outputted
        //     writer.println(String.format("Test run aborted due to unexpected exception: %s",
        //                     t.getMessage()));
        //     t.printStackTrace(writer);
        // } finally {
        String testClassName;
        mTestRunner->GetTestClassName(&testClassName);
        String byteArrayOutputStreamStr;
        byteArrayOutputStream->ToString(&byteArrayOutputStreamStr);
        String value("");
        value.AppendFormat("\nTest results for %s=%s", testClassName.string(), byteArrayOutputStreamStr.string());
        mResults->PutString(IInstrumentation::REPORT_KEY_STREAMRESULT, value);

        if (mCoverage) {
            GenerateCoverageReport();
        }
        ICloseable::Probe(writer)->Close();

        Finish(IActivity::RESULT_OK, mResults);
        // }
    }
    return NOERROR;
}

ECode CInstrumentationTestRunner::GetTestSuite(
    /* [out] */ ITestSuite** suite)
{
    return GetAllTests(suite);
}

ECode CInstrumentationTestRunner::GetAllTests(
    /* [out] */ ITestSuite** suite)
{
    VALIDATE_NOT_NULL(suite);
    *suite = NULL;
    return NOERROR;
}

ECode CInstrumentationTestRunner::GetLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader);
    *loader = NULL;
    return NOERROR;
}

void CInstrumentationTestRunner::GenerateCoverageReport()
{
    // TODO:
    // use reflection to call emma dump coverage method, to avoid
    // always statically compiling against emma jar
//    String coverageFilePath = getCoverageFilePath();
//    java.io.File coverageFile = new java.io.File(coverageFilePath);
//    try {
//        Class<?> emmaRTClass = Class.forName("com.vladium.emma.rt.RT");
//        Method dumpCoverageMethod = emmaRTClass.getMethod("dumpCoverageData",
//                coverageFile.getClass(), boolean.class, boolean.class);
//
//        dumpCoverageMethod.invoke(null, coverageFile, false, false);
//        // output path to generated coverage file so it can be parsed by a test harness if
//        // needed
//        mResults.putString(REPORT_KEY_COVERAGE_PATH, coverageFilePath);
//        // also output a more user friendly msg
//        final String currentStream = mResults.getString(
//                Instrumentation.REPORT_KEY_STREAMRESULT);
//        mResults.putString(Instrumentation.REPORT_KEY_STREAMRESULT,
//            String.format("%s\nGenerated code coverage data to %s", currentStream,
//            coverageFilePath));
//    } catch (ClassNotFoundException e) {
//        reportEmmaError("Is emma jar on classpath?", e);
//    } catch (SecurityException e) {
//        reportEmmaError(e);
//    } catch (NoSuchMethodException e) {
//        reportEmmaError(e);
//    } catch (IllegalArgumentException e) {
//        reportEmmaError(e);
//    } catch (IllegalAccessException e) {
//        reportEmmaError(e);
//    } catch (InvocationTargetException e) {
//        reportEmmaError(e);
//    }
}

String CInstrumentationTestRunner::GetCoverageFilePath()
{
    if (mCoverageFilePath.IsNull()) {
        AutoPtr<IContext> context;
        GetTargetContext((IContext**)&context);
        AutoPtr<IFile> dir;
        context->GetFilesDir((IFile**)&dir);
        String dirPath;
        dir->GetAbsolutePath(&dirPath);

        AutoPtr<IFileHelper> helper;
        CFileHelper::AcquireSingleton((IFileHelper**)&helper);
        String separator;
        helper->GetSeparator(&separator);
        return dirPath + separator + DEFAULT_COVERAGE_FILE_NAME;
    }
    else {
        return mCoverageFilePath;
    }
}

void CInstrumentationTestRunner::ReportEmmaError(
    /* [in] */ ECode ec)
{
    return ReportEmmaError(String(""), ec);
}

void CInstrumentationTestRunner::ReportEmmaError(
    /* [in] */ const String& hint,
    /* [in] */ ECode ec)
{
    String msg = String("Failed to generate emma coverage. ") + hint;
    Logger::E(TAG, "%s 0x%08x", msg.string(), ec);
    mResults->PutString(IInstrumentation::REPORT_KEY_STREAMRESULT, String("\nError: ") + msg);
}

ECode CInstrumentationTestRunner::Start()
{
    return Instrumentation::Start();
}

ECode CInstrumentationTestRunner::OnException(
    /* [in] */ IInterface* obj,
    /* [in] */ ECode e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return Instrumentation::OnException(obj, e, result);
}

ECode CInstrumentationTestRunner::SendStatus(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    return Instrumentation::SendStatus(resultCode, results);
}

ECode CInstrumentationTestRunner::Finish(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    return Instrumentation::Finish(resultCode, results);
}

ECode CInstrumentationTestRunner::SetAutomaticPerformanceSnapshots()
{
    return Instrumentation::SetAutomaticPerformanceSnapshots();
}

ECode CInstrumentationTestRunner::StartPerformanceSnapshot()
{
    return Instrumentation::StartPerformanceSnapshot();
}

ECode CInstrumentationTestRunner::EndPerformanceSnapshot()
{
    return Instrumentation::EndPerformanceSnapshot();
}

ECode CInstrumentationTestRunner::OnDestroy()
{
    return Instrumentation::OnDestroy();
}

ECode CInstrumentationTestRunner::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    return Instrumentation::GetContext(context);
}

ECode CInstrumentationTestRunner::CInstrumentationTestRunner::GetComponentName(
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component);
    return Instrumentation::GetComponentName(component);
}

ECode CInstrumentationTestRunner::GetTargetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    return Instrumentation::GetTargetContext(context);
}

ECode CInstrumentationTestRunner::IsProfiling(
    /* [out] */ Boolean* isProfiling)
{
    VALIDATE_NOT_NULL(isProfiling);
    return Instrumentation::IsProfiling(isProfiling);
}

ECode CInstrumentationTestRunner::StartProfiling()
{
    return Instrumentation::StartProfiling();
}

ECode CInstrumentationTestRunner::StopProfiling()
{
    return Instrumentation::StopProfiling();
}

ECode CInstrumentationTestRunner::SetInTouchMode(
    /* [in] */ Boolean inTouch)
{
    return Instrumentation::SetInTouchMode(inTouch);
}

ECode CInstrumentationTestRunner::WaitForIdle(
    /* [in] */ IRunnable* recipient)
{
    return Instrumentation::WaitForIdle(recipient);
}

ECode CInstrumentationTestRunner::WaitForIdleSync()
{
    return Instrumentation::WaitForIdleSync();
}

ECode CInstrumentationTestRunner::RunOnMainSync(
    /* [in] */ IRunnable* runner)
{
    return Instrumentation::RunOnMainSync(runner);
}

ECode CInstrumentationTestRunner::StartActivitySync(
    /* [in] */ IIntent* intent,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    return Instrumentation::StartActivitySync(intent, activity);
}

ECode CInstrumentationTestRunner::AddMonitor(
    /* [in] */ IInstrumentationActivityMonitor* monitor)
{
    return Instrumentation::AddMonitor(monitor);
}

ECode CInstrumentationTestRunner::AddMonitor(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ IInstrumentationActivityResult* result,
    /* [in] */ Boolean block,
    /* [out] */ IInstrumentationActivityMonitor** monitor)
{
    VALIDATE_NOT_NULL(monitor);
    return Instrumentation::AddMonitor(filter, result, block, monitor);
}

ECode CInstrumentationTestRunner::AddMonitor(
    /* [in] */ const String& cls,
    /* [in] */ IInstrumentationActivityResult* result,
    /* [in] */ Boolean block,
    /* [out] */ IInstrumentationActivityMonitor** monitor)
{
    VALIDATE_NOT_NULL(monitor);
    return Instrumentation::AddMonitor(cls, result, block, monitor);
}

ECode CInstrumentationTestRunner::CheckMonitorHit(
    /* [in] */ IInstrumentationActivityMonitor* monitor,
    /* [in] */ Int32 minHits,
    /* [out] */ Boolean* reached)
{
    VALIDATE_NOT_NULL(reached);
    return Instrumentation::CheckMonitorHit(monitor, minHits, reached);
}

ECode CInstrumentationTestRunner::WaitForMonitor(
    /* [in] */ IInstrumentationActivityMonitor* monitor,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    return Instrumentation::WaitForMonitor(monitor, activity);
}

ECode CInstrumentationTestRunner::WaitForMonitorWithTimeout(
    /* [in] */ IInstrumentationActivityMonitor* monitor,
    /* [in] */ Int64 timeOut,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    return Instrumentation::WaitForMonitorWithTimeout(monitor, timeOut, activity);
}

ECode CInstrumentationTestRunner::RemoveMonitor(
    /* [in] */ IInstrumentationActivityMonitor* monitor)
{
    return Instrumentation::RemoveMonitor(monitor);
}

ECode CInstrumentationTestRunner::InvokeMenuActionSync(
    /* [in] */ IActivity* targetActivity,
    /* [in] */ Int32 id,
    /* [in] */ Int32 flag,
    /* [out] */ Boolean* success)
{
    VALIDATE_NOT_NULL(success);
    return Instrumentation::InvokeMenuActionSync(targetActivity, id, flag, success);
}

ECode CInstrumentationTestRunner::InvokeContextMenuAction(
    /* [in] */ IActivity* targetActivity,
    /* [in] */ Int32 id,
    /* [in] */ Int32 flag,
    /* [out] */ Boolean* success)
{
    VALIDATE_NOT_NULL(success);
    return Instrumentation::InvokeContextMenuAction(targetActivity, id, flag, success);
}

ECode CInstrumentationTestRunner::SendStringSync(
    /* [in] */ const String& text)
{
    return Instrumentation::SendStringSync(text);
}

ECode CInstrumentationTestRunner::SendKeySync(
    /* [in] */ IKeyEvent* event)
{
    return Instrumentation::SendKeySync(event);
}

ECode CInstrumentationTestRunner::SendKeyDownUpSync(
    /* [in] */ Int32 key)
{
    return Instrumentation::SendKeyDownUpSync(key);
}

ECode CInstrumentationTestRunner::SendCharacterSync(
    /* [in] */ Int32 keyCode)
{
    return Instrumentation::SendCharacterSync(keyCode);
}

ECode CInstrumentationTestRunner::SendPointerSync(
    /* [in] */ IMotionEvent* event)
{
    return Instrumentation::SendPointerSync(event);
}

ECode CInstrumentationTestRunner::SendTrackballEventSync(
    /* [in] */ IMotionEvent* event)
{
    return Instrumentation::SendTrackballEventSync(event);
}

ECode CInstrumentationTestRunner::NewApplication(
    /* [in] */ IClassLoader* cl,
    /* [in] */ const String& className,
    /* [in] */ IContext* ctx,
    /* [out] */ IApplication** app)
{
    VALIDATE_NOT_NULL(app);
    return Instrumentation::NewApplication(cl, className, ctx, app);
}

ECode CInstrumentationTestRunner::CallApplicationOnCreate(
    /* [in] */ IApplication* app)
{
    return Instrumentation::CallApplicationOnCreate(app);
}

ECode CInstrumentationTestRunner::NewActivity(
    /* [in] */ IClassInfo* clazz,
    /* [in] */ IContext* ctx,
    /* [in] */ IBinder* token,
    /* [in] */ IApplication* application,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* info,
    /* [in] */ ICharSequence* title,
    /* [in] */ IActivity* parent,
    /* [in] */ const String& id,
    /* [in] */ IInterface* lastNonConfigurationInstance,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    // TODO:
    return Instrumentation::NewActivity(clazz, ctx, token, application, intent, info, title,
            parent, id, NULL/*lastNonConfigurationInstance*/, activity);
}

ECode CInstrumentationTestRunner::NewActivity(
    /* [in] */ IClassLoader* cl,
    /* [in] */ const String& className,
    /* [in] */ IIntent* intent,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    return Instrumentation::NewActivity(cl, className, intent, activity);
}

ECode CInstrumentationTestRunner::CallActivityOnCreate(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* icicle)
{
    return Instrumentation::CallActivityOnCreate(activity, icicle);
}

ECode CInstrumentationTestRunner::CallActivityOnDestroy(
    /* [in] */ IActivity* activity)
{
    return Instrumentation::CallActivityOnDestroy(activity);
}

ECode CInstrumentationTestRunner::CallActivityOnRestoreInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* savedInstanceState)
{
    return Instrumentation::CallActivityOnRestoreInstanceState(activity, savedInstanceState);
}

ECode CInstrumentationTestRunner::CallActivityOnPostCreate(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* icicle)
{
    return Instrumentation::CallActivityOnPostCreate(activity, icicle);
}

ECode CInstrumentationTestRunner::CallActivityOnNewIntent(
    /* [in] */ IActivity* activity,
    /* [in] */ IIntent* intent)
{
    return Instrumentation::CallActivityOnNewIntent(activity, intent);
}

ECode CInstrumentationTestRunner::CallActivityOnStart(
    /* [in] */ IActivity* activity)
{
    return Instrumentation::CallActivityOnStart(activity);
}

ECode CInstrumentationTestRunner::CallActivityOnRestart(
    /* [in] */ IActivity* activity)
{
    return Instrumentation::CallActivityOnRestart(activity);
}

ECode CInstrumentationTestRunner::CallActivityOnResume(
    /* [in] */ IActivity* activity)
{
    return Instrumentation::CallActivityOnResume(activity);
}

ECode CInstrumentationTestRunner::CallActivityOnStop(
    /* [in] */ IActivity* activity)
{
    return Instrumentation::CallActivityOnStop(activity);
}

ECode CInstrumentationTestRunner::CallActivityOnSaveInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* outState)
{
    return Instrumentation::CallActivityOnSaveInstanceState(activity, outState);
}

ECode CInstrumentationTestRunner::CallActivityOnPause(
    /* [in] */ IActivity* activity)
{
    return Instrumentation::CallActivityOnPause(activity);
}

ECode CInstrumentationTestRunner::CallActivityOnUserLeaving(
    /* [in] */ IActivity* activity)
{
    return Instrumentation::CallActivityOnUserLeaving(activity);
}

ECode CInstrumentationTestRunner::StartAllocCounting()
{
    return Instrumentation::StartAllocCounting();
}

ECode CInstrumentationTestRunner::StopAllocCounting()
{
    return Instrumentation::StopAllocCounting();
}

ECode CInstrumentationTestRunner::GetAllocCounts(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    return Instrumentation::GetAllocCounts(bundle);
}

ECode CInstrumentationTestRunner::GetBinderCounts(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    return Instrumentation::GetBinderCounts(bundle);
}

ECode CInstrumentationTestRunner::ExecStartActivity(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [out] */ IInstrumentationActivityResult** result)
{
    VALIDATE_NOT_NULL(result);
    return Instrumentation::ExecStartActivity(who, contextThread, token,
            target, intent, requestCode, options, result);
}

ECode CInstrumentationTestRunner::ExecStartActivities(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options)
{
    return Instrumentation::ExecStartActivities(who, contextThread, token,
            target, intents, options);
}

ECode CInstrumentationTestRunner::ExecStartActivitiesAsUser(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId)
{
    return Instrumentation::ExecStartActivitiesAsUser(who, contextThread, token,
            target, intents, options, userId);
}

ECode CInstrumentationTestRunner::ExecStartActivity(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IFragment* target,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [out] */ IInstrumentationActivityResult** activityResult)
{
    VALIDATE_NOT_NULL(activityResult);
    return Instrumentation::ExecStartActivity(who, contextThread, token, target,
            intent, requestCode, options, activityResult);
}

ECode CInstrumentationTestRunner::ExecStartActivity(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* user,
    /* [out] */ IInstrumentationActivityResult** activityResult)
{
    VALIDATE_NOT_NULL(activityResult);
    return Instrumentation::ExecStartActivity(who, contextThread, token, target,
            intent, requestCode, options, user, activityResult);
}

ECode CInstrumentationTestRunner::Init(
    /* [in] */ IActivityThread* thread,
    /* [in] */ IContext* instrContext,
    /* [in] */ IContext* appContext,
    /* [in] */ IComponentName* component,
    /* [in] */ IInstrumentationWatcher* watcher)
{
    // TODO:
    return Instrumentation::Init(thread, instrContext, appContext, component, watcher, NULL);
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
