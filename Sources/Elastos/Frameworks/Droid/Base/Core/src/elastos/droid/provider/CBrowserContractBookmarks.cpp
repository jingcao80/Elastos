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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/BrowserContract.h"
#include "elastos/droid/provider/CBrowserContractBookmarks.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractBookmarks)

CAR_INTERFACE_IMPL(CBrowserContractBookmarks, Object, IBrowserContractBookmarks)

ECode CBrowserContractBookmarks::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Bookmarks::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowserContractBookmarks::GetCONTENT_URI_DEFAULT_FOLDER(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::Bookmarks::CONTENT_URI_DEFAULT_FOLDER;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode CBrowserContractBookmarks::BuildFolderUri(
    /* [in] */ Int64 folderId,
    /* [out] */ IUri** uri)
{
    return BrowserContract::Bookmarks::BuildFolderUri(folderId, uri);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
