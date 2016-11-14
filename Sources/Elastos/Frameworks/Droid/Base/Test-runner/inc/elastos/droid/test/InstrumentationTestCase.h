
#ifndef __INSTRUMENTATIONTESTCASE_H__
#define __INSTRUMENTATIONTESTCASE_H__

#include <elastos/droid/ext/frameworkdef.h>
#include <_Elastos.Droid.Test.h>
#include <eunit/framework/TestCase.h>
#include <elastos/core/Runnable.h>

using Elastos::Core::IRunnable;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IInstrumentation;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBundle;
using Eunit::Framework::TestCase;

namespace Elastos {
namespace Droid {
namespace Test {

class ECO_PUBLIC InstrumentationTestCase
    : public TestCase
    , public IInstrumentationTestCase
{
private:
    class ECO_LOCAL _Runnable : public Runnable
    {
    public:
        _Runnable(
            /* [in] */ IRunnable* r,
            /* [in] */ ArrayOf<ECode>* exceptions)
            : mRunnable(r)
            , mExceptions(exceptions)
        {}

        CARAPI Run();

    private:
        AutoPtr<IRunnable> mRunnable;
        AutoPtr< ArrayOf<ECode> > mExceptions;
    };

    class ECO_LOCAL _RunnableInRunTest : public Runnable
    {
    public:
        _RunnableInRunTest(
            /* [in] */ IMethodInfo* testMethod,
            /* [in] */ Int32 tolerance,
            /* [in] */ Boolean isRepetitive,
            /* [out] */ ECode* ec,
            /* [in] */ InstrumentationTestCase* host)
            : mTestMethod(testMethod)
            , mTolerance(tolerance)
            , mIsRepetitive(isRepetitive)
            , mEC(ec)
            , mHost(host)
        {}

        CARAPI Run();

    private:
        AutoPtr<IMethodInfo> mTestMethod;
        Int32 mTolerance;
        Boolean mIsRepetitive;
        ECode* mEC;
        InstrumentationTestCase* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Injects instrumentation into this test case. This method is
     * called by the test runner during test setup.
     *
     * @param instrumentation the instrumentation to use with this instance
     */
    CARAPI InjectInstrumentation(
        /* [in] */ IInstrumentation* instrumentation);

    /**
     * Inheritors can access the instrumentation using this.
     * @return instrumentation
     */
    CARAPI GetInstrumentation(
        /* [out] */ IInstrumentation** instrumentation);

    /**
     * Utility method for launching an activity.
     *
     * <p>The {@link Intent} used to launch the Activity is:
     *  action = {@link Intent#ACTION_MAIN}
     *  extras = null, unless a custom bundle is provided here
     * All other fields are null or empty.
     *
     * <p><b>NOTE:</b> The parameter <i>pkg</i> must refer to the package identifier of the
     * package hosting the activity to be launched, which is specified in the AndroidManifest.xml
     * file.  This is not necessarily the same as the java package name.
     *
     * @param pkg The package hosting the activity to be launched.
     * @param activityCls The activity class to launch.
     * @param extras Optional extra stuff to pass to the activity.
     * @return The activity, or null if non launched.
     */
    CARAPI LaunchActivity(
        /* [in] */ const String& pkg,
        /* [in] */ IClassInfo* activityCls,
        /* [in] */ IBundle* extras,
        /* [out] */ IActivity** activity);

    /**
     * Utility method for launching an activity with a specific Intent.
     *
     * <p><b>NOTE:</b> The parameter <i>pkg</i> must refer to the package identifier of the
     * package hosting the activity to be launched, which is specified in the AndroidManifest.xml
     * file.  This is not necessarily the same as the java package name.
     *
     * @param pkg The package hosting the activity to be launched.
     * @param activityCls The activity class to launch.
     * @param intent The intent to launch with
     * @return The activity, or null if non launched.
     */
    CARAPI LaunchActivityWithIntent(
        /* [in] */ const String& pkg,
        /* [in] */ IClassInfo* activityCls,
        /* [in] */ IIntent* intent,
        /* [out] */ IActivity** activity);

    /**
     * Helper for running portions of a test on the UI thread.
     *
     * Note, in most cases it is simpler to annotate the test method with
     * {@link android.test.UiThreadTest}, which will run the entire test method on the UI thread.
     * Use this method if you need to switch in and out of the UI thread to perform your test.
     *
     * @param r runnable containing test code in the {@link Runnable#run()} method
     */
   CARAPI RunTestOnUiThread(
        /* [in] */ IRunnable* r);

    /**
     * Sends a series of key events through instrumentation and waits for idle. The sequence
     * of keys is a string containing the key names as specified in KeyEvent, without the
     * KEYCODE_ prefix. For instance: sendKeys("DPAD_LEFT A B C DPAD_CENTER"). Each key can
     * be repeated by using the N* prefix. For instance, to send two KEYCODE_DPAD_LEFT, use
     * the following: sendKeys("2*DPAD_LEFT").
     *
     * @param keysSequence The sequence of keys.
     */
    CARAPI SendKeys(
        /* [in] */ const String& keysSequence);

    /**
     * Sends a series of key events through instrumentation and waits for idle. For instance:
     * sendKeys(KEYCODE_DPAD_LEFT, KEYCODE_DPAD_CENTER).
     *
     * @param keys The series of key codes to send through instrumentation.
     */
    CARAPI SendKeys(
        /* [in] */ ArrayOf<Int32>* keys);

    /**
     * Sends a series of key events through instrumentation and waits for idle. Each key code
     * must be preceded by the number of times the key code must be sent. For instance:
     * sendRepeatedKeys(1, KEYCODE_DPAD_CENTER, 2, KEYCODE_DPAD_LEFT).
     *
     * @param keys The series of key repeats and codes to send through instrumentation.
     */
    CARAPI SendRepeatedKeys(
        /* [in] */ ArrayOf<Int32>* keys);

protected:
    /**
     * Runs the current unit test. If the unit test is annotated with
     * {@link android.test.UiThreadTest}, the test is run on the UI thread.
     */
    CARAPI RunTest();

    /**
     * Make sure all resources are cleaned up and garbage collected before moving on to the next
     * test. Subclasses that override this method should make sure they call super.tearDown()
     * at the end of the overriding method.
     *
     * @throws Exception
     */
    CARAPI TearDown();

private:
    // For backwards-compatibility after adding isRepetitive
    ECO_LOCAL CARAPI RunMethod(
        /* [in] */ IMethodInfo* runMethod,
        /* [in] */ Int32 tolerance);

    ECO_LOCAL CARAPI RunMethod(
        /* [in] */ IMethodInfo* runMethod,
        /* [in] */ Int32 tolerance,
        /* [in] */ Boolean isRepetitive);

    ECO_LOCAL CARAPI_(Int32) KeyStringToKeyCode(
        /* [in] */ const String& key);

private:
    AutoPtr<IInstrumentation> mInstrumentation;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__INSTRUMENTATIONTESTCASE_H__
