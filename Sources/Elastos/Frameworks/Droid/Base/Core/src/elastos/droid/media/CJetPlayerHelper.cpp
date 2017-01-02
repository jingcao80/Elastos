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

#include "elastos/droid/media/CJetPlayer.h"
#include "elastos/droid/media/CJetPlayerHelper.h"

using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CJetPlayerHelper, Singleton, IJetPlayerHelper)

CAR_SINGLETON_IMPL(CJetPlayerHelper)

ECode CJetPlayerHelper::GetJetPlayer(
    /* [out] */ IJetPlayer** player)
{
    VALIDATE_NOT_NULL(player);

    AutoPtr<IJetPlayer> obj = CJetPlayer::GetJetPlayer();
    *player = obj;
    REFCOUNT_ADD(*player);
    return NOERROR;
}

ECode CJetPlayerHelper::GetMaxTracks(
    /* [out] */ Int32 * tracks)
{
    VALIDATE_NOT_NULL(tracks);

    *tracks = CJetPlayer::GetMaxTracks();
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
