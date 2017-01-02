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

#include "elastos/droid/hardware/camera2/utils/TaskSingleDrainer.h"
#include "elastos/droid/hardware/camera2/utils/CTaskDrainer.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(TaskSingleDrainer, Object, ITaskSingleDrainer)

TaskSingleDrainer::TaskSingleDrainer()
{
}

TaskSingleDrainer::~TaskSingleDrainer()
{
}

ECode TaskSingleDrainer::constructor()
{
    AutoPtr<Object> obj = new Object();
    mSingleTask = TO_IINTERFACE(obj);
    return NOERROR;
}

ECode TaskSingleDrainer::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ ITaskDrainerDrainListener* listener)
{
    return CTaskDrainer::New(handler, listener, (ITaskDrainer**)&mTaskDrainer);
}

ECode TaskSingleDrainer::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ ITaskDrainerDrainListener* listener,
    /* [in] */ const String& name)
{
    return CTaskDrainer::New(handler, listener, name, (ITaskDrainer**)&mTaskDrainer);
}

ECode TaskSingleDrainer::TaskStarted()
{
    return mTaskDrainer->TaskStarted(mSingleTask);
}

ECode TaskSingleDrainer::TaskFinished()
{

    return mTaskDrainer->TaskFinished(mSingleTask);
}

ECode TaskSingleDrainer::BeginDrain()
{
    return mTaskDrainer->BeginDrain();
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos