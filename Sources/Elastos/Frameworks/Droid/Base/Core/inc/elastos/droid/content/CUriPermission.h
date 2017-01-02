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

#ifndef __ELASTOS_DROID_CONTENT_CURIPERMISSION_H__
#define __ELASTOS_DROID_CONTENT_CURIPERMISSION_H__

#include "_Elastos_Droid_Content_CUriPermission.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Description of a single Uri permission grant. This grants may have been
 * created via {@link Intent#FLAG_GRANT_READ_URI_PERMISSION}, etc when sending
 * an {@link Intent}, or explicitly through
 * {@link Context#grantUriPermission(String, android.net.Uri, int)}.
 *
 * @see ContentResolver#getPersistedUriPermissions()
 */
CarClass(CUriPermission)
    , public Object
    , public IUriPermission
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CUriPermission();

    virtual ~CUriPermission();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ Int64 persistedTime);

    /**
     * Return the Uri this permission pertains to.
     */
    CARAPI GetUri(
        /* [out] */ IUri** uri);

    /**
     * Returns if this permission offers read access.
     */
    CARAPI IsReadPermission(
        /* [out] */ Boolean* permission);

    /**
     * Returns if this permission offers write access.
     */
    CARAPI IsWritePermission(
        /* [out] */ Boolean* permission);

    /**
     * Return the time when this permission was first persisted, in milliseconds
     * since January 1, 1970 00:00:00.0 UTC. Returns {@link #INVALID_TIME} if
     * not persisted.
     *
     * @see ContentResolver#takePersistableUriPermission(Uri, int)
     * @see System#currentTimeMillis()
     */
    CARAPI GetPersistedTime(
        /* [out] */ Int64* time);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<IUri> mUri;
    Int32 mModeFlags;
    Int64 mPersistedTime;
};

} // Content
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTENT_CURIPERMISSION_H__
