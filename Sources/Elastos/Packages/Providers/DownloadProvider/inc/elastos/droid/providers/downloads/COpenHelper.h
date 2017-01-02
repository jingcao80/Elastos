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

#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_COPENHELPER_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_COPENHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_COpenHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

CarClass(COpenHelper)
    , public Singleton
    , public IOpenHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Build and start an {@link Intent} to view the download with given ID,
     * handling subtleties around installing packages.
     */
    CARAPI StartViewIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id,
        /* [in] */ Int32 intentFlags,
        /* [out] */ Boolean* result);

private:
    /**
     * Build an {@link Intent} to view the download with given ID, handling
     * subtleties around installing packages.
     */
    static CARAPI_(AutoPtr<IIntent>) BuildViewIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    static CARAPI_(AutoPtr<IUri>) GetRefererUri(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    static CARAPI_(Int32) GetOriginatingUid(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    static CARAPI_(String) GetCursorString(
        /* [in] */ ICursor* cursor,
        /* [in] */ String column);

    static CARAPI_(AutoPtr<IUri>) GetCursorUri(
        /* [in] */ ICursor* cursor,
        /* [in] */ String column);

    static CARAPI_(AutoPtr<IFile>) GetCursorFile(
        /* [in] */ ICursor* cursor,
        /* [in] */ String column);
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_COPENHELPER_H__
