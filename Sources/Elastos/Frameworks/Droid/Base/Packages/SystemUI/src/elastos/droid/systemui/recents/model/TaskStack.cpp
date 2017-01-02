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

#include "elastos/droid/systemui/recents/model/TaskStack.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include "elastos/droid/systemui/recents/misc/Utilities.h"
#include "elastos/droid/systemui/recents/misc/NamedCounter.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::SystemUI::Recents::Misc::NamedCounter;
using Elastos::Droid::SystemUI::Recents::Misc::Utilities;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

FilteredTaskList::FilteredTaskList()
{
    CArrayList::New((IArrayList**)&mTasks);
    CArrayList::New((IArrayList**)&mFilteredTasks);
}

/** Sets the task filter, saving the current touch state */
Boolean FilteredTaskList::SetFilter(
    /* [in] */ ITaskFilter* filter)
{
    AutoPtr<IArrayList> prevFilteredTasks;
    CArrayList::New(ICollection::Probe(mFilteredTasks), (IArrayList**)&prevFilteredTasks);
    mFilter = filter;
    UpdateFilteredTasks();
    Boolean equals;
    prevFilteredTasks->Equals(mFilteredTasks, &equals);
    if (!equals) {
        return TRUE;
    }
    else {
        // If the tasks are exactly the same pre/post filter, then just reset it
        mFilter = NULL;
        return FALSE;
    }
}

/** Removes the task filter and returns the previous touch state */
void FilteredTaskList::RemoveFilter()
{
    mFilter = NULL;
    UpdateFilteredTasks();
}

/** Adds a new task to the task list */
void FilteredTaskList::Add(
    /* [in] */ Task* t)
{
    mTasks->Add((ITask*)t);
    UpdateFilteredTasks();
}

/** Sets the list of tasks */
void FilteredTaskList::Set(
    /* [in] */ IList* tasks)
{
    mTasks->Clear();
    mTasks->AddAll(ICollection::Probe(tasks));
    UpdateFilteredTasks();
}

/** Removes a task from the base list only if it is in the filtered list */
Boolean FilteredTaskList::Remove(
    /* [in] */ Task* t)
{
    Boolean contains;
    mFilteredTasks->Contains((ITask*)t, &contains);
    if (contains) {
        Boolean removed;
        mTasks->Remove((ITask*)t, &removed);
        UpdateFilteredTasks();
        return removed;
    }
    return FALSE;
}

/** Returns the index of this task in the list of filtered tasks */
Int32 FilteredTaskList::IndexOf(
    /* [in] */ Task* t)
{
    HashMap<AutoPtr<ITaskKey>, Int32>::Iterator find = mTaskIndices.Find(t->mKey);
    if (find != mTaskIndices.End()) {
        return find->mSecond;
    }
    return -1;
}

/** Returns the size of the list of filtered tasks */
Int32 FilteredTaskList::GetSize()
{
    Int32 size;
    mFilteredTasks->GetSize(&size);
    return size;
}

/** Returns whether the filtered list contains this task */
Boolean FilteredTaskList::Contains(
    /* [in] */ Task* t)
{
    return mTaskIndices.Find(t->mKey) != mTaskIndices.End();
}

/** Updates the list of filtered tasks whenever the base task list changes */
void FilteredTaskList::UpdateFilteredTasks()
{
    mFilteredTasks->Clear();
    if (mFilter != NULL) {
        Int32 taskCount;
        mTasks->GetSize(&taskCount);
        for (Int32 i = 0; i < taskCount; i++) {
            AutoPtr<IInterface> item;
            mTasks->Get(i, (IInterface**)&item);
            ITask* t = ITask::Probe(item);
            Boolean res;
            mFilter->AcceptTask(t, i, &res);
            if (res) {
                mFilteredTasks->Add(t);
            }
        }
    }
    else {
        mFilteredTasks->AddAll(ICollection::Probe(mTasks));
    }
    UpdateFilteredTaskIndices();
}

