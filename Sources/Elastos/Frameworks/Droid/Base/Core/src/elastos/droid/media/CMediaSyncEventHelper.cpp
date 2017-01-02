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

#include "elastos/droid/media/CMediaSyncEventHelper.h"
#include "elastos/droid/media/MediaSyncEvent.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CMediaSyncEventHelper)

CAR_INTERFACE_IMPL(CMediaSyncEventHelper, Singleton, IMediaSyncEventHelper)

ECode CMediaSyncEventHelper::CreateEvent(
    /* [in] */ Int32 eventType,
    /* [out] */ IMediaSyncEvent** result)
{
    return MediaSyncEvent::CreateEvent(eventType, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
