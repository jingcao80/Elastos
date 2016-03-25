
#ifndef __ACTIVITYINSTRUMENTATIONTESTCASE2_H__
#define __ACTIVITYINSTRUMENTATIONTESTCASE2_H__

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/droid/test/ActivityTestCase.h>

namespace Elastos {
namespace Droid {
namespace Test {

class ECO_PUBLIC ActivityInstrumentationTestCase2
    : public ActivityTestCase
    , public IActivityInstrumentationTestCase2
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates an {@link ActivityInstrumentationTestCase2}.
     *
     * @param pkg ignored - no longer in use.
     * @param activityClass The activity to test. This must be a class in the instrumentation
     * targetPackage specified in the AndroidManifest.xml
     *
     * @deprecated use {@link #ActivityInstrumentationTestCase2(Class)} instead
     */
    ActivityInstrumentationTestCase2(
        /* [in] */ const String& pkg,
        /* [in] */ IClassInfo* activityClass);

    /**
     * Creates an {@link ActivityInstrumentationTestCase2}.
     *
     * @param activityClass The activity to test. This must be a class in the instrumentation
     * targetPackage specified in the AndroidManifest.xml
     */
    ActivityInstrumentationTestCase2(
        /* [in] */ IClassInfo* activityClass);

    /**
     * Get the Activity under test, starting it if necessary.
     *
     * For each test method invocation, the Activity will not actually be created until the first
     * time this method is called.
     *
     * <p>If you wish to provide custom setup values to your Activity, you may call
     * {@link #setActivityIntent(Intent)} and/or {@link #setActivityInitialTouchMode(boolean)}
     * before your first call to getActivity().  Calling them after your Activity has
     * started will have no effect.
     *
     * <p><b>NOTE:</b> Activities under test may not be started from within the UI thread.
     * If your test method is annotated with {@link android.test.UiThreadTest}, then your Activity
     * will be started automatically just before your test method is run.  You still call this
     * method in order to get the Activity under test.
     *
     * @return the Activity under test
     */
    CARAPI GetActivity(
        /* [out] */ IActivity** activity);

    /**
     * Call this method before the first call to {@link #getActivity} to inject a customized Intent
     * into the Activity under test.
     *
     * <p>If you do not call this, the default intent will be provided.  If you call this after
     * your Activity has been started, it will have no effect.
     *
     * <p><b>NOTE:</b> Activities under test may not be started from within the UI thread.
     * If your test method is annotated with {@link android.test.UiThreadTest}, then you must call
     * {@link #setActivityIntent(Intent)} from {@link #setUp()}.
     *
     * <p>The default Intent (if this method is not called) is:
     *  action = {@link Intent#ACTION_MAIN}
     *  flags = {@link Intent#FLAG_ACTIVITY_NEW_TASK}
     * All other fields are null or empty.
     *
     * @param i The Intent to start the Activity with, or null to reset to the default Intent.
     */
    CARAPI SetActivityIntent(
        /* [in] */ IIntent* i);

    /**
     * Call this method before the first call to {@link #getActivity} to set the initial touch
     * mode for the Activity under test.
     *
     * <p>If you do not call this, the touch mode will be false.  If you call this after
     * your Activity has been started, it will have no effect.
     *
     * <p><b>NOTE:</b> Activities under test may not be started from within the UI thread.
     * If your test method is annotated with {@link android.test.UiThreadTest}, then you must call
     * {@link #setActivityInitialTouchMode(boolean)} from {@link #setUp()}.
     *
     * @param initialTouchMode true if the Activity should be placed into "touch mode" when started
     */
    CARAPI SetActivityInitialTouchMode(
        /* [in] */ Boolean initialTouchMode);

protected:
    ActivityInstrumentationTestCase2();

    /**
     * Creates an {@link ActivityInstrumentationTestCase2}.
     *
     * @param pkg ignored - no longer in use.
     * @param activityClass The activity to test. This must be a class in the instrumentation
     * targetPackage specified in the AndroidManifest.xml
     *
     * @deprecated use {@link #ActivityInstrumentationTestCase2(Class)} instead
     */
    CARAPI Init(
        /* [in] */ const String& pkg,
        /* [in] */ IClassInfo* activityClass);

    /**
     * Creates an {@link ActivityInstrumentationTestCase2}.
     *
     * @param activityClass The activity to test. This must be a class in the instrumentation
     * targetPackage specified in the AndroidManifest.xml
     */
    CARAPI Init(
        /* [in] */ IClassInfo* activityClass);

    CARAPI SetUp();

    CARAPI TearDown();

    /**
     * Runs the current unit test. If the unit test is annotated with
     * {@link android.test.UiThreadTest}, force the Activity to be created before switching to
     * the UI thread.
     */
    CARAPI RunTest();

public:
    AutoPtr<IClassInfo> mActivityClass;
    Boolean mInitialTouchMode;
    AutoPtr<IIntent> mActivityIntent;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__ACTIVITYINSTRUMENTATIONTESTCASE2_H__