/** Updates the mapping of tasks to indices. */
void FilteredTaskList::UpdateFilteredTaskIndices()
{
    mTaskIndices.Clear();
    Int32 taskCount;
    mFilteredTasks->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> item;
        mFilteredTasks->Get(i, (IInterface**)&item);
        Task* t = (Task*)ITask::Probe(item);
        mTaskIndices[t->mKey] = i;
    }
}

/** Returns whether this task list is filtered */
Boolean FilteredTaskList::HasFilter()
{
    return (mFilter != NULL);
}

/** Returns the list of filtered tasks */
AutoPtr<IArrayList> FilteredTaskList::GetTasks()
{
    return mFilteredTasks;
}

CAR_INTERFACE_IMPL(TaskStack::TaskFilter, Object, ITaskFilter)

TaskStack::TaskFilter::TaskFilter(
    /* [in] */ Task* t)
    : mT(t)
{}

// @Override
ECode TaskStack::TaskFilter::AcceptTask(
    /* [in] */ ITask* _at,
    /* [in] */ Int32 i,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Task* at = (Task*)_at;
    AutoPtr<IComponentName> tComp, atComp;
    mT->mKey->mBaseIntent->GetComponent((IComponentName**)&tComp);
    at->mKey->mBaseIntent->GetComponent((IComponentName**)&atComp);
    String tPkgName, atPkgName;
    tComp->GetPackageName(&tPkgName);
    atComp->GetPackageName(&atPkgName);
    *result = tPkgName.Equals(atPkgName);
    return NOERROR;
}

CAR_INTERFACE_IMPL(TaskStack::TaskComparator, Object, IComparator)

ECode TaskStack::TaskComparator::Compare(
    /* [in] */ IInterface* _task,
    /* [in] */ IInterface* _task2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Task* task = (Task*)ITask::Probe(_task);
    Task* task2 = (Task*)ITask::Probe(_task2);
    assert(task && task2);
    *result = (Int32) (task->mKey->mFirstActiveTime - task2->mKey->mFirstActiveTime);
    return NOERROR;
}

CAR_INTERFACE_IMPL(TaskStack::TaskGroupingComparator, Object, IComparator)

ECode TaskStack::TaskGroupingComparator::Compare(
    /* [in] */ IInterface* _taskGrouping,
    /* [in] */ IInterface* _taskGrouping2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    TaskGrouping* taskGrouping = (TaskGrouping*)ITaskGrouping::Probe(_taskGrouping);
    TaskGrouping* taskGrouping2 = (TaskGrouping*)ITaskGrouping::Probe(_taskGrouping2);
    assert(taskGrouping && taskGrouping2);
    *result = (Int32) (taskGrouping->mLatestActiveTimeInGroup - taskGrouping2->mLatestActiveTimeInGroup);
    return NOERROR;
}

CAR_INTERFACE_IMPL(TaskStack::TaskKeyComparator, Object, IComparator)

ECode TaskStack::TaskKeyComparator::Compare(
    /* [in] */ IInterface* _taskKey,
    /* [in] */ IInterface* _taskKey2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Task::TaskKey* taskKey = (Task::TaskKey*)ITaskKey::Probe(_taskKey);
    Task::TaskKey* taskKey2 = (Task::TaskKey*)ITaskKey::Probe(_taskKey2);
    assert(taskKey && taskKey2);
    *result = (Int32) (taskKey->mFirstActiveTime - taskKey2->mFirstActiveTime);
    return NOERROR;
}

CAR_INTERFACE_IMPL(TaskStack, Object, ITaskStack)

const Int32 TaskStack::IndividualTaskIdOffset;

TaskStack::TaskStack()
{
    mTaskList = new FilteredTaskList();
    CArrayList::New((IArrayList**)&mGroups);
}

/** Sets the callbacks for this task stack */
void TaskStack::SetCallbacks(
    /* [in] */ ITaskStackCallbacks* cb)
{
    mCb = cb;
}

/** Adds a new task */
void TaskStack::AddTask(
    /* [in] */ Task* t)
{
    mTaskList->Add(t);
    if (mCb != NULL) {
        mCb->OnStackTaskAdded(this, t);
    }
}

