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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKGROUPING_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKGROUPING_H__

#include "elastos/droid/systemui/recents/model/Task.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::IArrayList;
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
    CAR_INTERFACE_DECL()

    /** Creates a group with a specified affiliation. */
    TaskGrouping(
        /* [in] */ Int32 affiliation);

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

    CARAPI ToString(
        /* [out] */ String* str);

private:
    /** Updates the mapping of tasks to indices. */
    CARAPI_(void) UpdateTaskIndices();

public:
    Int32 mAffiliation;
    Int64 mLatestActiveTimeInGroup;

    AutoPtr<Task::TaskKey> mFrontMostTaskKey;
    AutoPtr<IArrayList> mTaskKeys; // item is Task::TaskKey
    HashMap<AutoPtr<ITaskKey>, Int32> mTaskKeyIndices;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_TASKGROUPING_H__
