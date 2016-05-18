
#ifndef __CANDROIDTESTRUNNER_H__
#define __CANDROIDTESTRUNNER_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Test_CAndroidTestRunner.h"
#include <_Eunit.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Eunit::Framework::ITest;
using Eunit::Framework::ITestListener;
using Eunit::Framework::ITestResult;
using Eunit::Framework::ITestCase;
using Eunit::Runner::ITestSuiteLoader;
using Eunit::Runner::IBaseTestRunner;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Droid::App::IInstrumentation;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPerformanceResultsWriter;

namespace Elastos {
namespace Droid {
namespace Test {

CarClass(CAndroidTestRunner)
    , public Object
    , public IBaseTestRunner
    , public IAndroidTestRunner
{
public:
    CAndroidTestRunner();

    CAR_INTERFACE_DECL()

    CARAPI SetPreference(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI TestStarted(
        /* [in] */ const String& testName);

    CARAPI TestEnded(
        /* [in] */ const String& testName);

    CARAPI TestFailed(
        /* [in] */ Int32 status,
        /* [in] */ ITest* test,
        /* [in] */ ECode ec);

    /**
     * Returns the formatted string of the elapsed time.
     */
    CARAPI ElapsedTimeAsString(
        /* [in] */ Int64 runTime,
        /* [out] */ String* timeStr);

    /**
     * Sets the loading behaviour of the test runner
     */
    CARAPI SetLoading(
        /* [in] */ Boolean bEnable);

    /**
     * Extract the class name from a String in VA/Java style
     */
    CARAPI ExtractClassName(
        /* [in] */ const String& className,
        /* [out] */ String* result);

    /**
     * Returns the loader to be used.
     *
     * @deprecated not present in JUnit4.10
     */
    CARAPI GetLoader(
        /* [out] */ ITestSuiteLoader** loader);

    CARAPI SetTestClassName(
        /* [in] */ const String& testPackageName,
        /* [in] */ const String& testClassName,
        /* [in] */ const String& testMethodName);

    CARAPI SetTest(
        /* [in] */ ITest* test);

    CARAPI ClearTestListeners();

    CARAPI AddTestListener(
        /* [in] */ ITestListener* testListener);

    CARAPI GetTestCases(
        /* [out] */ IList** testCases);

    CARAPI GetTestClassName(
        /* [out] */ String* className);

    CARAPI GetTestResult(
        /* [out] */ ITestResult** result);

    CARAPI RunTest();

    CARAPI RunTest(
        /* [in] */ ITestResult* testResult);

    CARAPI SetContext(
        /* [in] */ IContext* context);

    CARAPI SetInstrumentation(
        /* [in] */ IInstrumentation* instrumentation);

    CARAPI SetPerformanceResultsWriter(
        /* [in] */ IPerformanceResultsWriter* writer);

    CARAPI_(void) SetSkipExecution(
        /* [in] */ Boolean skip);

protected:
    /**
     * Clears the status message.
     */
    CARAPI_(void) ClearStatus() { // Belongs in the GUI TestRunner class
    }

    CARAPI_(AutoPtr<ITestResult>) CreateTestResult();

private:
    CARAPI LoadTestClass(
        /* [in] */ const String& testPackageName,
        /* [in] */ const String& testClassName,
        /* [out] */ IClassInfo** clazz);

    CARAPI_(AutoPtr<ITestCase>) BuildSingleTestMethod(
        /* [in] */ IClassInfo* testClass,
        /* [in] */ const String& testMethodName);

    CARAPI NewSingleTestMethod(
        /* [in] */ IClassInfo* testClass,
        /* [in] */ const String& testMethodName,
        /* [in] */ IConstructorInfo* constructor,
        /* [out] */ ITestCase** testCase);

    CARAPI NewSingleTestMethod(
        /* [in] */ IClassInfo* testClass,
        /* [in] */ const String& testMethodName,
        /* [in] */ IConstructorInfo* constructor,
        /* [in] */ const String& arg,
        /* [out] */ ITestCase** testCase);

    CARAPI_(Boolean) ShouldRunSingleTestMethod(
        /* [in] */ const String& testMethodName,
        /* [in] */ IClassInfo* testClass);

    CARAPI GetTest(
        /* [in] */ IClassInfo* clazz,
        /* [out] */ ITest** test);

    CARAPI_(void) SetContextIfAndroidTestCase(
        /* [in] */ ITest* test,
        /* [in] */ IContext* context,
        /* [in] */ IContext* testContext);

    CARAPI_(void) SetInstrumentationIfInstrumentationTestCase(
        /* [in] */ ITest* test,
        /* [in] */ IInstrumentation* instrumentation);

    CARAPI_(void) SetPerformanceWriterIfPerformanceCollectorTestCase(
        /* [in] */ ITest* test,
        /* [in] */ IPerformanceResultsWriter* writer);

private:
    static const String TAG;
    AutoPtr<ITestResult> mTestResult;
    String mTestClassName;
    AutoPtr<IList> mTestCases;
    AutoPtr<IContext> mContext;
    Boolean mSkipExecution;

    List< AutoPtr<ITestListener> > mTestListeners;
    AutoPtr<IInstrumentation> mInstrumentation;
    AutoPtr<IPerformanceResultsWriter> mPerfWriter;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__CANDROIDTESTRUNNER_H__
