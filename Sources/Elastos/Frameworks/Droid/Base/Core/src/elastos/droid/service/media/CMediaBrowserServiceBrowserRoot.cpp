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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/service/media/CMediaBrowserServiceBrowserRoot.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

CAR_OBJECT_IMPL(CMediaBrowserServiceBrowserRoot)

CMediaBrowserServiceBrowserRoot::CMediaBrowserServiceBrowserRoot()
    : mRootId(String(NULL))
{
}

ECode CMediaBrowserServiceBrowserRoot::constructor(
    /* [in] */ const String& rootId,
    /* [in] */ IBundle * extras)
{
    if (rootId == NULL) {
        // throw new IllegalArgumentException("The root id in BrowserRoot cannot be null. " +
        //         "Use null for BrowserRoot instead.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRootId = rootId;
    mExtras = extras;
    return NOERROR;
}

/**
 * Gets the root id for browsing.
 */
ECode CMediaBrowserServiceBrowserRoot::GetRootId(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRootId;
    return NOERROR;
}

/**
 * Gets any extras about the brwoser service.
 */
ECode CMediaBrowserServiceBrowserRoot::GetExtras(
    /* [out] */ IBundle ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos
