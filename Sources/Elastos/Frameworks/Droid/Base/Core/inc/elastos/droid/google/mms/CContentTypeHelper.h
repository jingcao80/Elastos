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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_CCONTENTTYPEHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_CCONTENTTYPEHELPER_H__

#include "_Elastos_Droid_Google_Mms_CContentTypeHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

CarClass(CContentTypeHelper)
    , public Singleton
    , public IContentTypeHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI IsSupportedType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsSupportedImageType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsSupportedAudioType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsSupportedVideoType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsTextType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsImageType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsAudioType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsVideoType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsDrmType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsUnspecified(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);
};

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_CCONTENTTYPEHELPER_H__