/** Removes a task */
void TaskStack::RemoveTask(
    /* [in] */ Task* t)
{
    if (mTaskList->Contains(t)) {
        // Remove the task from the list
        mTaskList->Remove(t);
        // Remove it from the group as well, and if it is empty, remove the group
        AutoPtr<TaskGrouping> group = (TaskGrouping*)(t->mGroup).Get();
        group->RemoveTask(t);
        if (group->GetTaskCount() == 0) {
            RemoveGroup(group);
        }
        // Update the lock-to-app state
        t->mLockToThisTask = FALSE;
        AutoPtr<Task> newFrontMostTask = GetFrontMostTask();
        if (newFrontMostTask != NULL && newFrontMostTask->mLockToTaskEnabled) {
            newFrontMostTask->mLockToThisTask = TRUE;
        }
        if (mCb != NULL) {
            // Notify that a task has been removed
            mCb->OnStackTaskRemoved(this, t, newFrontMostTask);
        }
    }
}

/** Sets a few tasks in one go */
void TaskStack::SetTasks(
    /* [in] */ IList* tasks)
{
    AutoPtr<IArrayList> taskList = mTaskList->GetTasks();
    Int32 taskCount;
    taskList->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> item;
        taskList->Get(i, (IInterface**)&item);
        Task* t = (Task*)ITask::Probe(item);
        // Remove the task from the list
        mTaskList->Remove(t);
        // Remove it from the group as well, and if it is empty, remove the group
        AutoPtr<TaskGrouping> group = (TaskGrouping*)(t->mGroup).Get();
        group->RemoveTask(t);
        if (group->GetTaskCount() == 0) {
            RemoveGroup(group);
        }
        if (mCb != NULL) {
            // Notify that a task has been removed
            mCb->OnStackTaskRemoved(this, t, NULL);
        }
    }

    mTaskList->Set(tasks);
    tasks->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> item;
        tasks->Get(i, (IInterface**)&item);
        ITask* t = ITask::Probe(item);
        if (mCb != NULL) {
            mCb->OnStackTaskAdded(this, t);
        }
    }
}

/** Gets the front task */
AutoPtr<Task> TaskStack::GetFrontMostTask()
{

    if (mTaskList->GetSize() == 0)
        return NULL;
    AutoPtr<IInterface> item;
    mTaskList->GetTasks()->Get(mTaskList->GetSize() - 1, (IInterface**)&item);
    return (Task*)ITask::Probe(item);
}

/** Gets the tasks */
AutoPtr<IArrayList> TaskStack::GetTasks()
{
    return mTaskList->GetTasks();
}

/** Gets the number of tasks */
Int32 TaskStack::GetTaskCount()
{
    return mTaskList->GetSize();
}

/** Returns the index of this task in this current task stack */
Int32 TaskStack::IndexOfTask(
    /* [in] */ Task* t)
{
    return mTaskList->IndexOf(t);
}

/** Finds the task with the specified task id. */
AutoPtr<Task> TaskStack::FindTaskWithId(
    /* [in] */ Int32 taskId)
{
    AutoPtr<IArrayList> tasks = mTaskList->GetTasks();
    Int32 taskCount;
    tasks->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> item;
        tasks->Get(i, (IInterface**)&item);
        Task* task = (Task*)ITask::Probe(item);
        if (task->mKey->mId == taskId) {
            return task;
        }
    }
    return NULL;
}

/******** Filtering ********/

/** Filters the stack into tasks similar to the one specified */
void TaskStack::FilterTasks(
    /* [in] */ Task* t)
{
    AutoPtr<IArrayList> oldStack;
    CArrayList::New(ICollection::Probe(mTaskList->GetTasks()), (IArrayList**)&oldStack);

    // Set the task list filter
    AutoPtr<ITaskFilter> filter = new TaskFilter(t);
    Boolean filtered = mTaskList->SetFilter(filter);
    if (filtered && mCb != NULL) {
        mCb->OnStackFiltered(this, IArrayList::Probe(oldStack), t);
    }
}

