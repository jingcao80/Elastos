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

#ifndef __ELASTOS_DROID_TEST_CINSTRUMENTATIONTESTRUNNER_H__
#define __ELASTOS_DROID_TEST_CINSTRUMENTATIONTESTRUNNER_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_Droid_Test_CInstrumentationTestRunner.h"
#include "CAndroidTestRunner.h"
#include "suitebuilder/CTestSuiteBuilder.h"
#include <elastos/droid/app/Instrumentation.h>
#include <eunit/textui/ResultPrinter.h>

using Elastos::Utility::IList;
using Eunit::Framework::ITestSuite;
using Eunit::Framework::ITestListener;
using Eunit::Framework::ITestFailure;
using Eunit::Textui::ResultPrinter;
using Elastos::Droid::App::Instrumentation;
using Elastos::Droid::App::IInstrumentation;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::Os::IPerformanceResultsWriter;
using Elastos::Droid::Internal::Utility::IPredicate;
using Elastos::Droid::Test::SuiteBuilder::CTestSuiteBuilder;

namespace Elastos {
namespace Droid {
namespace Test {

class CInstrumentationTestRunner
    : public Instrumentation
    , public IInstrumentationTestRunner
    , public ITestSuiteProvider
{
private:
    class StringResultPrinter : public ResultPrinter
    {
    public:
        StringResultPrinter(
            /* [in] */ IPrintStream* writer)
            : ResultPrinter(writer)
        {}

        CARAPI_(void) Print(
            /* [in] */ ITestResult* result,
            /* [in] */ Int64 runTime);
    };

    /**
     * This class sends status reports back to the IInstrumentationWatcher about
     * which suite each test belongs.
     */
    class SuiteAssignmentPrinter
        : public Object
        , public ITestListener
    {
    public:
        SuiteAssignmentPrinter(
            /* [in] */ CInstrumentationTestRunner* host)
            : mStartTime(0)
            , mEndTime(0)
            , mTimingValid(FALSE)
            , mHost(host)
        {}

        CAR_INTERFACE_DECL()

        /**
         * send a status for the start of a each test, so long tests can be seen as "running"
         */
        CARAPI StartTest(
            /* [in] */ ITest* test);

        /**
         * @see junit.framework.TestListener#addError(Test, Throwable)
         */
        CARAPI AddError(
            /* [in] */ ITest* test,
            /* [in] */ ECode ec);

        /**
         * @see junit.framework.TestListener#addFailure(Test, AssertionFailedError)
         */
        CARAPI AddFailure(
            /* [in] */ ITest* test,
            /* [in] */ ITestFailure* failure);

        /**
         * @see junit.framework.TestListener#endTest(Test)
         */
        CARAPI EndTest(
            /* [in] */ ITest* test);

    private:
        AutoPtr<IBundle> mTestResult;
        Int64 mStartTime;
        Int64 mEndTime;
        Boolean mTimingValid;
        CInstrumentationTestRunner* mHost;
    };

    /**
     * This class sends status reports back to the IInstrumentationWatcher
     */
    class WatcherResultPrinter
        : public Object
        , public ITestListener
        , public IPerformanceResultsWriter
    {
    public:
        WatcherResultPrinter(
            /* [in] */ Int32 numTests,
            /* [in] */ CInstrumentationTestRunner* host);

        CAR_INTERFACE_DECL()

        CARAPI StartTest(
            /* [in] */ ITest* test);

        CARAPI AddError(
            /* [in] */ ITest* test,
            /* [in] */ ECode ec);

        CARAPI AddFailure(
            /* [in] */ ITest* test,
            /* [in] */ ITestFailure* failure);

        CARAPI EndTest(
            /* [in] */ ITest* test);

        CARAPI WriteBeginSnapshot(
            /* [in] */ const String& label);

        CARAPI WriteEndSnapshot(
            /* [in] */ IBundle* results);

        CARAPI WriteStartTiming(
            /* [in] */ const String& label);

        CARAPI WriteStopTiming(
            /* [in] */ IBundle* results);

        CARAPI WriteMeasurement(
            /* [in] */ const String& label,
            /* [in] */ Int64 value);

        CARAPI WriteMeasurement(
            /* [in] */ const String& label,
            /* [in] */ Float value);

        CARAPI WriteMeasurement(
            /* [in] */ const String& label,
            /* [in] */ const String& value);

    public:
        AutoPtr<IBundle> mTestResult;
        Int32 mTestNum;
        Int32 mTestResultCode;
        String mTestClass;
        // PerformanceCollector mPerfCollector = new PerformanceCollector();
        Boolean mIsTimedTest;
        Boolean mIncludeDetailedStats;

    private:
        AutoPtr<IBundle> mResultTemplate;
        CInstrumentationTestRunner* mHost;
    };

    class TestMethodPredicate
        : public Object
        , public IPredicate
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI Apply(
            /* [in] */ IInterface* t,
            /* [out] */ Boolean* result);
    };

public:
    friend class WatcherResultPrinter;

