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

#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOPLAYLISTSMEMBERS_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOPLAYLISTSMEMBERS_H__

#include "_Elastos_Droid_Provider_CMediaStoreAudioPlaylistsMembers.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreAudioPlaylistsMembers)
    , public Singleton
    , public IMediaStoreAudioPlaylistsMembers
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 playlistId,
        /* [out] */ IUri** uri);

    /**
     * Convenience method to move a playlist item to a new location
     * @param res The content resolver to use
     * @param playlistId The numeric id of the playlist
     * @param from The position of the item to move
     * @param to The position to move the item to
     * @return true on success
     */
    CARAPI MoveItem(
        /* [in] */ IContentResolver* res,
        /* [in] */ Int64 playlistId,
        /* [in] */ Int32 from,
        /* [in] */ Int32 to,
        /* [out] */ Boolean* result);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREAUDIOPLAYLISTSMEMBERS_H__
