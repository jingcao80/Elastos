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

#ifndef __ELASTOS_DROID_PROVIDER_CDOWNLOADSIMPL_H__
#define __ELASTOS_DROID_PROVIDER_CDOWNLOADSIMPL_H__

#include "_Elastos_Droid_Provider_CDownloadsImpl.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CDownloadsImpl)
    , public Singleton
    , public IDownloadsImpl
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// URI to access downloads owned by the caller's UID.
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content URI for accessing all downloads across all UIDs (requires the
     * ACCESS_ALL_DOWNLOADS permission).
     */
    CARAPI GetALL_DOWNLOADS_CONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content URI for accessing publicly accessible downloads (i.e., it requires no
     * permissions to access this downloaded file)
     */
    CARAPI GetPUBLICLY_ACCESSIBLE_DOWNLOADS_URI(
        /* [out] */ IUri** uri);

    /**
     * Returns whether the status is informational (i.e. 1xx).
     */
    CARAPI IsStatusInformational(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* res);

    /**
     * Returns whether the status is a success (i.e. 2xx).
     */
    CARAPI IsStatusSuccess(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* res);

    /**
     * Returns whether the status is an error (i.e. 4xx or 5xx).
     */
    CARAPI IsStatusError(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* res);

    /**
     * Returns whether the status is a client error (i.e. 4xx).
     */
    CARAPI IsStatusClientError(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* res);

    /**
     * Returns whether the status is a server error (i.e. 5xx).
     */
    CARAPI IsStatusServerError(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* res);

    /**
     * this method determines if a notification should be displayed for a
     * given {@link #COLUMN_VISIBILITY} value
     * @param visibility the value of {@link #COLUMN_VISIBILITY}.
     * @return true if the notification should be displayed. false otherwise.
     */
    CARAPI IsNotificationToBeDisplayed(
        /* [in] */ Int32 visibility,
        /* [out] */ Boolean* res);

    /**
     * Returns whether the download has completed (either with success or
     * error).
     */
    CARAPI IsStatusCompleted(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* res);

    /** {@hide} */
    CARAPI StatusToString(
        /* [in] */ Int32 status,
        /* [out] */ String* value);
};

}
}
}

#endif //__ELASTOS_DROID_PROVIDER_CDOWNLOADSIMPL_H__