    CInstrumentationTestRunner();

    CAR_INTERFACE_DECL()

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean * result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI OnCreate(
        /* [in] */ IBundle* arguments);

    /**
     * Get the Bundle object that contains the arguments
     *
     * @return the Bundle object
     * @hide
     */
    CARAPI GetBundle(
        /* [out] */ IBundle** bundle);

    virtual CARAPI_(AutoPtr<IList>) GetBuilderRequirements();

    /**
     * Initialize the current thread as a looper.
     * <p/>
     * Exposed for unit testing.
     */
    virtual CARAPI_(void) PrepareLooper();

    CARAPI OnStart();

    CARAPI GetTestSuite(
        /* [out] */ ITestSuite** suite);

    CARAPI GetAllTests(
        /* [out] */ ITestSuite** suite);

    CARAPI GetLoader(
        /* [out] */ IClassLoader** loader);

    CARAPI Start();

    CARAPI OnException(
        /* [in] */ IInterface* obj,
        /* [in] */ ECode e,
        /* [out] */ Boolean* result);

    CARAPI SendStatus(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI Finish(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI SetAutomaticPerformanceSnapshots();

    CARAPI StartPerformanceSnapshot();

    CARAPI EndPerformanceSnapshot();

    CARAPI OnDestroy();

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI GetComponentName(
        /* [out] */ IComponentName** component);

    CARAPI GetTargetContext(
        /* [out] */ IContext** context);

    CARAPI IsProfiling(
        /* [out] */ Boolean* isProfiling);

    CARAPI StartProfiling();

    CARAPI StopProfiling();

    CARAPI SetInTouchMode(
        /* [in] */ Boolean inTouch);

    CARAPI WaitForIdle(
        /* [in] */ IRunnable* recipient);

    CARAPI WaitForIdleSync();

    CARAPI RunOnMainSync(
        /* [in] */ IRunnable* runner);

    CARAPI StartActivitySync(
        /* [in] */ IIntent* intent,
        /* [out] */ IActivity** activity);

    CARAPI AddMonitor(
        /* [in] */ IInstrumentationActivityMonitor* monitor);

    CARAPI AddMonitor(
        /* [in] */ IIntentFilter* filter,
        /* [in] */ IInstrumentationActivityResult* result,
        /* [in] */ Boolean block,
        /* [out] */ IInstrumentationActivityMonitor** monitor);

    CARAPI AddMonitor(
        /* [in] */ const String& cls,
        /* [in] */ IInstrumentationActivityResult* result,
        /* [in] */ Boolean block,
        /* [out] */ IInstrumentationActivityMonitor** monitor);

    CARAPI CheckMonitorHit(
        /* [in] */ IInstrumentationActivityMonitor* monitor,
        /* [in] */ Int32 minHits,
        /* [out] */ Boolean* reached);

    CARAPI WaitForMonitor(
        /* [in] */ IInstrumentationActivityMonitor* monitor,
        /* [out] */ IActivity** activity);

    CARAPI WaitForMonitorWithTimeout(
        /* [in] */ IInstrumentationActivityMonitor* monitor,
        /* [in] */ Int64 timeOut,
        /* [out] */ IActivity** activity);

    CARAPI RemoveMonitor(
        /* [in] */ IInstrumentationActivityMonitor* monitor);

    CARAPI InvokeMenuActionSync(
        /* [in] */ IActivity* targetActivity,
        /* [in] */ Int32 id,
        /* [in] */ Int32 flag,
        /* [out] */ Boolean* success);

    CARAPI InvokeContextMenuAction(
        /* [in] */ IActivity* targetActivity,
        /* [in] */ Int32 id,
        /* [in] */ Int32 flag,
        /* [out] */ Boolean* success);

    CARAPI SendStringSync(
        /* [in] */ const String& text);

    CARAPI SendKeySync(
        /* [in] */ IKeyEvent* event);

    CARAPI SendKeyDownUpSync(
        /* [in] */ Int32 key);

    CARAPI SendCharacterSync(
        /* [in] */ Int32 keyCode);

    CARAPI SendPointerSync(
        /* [in] */ IMotionEvent* event);

    CARAPI SendTrackballEventSync(
        /* [in] */ IMotionEvent* event);

    CARAPI NewApplication(
        /* [in] */ IClassLoader* cl,
        /* [in] */ const String& className,
        /* [in] */ IContext* ctx,
        /* [out] */ IApplication** app);

    CARAPI CallApplicationOnCreate(
        /* [in] */ IApplication* app);

    CARAPI NewActivity(
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
        /* [out] */ IActivity** activity);

    CARAPI NewActivity(
        /* [in] */ IClassLoader* cl,
        /* [in] */ const String& className,
        /* [in] */ IIntent* intent,
        /* [out] */ IActivity** activity);

    CARAPI CallActivityOnCreate(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* icicle);

    CARAPI CallActivityOnDestroy(
        /* [in] */ IActivity* activity);

    CARAPI CallActivityOnRestoreInstanceState(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* savedInstanceState);

    CARAPI CallActivityOnPostCreate(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* icicle);

    CARAPI CallActivityOnNewIntent(
        /* [in] */ IActivity* activity,
        /* [in] */ IIntent* intent);

    CARAPI CallActivityOnStart(
        /* [in] */ IActivity* activity);

    CARAPI CallActivityOnRestart(
        /* [in] */ IActivity* activity);

    CARAPI CallActivityOnResume(
        /* [in] */ IActivity* activity);

    CARAPI CallActivityOnStop(
        /* [in] */ IActivity* activity);

    CARAPI CallActivityOnSaveInstanceState(
        /* [in] */ IActivity* activity,
        /* [in] */ IBundle* outState);

    CARAPI CallActivityOnPause(
        /* [in] */ IActivity* activity);

    CARAPI CallActivityOnUserLeaving(
        /* [in] */ IActivity* activity);

    CARAPI StartAllocCounting();

    CARAPI StopAllocCounting();

    CARAPI GetAllocCounts(
        /* [out] */ IBundle** bundle);

    CARAPI GetBinderCounts(
        /* [out] */ IBundle** bundle);

    CARAPI ExecStartActivity(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [out] */ IInstrumentationActivityResult** result);

    CARAPI ExecStartActivities(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options);

    CARAPI ExecStartActivitiesAsUser(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId);

    CARAPI ExecStartActivity(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IFragment* target,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [out] */ IInstrumentationActivityResult** activityResult);

    CARAPI ExecStartActivity(
        /* [in] */ IContext* who,
        /* [in] */ IBinder* contextThread,
        /* [in] */ IBinder* token,
        /* [in] */ IActivity* target,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestCode,
        /* [in] */ IBundle* options,
        /* [in] */ IUserHandle* user,
        /* [out] */ IInstrumentationActivityResult** activityResult);

    CARAPI Init(
        /* [in] */ IActivityThread* thread,
        /* [in] */ IContext* instrContext,
        /* [in] */ IContext* appContext,
        /* [in] */ IComponentName* component,
        /* [in] */ IInstrumentationWatcher* watcher);

protected:
    /**
     * Add a {@link TestListener}
     * @hide
     */
    virtual CARAPI_(void) AddTestListener(
        /* [in] */ ITestListener* listener);

    virtual CARAPI_(AutoPtr<CAndroidTestRunner>) GetAndroidTestRunner();

private:
    /**
     * Parses and loads the specified set of test classes
     *
     * @param testClassArg - comma-separated list of test classes and methods
     * @param testSuiteBuilder - builder to add tests to
     */
    CARAPI_(void) ParseTestClasses(
        /* [in] */ const String& testPackage,
        /* [in] */ const String& testClassArg,
        /* [in] */ CTestSuiteBuilder* testSuiteBuilder);

    /**
     * Parse and load the given test class and, optionally, method
     *
     * @param testClassName - full package name of test class and optionally method to add.
     *        Expected format: com.android.TestClass#testMethod
     * @param testSuiteBuilder - builder to add tests to
     */
    CARAPI_(void) ParseTestClass(
        /* [in] */ const String& testPackage,
        /* [in] */ const String& testClassName,
        /* [in] */ CTestSuiteBuilder* testSuiteBuilder);

    CARAPI_(Boolean) GetBooleanArgument(
        /* [in] */ IBundle* arguments,
        /* [in] */ const String& tag);

    /*
     * Returns the size predicate object, corresponding to the "size" argument value.
     */
    CARAPI_(AutoPtr<IPredicate>) GetSizePredicateFromArg(
        /* [in] */ const String& sizeArg);

    /**
    * Returns the test predicate object, corresponding to the annotation class value provided via
    * the {@link ARGUMENT_ANNOTATION} argument.
    *
    * @return the predicate or <code>null</code>
    */
    CARAPI_(AutoPtr<IPredicate>) GetAnnotationPredicate(
        /* [in] */ const String& annotationClassName);

    /**
     * Returns the negative test predicate object, corresponding to the annotation class value
     * provided via the {@link ARGUMENT_NOT_ANNOTATION} argument.
     *
     * @return the predicate or <code>null</code>
     */
    CARAPI_(AutoPtr<IPredicate>) GetNotAnnotationPredicate(
        /* [in] */ const String& annotationClassName);

     /**
     * Helper method to return the annotation class with specified name
     *
     * @param annotationClassName the fully qualified name of the class
     * @return the annotation class or <code>null</code>
     */
    // private Class<? extends Annotation> getAnnotationClass(String annotationClassName);

    CARAPI_(void) GenerateCoverageReport();

    CARAPI_(String) GetCoverageFilePath();

    CARAPI_(void) ReportEmmaError(
        /* [in] */ ECode ec);

    CARAPI_(void) ReportEmmaError(
        /* [in] */ const String& hint,
        /* [in] */ ECode ec);

public:
    /** @hide */
    static const String ARGUMENT_ANNOTATION;
    /** @hide */
    static const String ARGUMENT_NOT_ANNOTATION;

private:
    static const String SMALL_SUITE;
    static const String MEDIUM_SUITE;
    static const String LARGE_SUITE;

    static const String ARGUMENT_LOG_ONLY;

    /**
     * This constant defines the maximum allowed runtime (in ms) for a test included in the "small"
     * suite. It is used to make an educated guess at what suite an unlabeled test belongs.
     */
    static const Float SMALL_SUITE_MAX_RUNTIME;

    /**
     * This constant defines the maximum allowed runtime (in ms) for a test included in the
     * "medium" suite. It is used to make an educated guess at what suite an unlabeled test belongs.
     */
    static const Float MEDIUM_SUITE_MAX_RUNTIME;

    /**
     * If included in the status or final bundle sent to an IInstrumentationWatcher, this key
     * reports the run time in seconds of the current test.
     */
    static const String REPORT_KEY_RUN_TIME;
    /**
     * If included in the status or final bundle sent to an IInstrumentationWatcher, this key
     * reports the number of total iterations of the current test.
     */
    static const String REPORT_KEY_NUM_ITERATIONS;
    /**
     * If included in the status or final bundle sent to an IInstrumentationWatcher, this key
     * reports the guessed suite assignment for the current test.
     */
    static const String REPORT_KEY_SUITE_ASSIGNMENT;
    /**
     * If included in the status or final bundle sent to an IInstrumentationWatcher, this key
     * identifies the path to the generated code coverage file.
     */
    static const String REPORT_KEY_COVERAGE_PATH;

    // Default file name for code coverage
    static const String DEFAULT_COVERAGE_FILE_NAME;

    static const String TAG;

    AutoPtr<IBundle> mResults;
    AutoPtr<IBundle> mArguments;
    AutoPtr<CAndroidTestRunner> mTestRunner;
    Boolean mDebug;
    Boolean mJustCount;
    Boolean mSuiteAssignmentMode;
    Int32 mTestCount;
    String mPackageOfTests;
    Boolean mCoverage;
    String mCoverageFilePath;
    Int32 mDelayMsec;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEST_CINSTRUMENTATIONTESTRUNNER_H__
