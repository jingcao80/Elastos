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

#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTBOOKMARKS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTBOOKMARKS_H__

#include "_Elastos_Droid_Provider_CBrowserContractBookmarks.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractBookmarks)
    , public Singleton
    , public IBrowserContractBookmarks
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URI for the default folder
     * @hide
     */
    CARAPI GetCONTENT_URI_DEFAULT_FOLDER(
        /* [out] */ IUri** uri);

    /**
     * Builds a URI that points to a specific folder.
     * @param folderId the ID of the folder to point to
     * @hide
     */
    CARAPI BuildFolderUri(
        /* [in] */ Int64 folderId,
        /* [out] */ IUri** uri);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTBOOKMARKS_H__
