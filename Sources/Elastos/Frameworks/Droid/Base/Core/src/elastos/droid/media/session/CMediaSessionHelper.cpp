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

#include "elastos/droid/media/session/CMediaSessionHelper.h"
#include "elastos/droid/media/session/CMediaSession.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CMediaSessionHelper, Singleton, IMediaSessionHelper)

CAR_SINGLETON_IMPL(CMediaSessionHelper)

ECode CMediaSessionHelper::IsActiveState(
    /* [in] */ Int32 state,
    /* [out] */ Boolean * result)
{
    return CMediaSession::IsActiveState(state, result);
}


} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos
