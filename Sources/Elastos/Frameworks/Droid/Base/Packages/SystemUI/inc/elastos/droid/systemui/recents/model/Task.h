
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASK_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASK_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

class TaskGrouping;

/**
 * A task represents the top most task in the system's task stack.
 */
class Task
    : public Object
    , public ITask
{
public:
    /** The ComponentNameKey represents the unique primary key for a component
     * belonging to a specified user. */
    class ComponentNameKey
        : public Object
        , public IComponentNameKey
    {
    public:
        ComponentNameKey(
            /* [in] */ IComponentName* cn,
            /* [in] */ Int32 user);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

    public:
        AutoPtr<IComponentName> mComponent;
        Int32 mUserId;
    };

    /* The Task Key represents the unique primary key for the task */
    class TaskKey
        : public Object
        , public ITaskKey
    {
    public:
        TaskKey(
            /* [in] */ Int32 id,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 userId,
            /* [in] */ Int64 firstActiveTime,
            /* [in] */ Int64 lastActiveTime);

        CAR_INTERFACE_DECL()

        /** Returns the component name key for this task. */
        CARAPI_(AutoPtr<ComponentNameKey>) GetComponentNameKey();

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* hashCode);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<ComponentNameKey> mComponentNameKey;
        Int32 mId;
        AutoPtr<IIntent> mBaseIntent;
        Int32 mUserId;
        Int64 mFirstActiveTime;
        Int64 mLastActiveTime;

    };

public:
    Task();

    Task(
        /* [in] */ TaskKey* key,
        /* [in] */ Boolean isActive,
        /* [in] */ Int32 taskAffiliation,
        /* [in] */ Int32 taskAffiliationColor,
        /* [in] */ const String& activityTitle,
        /* [in] */ IDrawable* activityIcon,
        /* [in] */ Int32 colorPrimary,
        /* [in] */ Boolean lockToThisTask,
        /* [in] */ Boolean lockToTaskEnabled,
        /* [in] */ IBitmap* icon,
        /* [in] */ const String& iconFilename);

    CAR_INTERFACE_DECL()

    /** Copies the other task. */
    CARAPI CopyFrom(
        /* [in] */ Task* o);

    /** Set the callbacks */
    CARAPI SetCallbacks(
        /* [in] */ ITaskCallbacks* cb);

    /** Set the grouping */
    CARAPI SetGroup(
        /* [in] */ TaskGrouping* group);

    /** Notifies the callback listeners that this task has been loaded */
    CARAPI NotifyTaskDataLoaded(
        /* [in] */ IBitmap* thumbnail,
        /* [in] */ IDrawable* applicationIcon);

    /** Notifies the callback listeners that this task has been unloaded */
    CARAPI NotifyTaskDataUnloaded(
        /* [in] */ IBitmap* defaultThumbnail,
        /* [in] */ IDrawable* defaultApplicationIcon);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<TaskKey> mKey;
    AutoPtr<TaskGrouping> mGroup;
    Int32 mTaskAffiliation;
    Int32 mTaskAffiliationColor;
    Boolean mIsLaunchTarget;
    AutoPtr<IDrawable> mApplicationIcon;
    AutoPtr<IDrawable> mActivityIcon;
    String mActivityLabel;
    Int32 mColorPrimary;
    Boolean mUseLightOnPrimaryColor;
    AutoPtr<IBitmap> mThumbnail;
    Boolean mIsActive;
    Boolean mLockToThisTask;
    Boolean mLockToTaskEnabled;
    AutoPtr<IBitmap> mIcon;
    String mIconFilename;
    AutoPtr<ITaskCallbacks> mCb;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::SystemUI::Recents::Model::ITaskKey)

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASK_H__
