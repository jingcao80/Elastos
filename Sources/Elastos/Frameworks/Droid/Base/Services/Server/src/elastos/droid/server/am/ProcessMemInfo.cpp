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

#include "elastos/droid/server/am/ProcessMemInfo.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(ProcessMemInfo, Object, IProcessMemInfo)

ProcessMemInfo::ProcessMemInfo(
    /* [in] */ const String& name,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 oomAdj,
    /* [in] */ Int32 procState,
    /* [in] */ const String& adjType,
    /* [in] */ const String& adjReason)
    : mName(name)
    , mPid(pid)
    , mOomAdj(oomAdj)
    , mProcState(procState)
    , mAdjType(adjType)
    , mAdjReason(adjReason)
    , mPss(0)
{
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
