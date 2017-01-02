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

#include "elastos/droid/media/session/CPlaybackStateHelper.h"
#include "elastos/droid/media/session/CPlaybackState.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CPlaybackStateHelper, Singleton, IPlaybackStateHelper)

CAR_SINGLETON_IMPL(CPlaybackStateHelper)

ECode CPlaybackStateHelper::GetStateFromRccState(
    /* [in] */ Int32 rccState,
    /* [out] */ Int32 * result)
{
    return CPlaybackState::GetStateFromRccState(rccState, result);
}

ECode CPlaybackStateHelper::GetRccStateFromState(
    /* [in] */ Int32 state,
    /* [out] */ Int32 * result)
{
    return CPlaybackState::GetRccStateFromState(state, result);
}

ECode CPlaybackStateHelper::GetActionsFromRccControlFlags(
    /* [in] */ Int32 rccFlags,
    /* [out] */ Int64 * result)
{
    return CPlaybackState::GetActionsFromRccControlFlags(rccFlags, result);
}

ECode CPlaybackStateHelper::GetRccControlFlagsFromActions(
    /* [in] */ Int64 actions,
    /* [out] */ Int32 * result)
{
    return CPlaybackState::GetRccControlFlagsFromActions(actions, result);
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
