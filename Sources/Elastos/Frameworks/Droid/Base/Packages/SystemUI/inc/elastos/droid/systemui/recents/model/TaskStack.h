
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKSTACK_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKSTACK_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "elastos/droid/systemui/recents/model/TaskGrouping.h"

using Elastos::Core::IComparator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * A list of filtered tasks.
 */
class FilteredTaskList : public Object
{
public:
    FilteredTaskList();

    /** Sets the task filter, saving the current touch state */
    CARAPI_(Boolean) SetFilter(
        /* [in] */ ITaskFilter* filter);

    /** Removes the task filter and returns the previous touch state */
    CARAPI_(void) RemoveFilter();

    /** Adds a new task to the task list */
    CARAPI_(void) Add(
        /* [in] */ Task* t);

    /** Sets the list of tasks */
    CARAPI_(void) Set(
        /* [in] */ IList* tasks);

    /** Removes a task from the base list only if it is in the filtered list */
    CARAPI_(Boolean) Remove(
        /* [in] */ Task* t);

    /** Returns the index of this task in the list of filtered tasks */
    CARAPI_(Int32) IndexOf(
        /* [in] */ Task* t);

    /** Returns the size of the list of filtered tasks */
    CARAPI_(Int32) GetSize();

    /** Returns whether the filtered list contains this task */
    CARAPI_(Boolean) Contains(
        /* [in] */ Task* t);

    /** Returns whether this task list is filtered */
    CARAPI_(Boolean) HasFilter();

    /** Returns the list of filtered tasks */
    CARAPI_(AutoPtr<IArrayList>) GetTasks();

private:
    /** Updates the list of filtered tasks whenever the base task list changes */
    CARAPI_(void) UpdateFilteredTasks();

    /** Updates the mapping of tasks to indices. */
    CARAPI_(void) UpdateFilteredTaskIndices();

public:
    AutoPtr<IArrayList> mTasks; // item is ITask
    AutoPtr<IArrayList> mFilteredTasks; // item is ITask
    HashMap<AutoPtr<ITaskKey>, Int32> mTaskIndices;
    AutoPtr<ITaskFilter> mFilter;
};

/**
 * The task stack contains a list of multiple tasks.
 */
class TaskStack
    : public Object
    , public ITaskStack
{
public:
    /** A pair of indices representing the group and task positions in the stack and group. */
    class GroupTaskIndex : public Object
    {
    public:
        GroupTaskIndex()
            : mGroupIndex(0)
            , mTaskIndex(0)
        {}

        GroupTaskIndex(
            /* [in] */ Int32 gi,
            /* [in] */ Int32 ti)
            : mGroupIndex(gi)
            , mTaskIndex(ti)
        {}

    public:
        Int32 mGroupIndex; // Index in the stack
        Int32 mTaskIndex;  // Index in the group
    };

private:
    class TaskFilter
        : public Object
        , public ITaskFilter
    {
    public:
        TaskFilter(
            /* [in] */ Task* t);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI AcceptTask(
            /* [in] */ ITask* _at,
            /* [in] */ Int32 i,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<Task> mT;
    };

    class TaskComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* _task,
            /* [in] */ IInterface* _task2,
            /* [out] */ Int32* result);
    };

    class TaskGroupingComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* _taskGrouping,
            /* [in] */ IInterface* _taskGrouping2,
            /* [out] */ Int32* result);
    };

    class TaskKeyComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* _taskKey,
            /* [in] */ IInterface* _taskKey2,
            /* [out] */ Int32* result);
    };

public:
    TaskStack();

    CAR_INTERFACE_DECL()

    /** Sets the callbacks for this task stack */
    CARAPI_(void) SetCallbacks(
        /* [in] */ ITaskStackCallbacks* cb);

    /** Adds a new task */
    CARAPI_(void) AddTask(
        /* [in] */ Task* t);

    /** Removes a task */
    CARAPI_(void) RemoveTask(
        /* [in] */ Task* t);

    /** Sets a few tasks in one go */
    CARAPI_(void) SetTasks(
        /* [in] */ IList* tasks);

    /** Gets the front task */
    CARAPI_(AutoPtr<Task>) GetFrontMostTask();

    /** Gets the tasks */
    CARAPI_(AutoPtr<IArrayList>) GetTasks();

    /** Gets the number of tasks */
    CARAPI_(Int32) GetTaskCount();

    /** Returns the index of this task in this current task stack */
    CARAPI_(Int32) IndexOfTask(
        /* [in] */ Task* t);

    /** Finds the task with the specified task id. */
    CARAPI_(AutoPtr<Task>) FindTaskWithId(
        /* [in] */ Int32 taskId);

    /******** Filtering ********/

    /** Filters the stack into tasks similar to the one specified */
    CARAPI_(void) FilterTasks(
        /* [in] */ Task* t);

    /** Unfilters the current stack */
    CARAPI_(void) UnfilterTasks();

    /** Returns whether tasks are currently filtered */
    CARAPI_(Boolean) HasFilteredTasks();

    /******** Grouping ********/

    /** Adds a group to the set */
    CARAPI_(void) AddGroup(
        /* [in] */ TaskGrouping* group);

    CARAPI_(void) RemoveGroup(
        /* [in] */ TaskGrouping* group);

    /** Returns the group with the specified affiliation. */
    CARAPI_(AutoPtr<TaskGrouping>) GetGroupWithAffiliation(
        /* [in] */ Int32 affiliation);

    /**
     * Temporary: This method will simulate affiliation groups by
     */
    CARAPI_(void) CreateAffiliatedGroupings(
        /* [in] */ RecentsConfiguration* config);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    // The task offset to apply to a task id as a group affiliation
    static const Int32 IndividualTaskIdOffset = 1 << 16;

    AutoPtr<FilteredTaskList> mTaskList;
    AutoPtr<ITaskStackCallbacks> mCb;

    AutoPtr<IArrayList> mGroups; // item is TaskGrouping
    HashMap<Int32, AutoPtr<TaskGrouping> > mAffinitiesGroups;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKSTACK_H__
