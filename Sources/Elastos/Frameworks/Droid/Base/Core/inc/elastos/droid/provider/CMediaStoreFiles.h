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

#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREFILES_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREFILES_H__

#include "_Elastos_Droid_Provider_CMediaStoreFiles.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreFiles)
    , public Singleton
    , public IMediaStoreFiles
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Get the content:// style URI for the files table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @return the URI to the files table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);

    /**
     * Get the content:// style URI for a single row in the files table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @param rowId the file to get the URI for
     * @return the URI to the files table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 rowId,
        /* [out] */ IUri** uri);

    /**
     * For use only by the MTP implementation.
     * @hide
     */
    CARAPI GetMtpObjectsUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);

    /**
     * For use only by the MTP implementation.
     * @hide
     */
    CARAPI GetMtpObjectsUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 fileId,
        /* [out] */ IUri** uri);

    /**
     * Used to implement the MTP GetObjectReferences and SetObjectReferences commands.
     * @hide
     */
    CARAPI GetMtpReferencesUri(
        /* [in] */ const String& volumeName,
        /* [in] */ Int64 fileId,
        /* [out] */ IUri** uri);
};

} //namespace Provider
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREFILES_H__
