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

#include "MonkeyRotationEvent.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyRotationEvent::MonkeyRotationEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ROTATION)
    , mRotationDegree(0)
    , mPersist(FALSE)
{}

MonkeyRotationEvent::MonkeyRotationEvent(
    /* [in] */ Int32 degree,
    /* [in] */ Boolean persist)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ROTATION)
    , mRotationDegree(degree)
    , mPersist(persist)
{}

ECode MonkeyRotationEvent::Init(
    /* [in] */ Int32 degree,
    /* [in] */ Boolean persist)
{
    Int32 *pDegree = &mRotationDegree;
    *pDegree = degree;
    Boolean *pPersist = (Boolean*)&mPersist;
    *pPersist = persist;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ROTATION);
    return NOERROR;
}

Int32 MonkeyRotationEvent::InjectEvent(
    /* [in] */ IIWindowManager* iwm,
    /* [in] */ IIActivityManager* iam,
    /* [in] */ Int32 verbose)
{
    if (verbose > 0) {
        String str;
        AutoPtr<IBoolean> tmp;
        CBoolean::New(mPersist, (IBoolean**)&tmp);
        tmp->ToString(&str);
        PFL_EX(":Sending rotation degree= %s, persist = %s",
            StringUtils::Int32ToString(mRotationDegree).string(), str.string())
    }

    //inject rotation event
    iwm->FreezeRotation(mRotationDegree);
    if (!mPersist) {
        iwm->ThawRotation();
    }
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

