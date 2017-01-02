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

#include "elastos/droid/webkit/CMimeTypeMapHelper.h"
#include "elastos/droid/webkit/MimeTypeMap.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CMimeTypeMapHelper, Object, IMimeTypeMapHelper);

CAR_SINGLETON_IMPL(CMimeTypeMapHelper);

ECode CMimeTypeMapHelper::GetFileExtensionFromUrl(
    /* [in] */ const String& url,
    /* [out] */ String* fileExtension)
{
    VALIDATE_NOT_NULL(fileExtension);
    *fileExtension = MimeTypeMap::GetFileExtensionFromUrl(url);
    return NOERROR;
}

ECode CMimeTypeMapHelper::GetSingleton(
    /* [out] */ IMimeTypeMap** mimeTypeMap)
{
    VALIDATE_NOT_NULL(mimeTypeMap);
    *mimeTypeMap = MimeTypeMap::sMimeTypeMap;
    REFCOUNT_ADD(*mimeTypeMap);
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos