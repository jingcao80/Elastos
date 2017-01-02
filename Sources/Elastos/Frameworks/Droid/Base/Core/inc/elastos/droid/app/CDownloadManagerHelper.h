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

#ifndef __ELASTOS_DROID_APP_CDOWNLOADMANAGER_HELPER_H__
#define __ELASTOS_DROID_APP_CDOWNLOADMANAGER_HELPER_H__

#include "_Elastos_Droid_App_CDownloadManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CDownloadManagerHelper)
    , public Singleton
    , public IDownloadManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetUNDERLYINGCOLUMNS(
        /* [out] */ ArrayOf<String>** columns);
    /**
     * Returns maximum size, in bytes, of downloads that may go over a mobile connection; or null if
     * there's no limit
     *
     * @param context the {@link Context} to use for accessing the {@link ContentResolver}
     * @return maximum size, in bytes, of downloads that may go over a mobile connection; or null if
     * there's no limit
     */
    CARAPI GetMaxBytesOverMobile(
        /* [in] */ IContext* context,
        /* [out] */ Int64* size);

    /**
     * Returns recommended maximum size, in bytes, of downloads that may go over a mobile
     * connection; or null if there's no recommended limit.  The user will have the option to bypass
     * this limit.
     *
     * @param context the {@link Context} to use for accessing the {@link ContentResolver}
     * @return recommended maximum size, in bytes, of downloads that may go over a mobile
     * connection; or null if there's no recommended limit.
     */
    CARAPI GetRecommendedMaxBytesOverMobile(
        /* [in] */ IContext* context,
        /* [out] */ Int64* size);

    /** {@hide} */
    CARAPI IsActiveNetworkExpensive(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* actived);

    /** {@hide} */
    CARAPI GetActiveNetworkWarningBytes(
        /* [in] */ IContext* context,
        /* [out] */ Int64* bytes);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CDOWNLOADMANAGER_HELPER_H__
