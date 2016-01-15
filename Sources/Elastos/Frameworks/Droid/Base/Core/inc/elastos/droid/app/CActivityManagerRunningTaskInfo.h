
#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGERRUNNINGTASKINFO_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGERRUNNINGTASKINFO_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_App_CActivityManagerRunningTaskInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Information you can retrieve about a particular task that is currently
 * "running" in the system.  Note that a running task does not mean the
 * given task actually has a process it is actively running in; it simply
 * means that the user has gone to it and never closed it, but currently
 * the system may have killed its process and is only holding on to its
 * last state in order to restart it when the user returns.
 */
CarClass(CActivityManagerRunningTaskInfo)
    , public Object
    , public IActivityManagerRunningTaskInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityManagerRunningTaskInfo();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * A unique identifier for this task.
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI SetId(
        /* [in] */ Int32 id);

    /**
     * The component launched as the first activity in the task.  This can
     * be considered the "application" of this task.
     */
    CARAPI GetBaseActivity(
        /* [out] */ IComponentName** baseActivity);

    CARAPI SetBaseActivity(
        /* [in] */ IComponentName* baseActivity);

    /**
     * The activity component at the top of the history stack of the task.
     * This is what the user is currently doing.
     */
    CARAPI GetTopActivity(
        /* [out] */ IComponentName** topActivity);

    CARAPI SetTopActivity(
        /* [in] */ IComponentName* topActivity);

    /**
     * Thumbnail representation of the task's current state.  Currently
     * always null.
     */
    CARAPI GetThumbnail(
        /* [out] */ IBitmap** thumbnail);

    CARAPI SetThumbnail(
        /* [in] */ IBitmap* thumbnail);

    /**
     * Description of the task's current state.
     */
    CARAPI GetDescription(
        /* [out] */ ICharSequence** description);

    CARAPI SetDescription(
        /* [in] */ ICharSequence* description);

    /**
     * Number of activities in this task.
     */
    CARAPI GetNumActivities(
        /* [out] */ Int32* num);

    CARAPI SetNumActivities(
        /* [in] */ Int32 num);

    /**
     * Number of activities that are currently running (not stopped
     * and persisted) in this task.
     */
    CARAPI GetNumRunning(
        /* [out] */ Int32* num);

    CARAPI SetNumRunning(
        /* [in] */ Int32 num);

private:
    Int32 mId;
    AutoPtr<IComponentName> mBaseActivity;
    AutoPtr<IComponentName> mTopActivity;
    AutoPtr<IBitmap> mThumbnail;
    AutoPtr<ICharSequence> mDescription;
    Int32 mNumActivities;
    Int32 mNumRunning;
    /**
     * Last time task was run. For sorting.
     * @hide
     */
    Int64 mLastActiveTime;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGERRUNNINGTASKINFO_H__
