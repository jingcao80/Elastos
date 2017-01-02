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

#include "elastos/droid/server/am/UserStartedState.h"
#include "Elastos.CoreLibrary.IO.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 UserStartedState::STATE_BOOTING = 0;
const Int32 UserStartedState::STATE_RUNNING = 1;
const Int32 UserStartedState::STATE_STOPPING = 2;
const Int32 UserStartedState::STATE_SHUTDOWN = 3;

CAR_INTERFACE_IMPL(UserStartedState, Object, IUserStartedState)

UserStartedState::UserStartedState(
    /* [in] */ IUserHandle* handle,
    /* [in] */ Boolean initial)
    : mHandle(handle)
    , mState(STATE_BOOTING)
    , mSwitching(FALSE)
    , mInitializing(FALSE)
{}

void UserStartedState::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IPrintWriter* pw)
{
    pw->Print(prefix);
    pw->Print(String("mState="));
    switch (mState) {
        case STATE_BOOTING:
            pw->Print(String("BOOTING")); break;
        case STATE_RUNNING:
            pw->Print(String("RUNNING")); break;
        case STATE_STOPPING:
            pw->Print(String("STOPPING")); break;
        case STATE_SHUTDOWN:
            pw->Print(String("SHUTDOWN")); break;
        default:
            pw->Print(mState); break;
    }
    if (mSwitching)
        pw->Print(String(" SWITCHING"));
    if (mInitializing)
        pw->Print(String(" INITIALIZING"));
    pw->Println();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
