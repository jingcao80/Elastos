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

#ifndef __ELASTOS_DROID_PROVIDER_CMEDIASTOREVIDEOTHUMBNAILS_H__
#define __ELASTOS_DROID_PROVIDER_CMEDIASTOREVIDEOTHUMBNAILS_H__

#include "_Elastos_Droid_Provider_CMediaStoreVideoThumbnails.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CMediaStoreVideoThumbnails)
    , public Singleton
    , public IMediaStoreVideoThumbnails
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for the internal storage.
     */
    CARAPI GetINTERNAL_CONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URI for the "primary" external storage
     * volume.
     */
    CARAPI GetEXTERNAL_CONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * This method cancels the thumbnail request so clients waiting for getThumbnail will be
     * interrupted and return immediately. Only the original process which made the getThumbnail
     * requests can cancel their own requests.
     *
     * @param cr ContentResolver
     * @param origId original video id
     */
    CARAPI CancelThumbnailRequest(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 origId);

    /**
     * This method checks if the thumbnails of the specified image (origId) has been created.
     * It will be blocked until the thumbnails are generated.
     *
     * @param cr ContentResolver used to dispatch queries to MediaProvider.
     * @param origId Original image id associated with thumbnail of interest.
     * @param kind The type of thumbnail to fetch. Should be either MINI_KIND or MICRO_KIND.
     * @param options this is only used for MINI_KIND when decoding the Bitmap
     * @return A Bitmap instance. It could be null if the original image
     *         associated with origId doesn't exist or memory is not enough.
     */
    CARAPI GetThumbnail(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 origId,
        /* [in] */ Int32 kind,
        /* [in] */ IBitmapFactoryOptions* options,
        /* [out] */ IBitmap** bitmap);

    /**
     * This method checks if the thumbnails of the specified image (origId) has been created.
     * It will be blocked until the thumbnails are generated.
     *
     * @param cr ContentResolver used to dispatch queries to MediaProvider.
     * @param origId Original image id associated with thumbnail of interest.
     * @param groupId the id of group to which this request belongs
     * @param kind The type of thumbnail to fetch. Should be either MINI_KIND or MICRO_KIND
     * @param options this is only used for MINI_KIND when decoding the Bitmap
     * @return A Bitmap instance. It could be null if the original image associated with
     *         origId doesn't exist or memory is not enough.
     */
    CARAPI GetThumbnail(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 origId,
        /* [in] */ Int64 groupId,
        /* [in] */ Int32 kind,
        /* [in] */ IBitmapFactoryOptions* options,
        /* [out] */ IBitmap** bitmap);

    /**
     * This method cancels the thumbnail request so clients waiting for getThumbnail will be
     * interrupted and return immediately. Only the original process which made the getThumbnail
     * requests can cancel their own requests.
     *
     * @param cr ContentResolver
     * @param origId original video id
     * @param groupId the same groupId used in getThumbnail.
     */
    CARAPI CancelThumbnailRequest(
        /* [in] */ IContentResolver* cr,
        /* [in] */ Int64 origId,
        /* [in] */ Int64 groupId);

    /**
     * Get the content:// style URI for the image media table on the
     * given volume.
     *
     * @param volumeName the name of the volume to get the URI for
     * @return the URI to the image media table on the given volume
     */
    CARAPI GetContentUri(
        /* [in] */ const String& volumeName,
        /* [out] */ IUri** uri);
};

}
}
}

#endif //__ELASTOS_DROID_PROVIDER_CMEDIASTOREVIDEOTHUMBNAILS_H__
