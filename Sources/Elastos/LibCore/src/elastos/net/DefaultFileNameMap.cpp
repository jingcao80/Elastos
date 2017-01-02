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

#include "DefaultFileNameMap.h"
#include "CMimeUtils.h"

using Libcore::Net::IMimeUtils;
using Libcore::Net::CMimeUtils;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(DefaultFileNameMap, Object, IFileNameMap)

ECode DefaultFileNameMap::GetContentTypeFor(
    /* [in] */ const String& filename,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    AutoPtr<IMimeUtils> utils;
    FAIL_RETURN(CMimeUtils::AcquireSingleton((IMimeUtils**)&utils));
    if (filename.EndWith("/")) {
         //a directory, return html
        return utils->GuessMimeTypeFromExtension(String("html"), type);
    }
    Int32 lastCharInExtension = filename.LastIndexOf('#');
    if (lastCharInExtension < 0) {
        lastCharInExtension = filename.GetLength();
    }
    Int32 firstCharInExtension = filename.LastIndexOf('.') + 1;
    String ext("");
    if (firstCharInExtension > filename.LastIndexOf('/')) {
        ext = filename.Substring(firstCharInExtension, lastCharInExtension).ToLowerCase();
    }
    return utils->GuessMimeTypeFromExtension(ext, type);
}

} // namespace Net
} // namespace Elastos
