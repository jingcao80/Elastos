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

#ifndef __ELASTOS_DROID_SERVER_WM_TASKGROUP_H__
#define __ELASTOS_DROID_SERVER_WM_TASKGROUP_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.View.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::View::IApplicationToken;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class TaskGroup : public Object
{
public:
    TaskGroup()
        : mTaskId(-1)
    {}

    CARAPI ToString(
        /* [out] */ String* str);

public:
    Int32 mTaskId;
    List<AutoPtr<IApplicationToken> > mTokens;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_TASKGROUP_H__
