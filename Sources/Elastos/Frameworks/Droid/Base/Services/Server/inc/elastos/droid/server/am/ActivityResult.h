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

#ifndef __ELASTOS_DROID_SERVER_AM_ACTIVITYRESULT_H__
#define __ELASTOS_DROID_SERVER_AM_ACTIVITYRESULT_H__

#include "Elastos.Droid.App.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IResultInfo;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityRecord;

class ActivityResult : public Object
{
public:
    ActivityResult(
        /* [in] */ ActivityRecord* from,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    ~ActivityResult();

public:
    AutoPtr<IResultInfo> mResultInfo;
    ActivityRecord* mFrom;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_ACTIVITYRESULT_H__
