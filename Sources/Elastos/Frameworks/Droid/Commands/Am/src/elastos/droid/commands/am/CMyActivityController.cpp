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

#include "CMyActivityController.h"

using Elastos::Droid::App::EIID_IIActivityController;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CAR_INTERFACE_IMPL_2(CMyActivityController, Object, IIActivityController, IBinder);

CAR_OBJECT_IMPL(CMyActivityController)

ECode CMyActivityController::constructor(
    /* [in] */ IIActivityManager* am,
    /* [in] */ const String& gdbPort)
{
    return Am::MyActivityController::constructor(am, gdbPort);
}

ECode CMyActivityController::ActivityResuming(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    return Am::MyActivityController::ActivityResuming(pkg, result);
}

ECode CMyActivityController::ActivityStarting(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    return Am::MyActivityController::ActivityStarting(intent, pkg, result);
}

ECode CMyActivityController::AppCrashed(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ Int64 timeMillis,
    /* [in] */ const String& stackTrace,
    /* [out] */ Boolean* result)
{
    return Am::MyActivityController::AppCrashed(
            processName, pid, shortMsg, longMsg, timeMillis, stackTrace, result);
}

ECode CMyActivityController::AppEarlyNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& annotation,
    /* [out] */ Int32* result)
{
    return Am::MyActivityController::AppEarlyNotResponding(
            processName, pid, annotation, result);
}

ECode CMyActivityController::AppNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& processStats,
    /* [out] */ Int32* result)
{
    return Am::MyActivityController::AppNotResponding(
            processName, pid, processStats, result);
}

ECode CMyActivityController::SystemNotResponding(
    /* [in] */ const String& message,
    /* [out] */ Int32* result)
{
    return Am::MyActivityController::SystemNotResponding(message, result);
}

ECode CMyActivityController::ToString(
    /* [out] */ String* string)
{
    return Object::ToString(string);
}

ECode CMyActivityController::Run()
{
    return Am::MyActivityController::Run();
}

}
}
}
}

