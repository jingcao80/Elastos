
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKGROUPING_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKGROUPING_H__

#include "elastos/droid/systemui/recents/model/Task.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {


/** Represents a grouping of tasks witihin a stack. */
class TaskGrouping
    : public Object
    , public ITaskGrouping
{
public:
    /** Creates a group with a specified affiliation. */
    TaskGrouping(
        /* [in] */ Int32 affiliation);

    CAR_INTERFACE_DECL()

    /** Adds a new task to this group. */
    CARAPI_(void) AddTask(
        /* [in] */ Task* t);

    /** Removes a task from this group. */
    CARAPI_(void) RemoveTask(
        /* [in] */ Task* t);

    /** Returns the key of the next task in the group. */
    CARAPI_(AutoPtr<Task::TaskKey>) GetNextTaskInGroup(
        /* [in] */ Task* t);

    /** Returns the key of the previous task in the group. */
    CARAPI_(AutoPtr<Task::TaskKey>) GetPrevTaskInGroup(
        /* [in] */ Task* t);

    /** Gets the front task */
    CARAPI_(Boolean) IsFrontMostTask(
        /* [in] */ Task* t);

    /** Finds the index of a given task in a group. */
    CARAPI_(Int32) IndexOf(
        /* [in] */ Task* t);

    /** Returns whether a task is in this grouping. */
    CARAPI_(Boolean) ContainsTask(
        /* [in] */ Task* t);

    /** Returns whether one task is above another in the group.  If they are not in the same group,
     * this returns false. */
    CARAPI_(Boolean) IsTaskAboveTask(
        /* [in] */ Task* t,
        /* [in] */ Task* below);

    /** Returns the number of tasks in this group. */
    CARAPI_(Int32) GetTaskCount();

private:
    /** Updates the mapping of tasks to indices. */
    CARAPI_(void) UpdateTaskIndices();

public:
    Int32 mAffiliation;
    Int64 mLatestActiveTimeInGroup;

    AutoPtr<Task::TaskKey> mFrontMostTaskKey;
    AutoPtr<IList> mTaskKeys; // item is Task::TaskKey
    HashMap<AutoPtr<ITaskKey>, Int32> mTaskKeyIndices;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKGROUPING_H__
