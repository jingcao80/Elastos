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

#include "CMimeUtils.h"
#include "MimeUtils.h"

namespace Libcore {
namespace Net {

CAR_INTERFACE_IMPL(CMimeUtils, Singleton, IMimeUtils)

CAR_SINGLETON_IMPL(CMimeUtils)

ECode CMimeUtils::HasMineType(
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean *result)
{
    return MimeUtils::HasMineType(mimeType, result);
}

ECode CMimeUtils::GuessMimeTypeFromExtension(
    /* [in] */ const String& extension,
    /* [out] */ String* result)
{
    return MimeUtils::GuessMimeTypeFromExtension(extension, result);
}

ECode CMimeUtils::HasExtension(
    /* [in] */ const String& extension,
    /* [out] */ Boolean* result)
{
    return MimeUtils::HasExtension(extension, result);
}

ECode CMimeUtils::GuessExtensionFromMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ String* result)
{
    return MimeUtils::GuessExtensionFromMimeType(mimeType, result);
}

} // namespace Net
} // namespace Libcore