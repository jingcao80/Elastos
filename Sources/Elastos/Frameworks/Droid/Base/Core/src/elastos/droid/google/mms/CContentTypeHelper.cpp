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

#include "elastos/droid/google/mms/CContentTypeHelper.h"
#include "elastos/droid/google/mms/ContentType.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

CAR_INTERFACE_IMPL(CContentTypeHelper, Singleton, IContentTypeHelper)
CAR_SINGLETON_IMPL(CContentTypeHelper)


ECode CContentTypeHelper::IsSupportedType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsSupportedType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsSupportedImageType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsSupportedImageType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsSupportedAudioType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsSupportedAudioType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsSupportedVideoType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsSupportedVideoType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsTextType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsSupportedVideoType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsImageType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsImageType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsAudioType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsAudioType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsVideoType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsVideoType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsDrmType(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsDrmType(contentType);
    return NOERROR;
}

ECode CContentTypeHelper::IsUnspecified(
    /* [in] */ const String& contentType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ContentType::IsUnspecified(contentType);
    return NOERROR;
}

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