/** Unfilters the current stack */
void TaskStack::UnfilterTasks()
{
    AutoPtr<IArrayList> oldStack;
    CArrayList::New(ICollection::Probe(mTaskList->GetTasks()), (IArrayList**)&oldStack);

    // Unset the filter, then update the virtual scroll
    mTaskList->RemoveFilter();
    if (mCb != NULL) {
        mCb->OnStackUnfiltered(this, IArrayList::Probe(oldStack));
    }
}

/** Returns whether tasks are currently filtered */
Boolean TaskStack::HasFilteredTasks()
{
    return mTaskList->HasFilter();
}

/******** Grouping ********/

/** Adds a group to the set */
void TaskStack::AddGroup(
    /* [in] */ TaskGrouping* group)
{
    assert(group != NULL);
    mGroups->Add((ITaskGrouping*)group);
    mAffinitiesGroups[group->mAffiliation] = group;
}

void TaskStack::RemoveGroup(
    /* [in] */ TaskGrouping* group)
{
    AutoPtr<ITaskGrouping> tg = (ITaskGrouping*)group;
    mGroups->Remove(tg);
    mAffinitiesGroups.Erase(group->mAffiliation);
}

/** Returns the group with the specified affiliation. */
AutoPtr<TaskGrouping> TaskStack::GetGroupWithAffiliation(
    /* [in] */ Int32 affiliation)
{
    HashMap<Int32, AutoPtr<TaskGrouping> >::Iterator find = mAffinitiesGroups.Find(affiliation);
    return find != mAffinitiesGroups.End() ? find->mSecond : NULL;
}

/**
 * Temporary: This method will simulate affiliation groups by
 */
