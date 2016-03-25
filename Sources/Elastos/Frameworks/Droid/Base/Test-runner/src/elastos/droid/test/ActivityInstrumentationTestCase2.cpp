
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/test/ActivityInstrumentationTestCase2.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Test {

CAR_INTERFACE_IMPL(ActivityInstrumentationTestCase2, ActivityTestCase, IActivityInstrumentationTestCase2)

ActivityInstrumentationTestCase2::ActivityInstrumentationTestCase2()
    : mInitialTouchMode(FALSE)
{}

ActivityInstrumentationTestCase2::ActivityInstrumentationTestCase2(
    /* [in] */ const String& pkg,
    /* [in] */ IClassInfo* activityClass)
    : mActivityClass(activityClass)
    , mInitialTouchMode(FALSE)
{}

ActivityInstrumentationTestCase2::ActivityInstrumentationTestCase2(
    /* [in] */ IClassInfo* activityClass)
    : mActivityClass(activityClass)
    , mInitialTouchMode(FALSE)
{}

ECode ActivityInstrumentationTestCase2::Init(
    /* [in] */ const String& pkg,
    /* [in] */ IClassInfo* activityClass)
{
    return Init(activityClass);
}

ECode ActivityInstrumentationTestCase2::Init(
    /* [in] */ IClassInfo* activityClass)
{
    mActivityClass = activityClass;
    return NOERROR;
}

ECode ActivityInstrumentationTestCase2::GetActivity(
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    AutoPtr<IActivity> a;
    ActivityTestCase::GetActivity((IActivity**)&a);
    if (a == NULL) {
        // set initial touch mode
        AutoPtr<IInstrumentation> instrumentation;
        GetInstrumentation((IInstrumentation**)&instrumentation);
        instrumentation->SetInTouchMode(mInitialTouchMode);
        AutoPtr<IContext> context;
        instrumentation->GetTargetContext((IContext**)&context);
        String targetPackage;
        context->GetPackageName(&targetPackage);
        // inject custom intent, if provided
        if (mActivityIntent == NULL) {
            LaunchActivity(targetPackage, mActivityClass, NULL, (IActivity**)&a);
        }
        else {
            LaunchActivityWithIntent(targetPackage, mActivityClass, mActivityIntent, (IActivity**)&a);
        }
        SetActivity(a);
    }
    *activity = a;
    REFCOUNT_ADD(*activity);
    return NOERROR;
}

ECode ActivityInstrumentationTestCase2::SetActivityIntent(
    /* [in] */ IIntent* i)
{
    mActivityIntent = i;
    return NOERROR;
}

ECode ActivityInstrumentationTestCase2::SetActivityInitialTouchMode(
    /* [in] */ Boolean initialTouchMode)
{
    mInitialTouchMode = initialTouchMode;
    return NOERROR;
}

ECode ActivityInstrumentationTestCase2::SetUp()
{
    FAIL_RETURN(ActivityTestCase::SetUp());

    mInitialTouchMode = FALSE;
    mActivityIntent = NULL;
    return NOERROR;
}

ECode ActivityInstrumentationTestCase2::TearDown()
{
    // Finish the Activity off (unless was never launched anyway)
    AutoPtr<IActivity> a;
    ActivityTestCase::GetActivity((IActivity**)&a);
    if (a != NULL) {
        a->Finish();
        SetActivity(NULL);
    }

    // Scrub out members - protects against memory leaks in the case where someone
    // creates a non-static inner class (thus referencing the test case) and gives it to
    // someone else to hold onto
    // ScrubClass(ActivityInstrumentationTestCase2.class);

    return ActivityTestCase::TearDown();
}

ECode ActivityInstrumentationTestCase2::RunTest()
{
    // try {
    //     Method method = getClass().getMethod(getName(), (Class[]) null);
    //     if (method.isAnnotationPresent(UiThreadTest.class)) {
    //         getActivity();
    //     }
    // } catch (Exception e) {
    //     // eat the exception here; super.runTest() will catch it again and handle it properly
    // }
    return ActivityTestCase::RunTest();
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
