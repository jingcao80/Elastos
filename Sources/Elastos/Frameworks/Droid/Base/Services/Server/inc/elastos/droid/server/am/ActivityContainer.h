#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVITYCONTAINER_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVITYCONTAINER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/ActivityStack.h"

using Elastos::Droid::App::IIActivityContainer;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IInputEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityStackSupervisor;

/** Exactly one of these classes per Display in the system. Capable of holding zero or more
  * attached {@link ActivityStack}s */
class ActivityDisplay
    : public Object
{
public:
    ActivityDisplay(
        /* [in] */ ActivityStackSupervisor* owner);

    // After instantiation, check that mDisplay is not null before using this. The alternative
    // is for this to throw an exception if mDisplayManager.getDisplay() returns null.
    ActivityDisplay(
        /* [in] */ Int32 displayId,
        /* [in] */ ActivityStackSupervisor* owner);

    virtual CARAPI Init(
        /* [in] */ IDisplay* display);

    virtual CARAPI AttachActivities(
        /* [in] */ ActivityStack* stack);

    virtual CARAPI DetachActivitiesLocked(
        /* [in] */ ActivityStack* stack);

    virtual CARAPI GetBounds(
        /* [in] */ IPoint* bounds);

    virtual CARAPI SetVisibleBehindActivity(
        /* [in] */ IActivityRecord* r);

    virtual CARAPI_(Boolean) HasVisibleBehindActivity();

    CARAPI ToString(
        /* [out] */ String* result);

public:
    /** Actual Display this object tracks. */
    Int32 mDisplayId;
    AutoPtr<IDisplay> mDisplay;
    AutoPtr<IDisplayInfo> mDisplayInfo;
    /** All of the stacks on this display. Order matters, topmost stack is in front of all other
      * stacks, bottommost behind. Accessed directly by ActivityManager package classes */
    AutoPtr<IArrayList> mStacks;//ActivityStack
    AutoPtr<IActivityRecord> mVisibleBehindActivity;
    ActivityStackSupervisor* mOwner;
};

class VirtualActivityDisplay
    : public ActivityDisplay
{
public:
    VirtualActivityDisplay(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 density,
        /* [in] */ ActivityStackSupervisor* owner);

    CARAPI SetSurface(
        /* [in] */ ISurface* surface);

    // @Override
    CARAPI DetachActivitiesLocked(
        /* [in] */ ActivityStack* stack);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    AutoPtr<IVirtualDisplay> mVirtualDisplay;
};

class ActivityContainer
    : public Object
    , public IIActivityContainer
    , public IBinder
{
public:
    CAR_INTERFACE_DECL();

    ActivityContainer();

    CARAPI constructor(
        /* [in] */ Int32 stackId,
        /* [in] */ IDisplayListener* owner);//ActivityStackSupervisor

    CARAPI AttachToDisplayLocked(
        /* [in] */ ActivityDisplay* activityDisplay);

    // @Override
    CARAPI AttachToDisplay(
        /* [in] */ Int32 displayId);

    // @Override
    CARAPI GetDisplayId(
        /* [out] */ Int32 *result);

    // @Override
    CARAPI InjectEvent(
        /* [in] */ IInputEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ReleaseResources();

    // @Override
    CARAPI StartActivity(
        /* [in] */ IIntent* intent,
        /* [out] */ Int32* result);

    // @Override
    CARAPI StartActivityIntentSender(
        /* [in] */ IIIntentSender* intentSender,
        /* [out] */ Int32* result);

    /** Throw a SecurityException if allowEmbedded is not true */
    // @Override
    CARAPI CheckEmbeddedAllowed(
        /* [in] */ IIntent* intent);

    /** Throw a SecurityException if allowEmbedded is not true */
    // @Override
    CARAPI CheckEmbeddedAllowedIntentSender(
        /* [in] */ IIIntentSender* intentSender);

    // @Override
    CARAPI SetSurface(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 density);

    virtual CARAPI_(AutoPtr<ActivityStackSupervisor>) GetOuter();

    virtual CARAPI_(Boolean) IsAttachedLocked();

    virtual CARAPI GetBounds(
        /* [in] */ IPoint* outBounds);

    // TODO: Make sure every change to ActivityRecord.visible results in a call to this.
    virtual CARAPI SetVisible(
        /* [in] */ Boolean visible);

    virtual CARAPI SetDrawn();

    // You can always start a new task on a regular ActivityStack.
    virtual CARAPI_(Boolean) IsEligibleForNewTasks();

    virtual CARAPI OnTaskListEmptyLocked();

    virtual CARAPI ToString(
        /* [out] */ String* info);

    CARAPI_(void) DetachLocked();

private:
    CARAPI CheckEmbeddedAllowedInner(
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType);

public:
    static const Int32 FORCE_NEW_TASK_FLAGS;// = IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_MULTIPLE_TASK | IIntent::FLAG_ACTIVITY_NO_ANIMATION;
    Int32 mStackId;
    AutoPtr<IActivityContainerCallback> mCallback;
    AutoPtr<ActivityStack> mStack;
    AutoPtr<IActivityRecord> mParentActivity;
    String mIdString;
    Boolean mVisible;
    /** Display this ActivityStack is currently on. Null if not attached to a Display. */
    AutoPtr<ActivityDisplay> mActivityDisplay;
    static const Int32 CONTAINER_STATE_HAS_SURFACE;// = 0;
    static const Int32 CONTAINER_STATE_NO_SURFACE;// = 1;
    static const Int32 CONTAINER_STATE_FINISHING;// = 2;
    Int32 mContainerState;
    ActivityStackSupervisor* mOwner;
};


class VirtualActivityContainer
    : public ActivityContainer
{
public:
    VirtualActivityContainer();

    CARAPI constructor(
        /* [in] */ IActivityRecord* parent,
        /* [in] */ IActivityContainerCallback* callback,
        /* [in] */ IDisplayListener* owner);

    // @Override
    CARAPI SetSurface(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 density);

    // @Override
    CARAPI_(Boolean) IsAttachedLocked();

    // @Override
    CARAPI SetDrawn();

    // Never start a new task on an ActivityView if it isn't explicitly specified.
    // @Override
    CARAPI_(Boolean) IsEligibleForNewTasks();

    CARAPI OnTaskListEmptyLocked();

private:
    CARAPI_(void) SetSurfaceLocked(
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 density);

    CARAPI_(void) SetSurfaceIfReadyLocked();

public:
    AutoPtr<ISurface> mSurface;
    Boolean mDrawn;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_ACTIVITYCONTAINER_H__