void TaskStack::CreateAffiliatedGroupings(
    /* [in] */ RecentsConfiguration* config)
{
    if (Constants::DebugFlags::App::EnableSimulatedTaskGroups) {
        HashMap<AutoPtr<ITaskKey>, AutoPtr<ITask> > taskMap;
        // Sort all tasks by increasing firstActiveTime of the task
        AutoPtr<IArrayList> tasks = mTaskList->GetTasks();
        AutoPtr<IComparator> comparator = new TaskComparator();
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        collections->Sort(IList::Probe(tasks), comparator);

        // Create groups when sequential packages are the same
        AutoPtr<NamedCounter> counter = new NamedCounter(String("task-group"), String(""));
        Int32 taskCount;
        tasks->GetSize(&taskCount);
        String prevPackage("");
        Int32 prevAffiliation = -1;
        AutoPtr<IRandom> r;
        CRandom::New((IRandom**)&r);
        Int32 groupCountDown = Constants::DebugFlags::App::TaskAffiliationsGroupCount;

        for (Int32 i = 0; i < taskCount; i++) {
            AutoPtr<IInterface> item;
            tasks->Get(i, (IInterface**)&item);
            Task* t = (Task*)ITask::Probe(item);
            AutoPtr<IComponentName> comp;
            t->mKey->mBaseIntent->GetComponent((IComponentName**)&comp);
            String packageName;
            comp->GetPackageName(&packageName);
            packageName = "pkg"; // Is bug??
            AutoPtr<TaskGrouping> group;
            if (packageName.Equals(prevPackage) && groupCountDown > 0) {
                group = GetGroupWithAffiliation(prevAffiliation);
                groupCountDown--;
            }
            else {
                Int32 affiliation = IndividualTaskIdOffset + t->mKey->mId;
                group = new TaskGrouping(affiliation);
                AddGroup(group);
                prevAffiliation = affiliation;
                prevPackage = packageName;
                groupCountDown = Constants::DebugFlags::App::TaskAffiliationsGroupCount;
            }
            group->AddTask(t);
            taskMap[t->mKey] = t;
        }
        // Sort groups by increasing latestActiveTime of the group
        comparator = new TaskGroupingComparator();
        collections->Sort(IList::Probe(mGroups), comparator);
        // Sort group tasks by increasing firstActiveTime of the task, and also build a new list of
        // tasks
        Int32 taskIndex = 0;
        Int32 groupCount;
        mGroups->GetSize(&groupCount);
        comparator = new TaskKeyComparator();
        for (Int32 i = 0; i < groupCount; i++) {
            AutoPtr<IInterface> item;
            tasks->Get(i, (IInterface**)&item);
            TaskGrouping* group = (TaskGrouping*)ITaskGrouping::Probe(item);
            collections->Sort(IList::Probe(group->mTaskKeys), comparator);
            AutoPtr<IArrayList> groupTasks = group->mTaskKeys;
            Int32 groupTaskCount;
            groupTasks->GetSize(&groupTaskCount);
            for (Int32 j = 0; j < groupTaskCount; j++) {
                AutoPtr<IInterface> item2;
                groupTasks->Get(j, (IInterface**)&item2);
                AutoPtr<ITaskKey> taskKey = ITaskKey::Probe(item2);
                tasks->Set(taskIndex, taskMap[taskKey]);
                taskIndex++;
            }
        }
        mTaskList->Set(IList::Probe(tasks));
    }
    else {
        // Create the task groups
        HashMap<AutoPtr<ITaskKey>, AutoPtr<Task> > tasksMap;
        AutoPtr<IArrayList> tasks = mTaskList->GetTasks();
        Int32 taskCount;
        tasks->GetSize(&taskCount);
        for (Int32 i = 0; i < taskCount; i++) {
            AutoPtr<IInterface> item;
            tasks->Get(i, (IInterface**)&item);
            Task* t = (Task*)ITask::Probe(item);
            AutoPtr<TaskGrouping> group;
            Int32 affiliation = t->mTaskAffiliation > 0 ? t->mTaskAffiliation :
                    IndividualTaskIdOffset + t->mKey->mId;
            if (mAffinitiesGroups.Find(affiliation) != mAffinitiesGroups.End()) {
                group = GetGroupWithAffiliation(affiliation);
            }
            else {
                group = new TaskGrouping(affiliation);
                AddGroup(group);
            }
            group->AddTask(t);
            tasksMap[t->mKey] = t;
        }

        // Update the task colors for each of the groups
        Float minAlpha = config->mTaskBarViewAffiliationColorMinAlpha;
        Int32 groupCount;
        mGroups->GetSize(&groupCount);
        for (Int32 i = 0; i < groupCount; i++) {
            AutoPtr<IInterface> item;
            mGroups->Get(i, (IInterface**)&item);
            TaskGrouping* group = (TaskGrouping*)ITaskGrouping::Probe(item);
            taskCount = group->GetTaskCount();
            // Ignore the groups that only have one task
            if (taskCount <= 1) continue;
            // Calculate the group color distribution
            AutoPtr<IInterface> key;
            group->mTaskKeys->Get(0, (IInterface**)&key);
            Int32 affiliationColor = tasksMap[ITaskKey::Probe(key)]->mTaskAffiliationColor;
            Float alphaStep = (1.0f - minAlpha) / taskCount;
            Float alpha = 1.0f;
            for (Int32 j = 0; j < taskCount; j++) {
                AutoPtr<IInterface> item2;
                group->mTaskKeys->Get(j, (IInterface**)&item2);
                AutoPtr<ITaskKey> taskKey = ITaskKey::Probe(item2);
                AutoPtr<Task> t = tasksMap[taskKey];
                t->mColorPrimary = Utilities::GetColorWithOverlay(
                    affiliationColor, IColor::WHITE, alpha);
                Logger::I("TaskStack", "CreateAffiliatedGroupings: mColorPrimary: %08x, task: %s",
                    t->mColorPrimary, TO_CSTR(t));
                alpha -= alphaStep;
            }
        }
    }
}

// @Override
ECode TaskStack::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("TaskStack{\n");
    AutoPtr<IArrayList> taskList = mTaskList->GetTasks();
    Int32 taskCount;
    taskList->GetSize(&taskCount);
    for (Int32 i = 0; i < taskCount; i++) {
        AutoPtr<IInterface> t;
        taskList->Get(i, (IInterface**)&t);
        sb += "          ";
        sb += TO_STR(t);
        sb += "\n";
    }
    sb += "}";
    return sb.ToString(str);
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
