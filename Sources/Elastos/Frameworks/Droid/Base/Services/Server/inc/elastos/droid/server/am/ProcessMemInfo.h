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

#ifndef __ELASTOS_DROID_SERVER_AM_PROCESSMEMINFO_H__
#define __ELASTOS_DROID_SERVER_AM_PROCESSMEMINFO_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ProcessMemInfo
    : public Object
    , public IProcessMemInfo
{
public:
    ProcessMemInfo(
        /* [in] */ const String& name,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 oomAdj,
        /* [in] */ Int32 procState,
        /* [in] */ const String& adjType,
        /* [in] */ const String& adjReason);

    CAR_INTERFACE_DECL()

public:
    const String mName;
    const Int32 mPid;
    const Int32 mOomAdj;
    const Int32 mProcState;
    const String mAdjType;
    const String mAdjReason;
    Int64 mPss;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_PROCESSMEMINFO_H__
