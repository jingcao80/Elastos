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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/mtp/CMtpConstants.h"

namespace Elastos {
namespace Droid {
namespace Mtp {

CAR_INTERFACE_IMPL(CMtpConstants, Singleton, IMtpConstants)

CAR_SINGLETON_IMPL(CMtpConstants)

ECode CMtpConstants::IsAbstractObject(
        /* [in] */ Int32 format,
        /* [out] */ Boolean* result)
{
    switch (format) {
        case IMtpConstants::FORMAT_ABSTRACT_MULTIMEDIA_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_IMAGE_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_AUDIO_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_VIDEO_ALBUM:
        case IMtpConstants::FORMAT_ABSTRACT_AV_PLAYLIST:
        case IMtpConstants::FORMAT_ABSTRACT_AUDIO_PLAYLIST:
        case IMtpConstants::FORMAT_ABSTRACT_VIDEO_PLAYLIST:
        case IMtpConstants::FORMAT_ABSTRACT_MEDIACAST:
        case IMtpConstants::FORMAT_ABSTRACT_DOCUMENT:
            *result = TRUE; break;
        default:
            *result = FALSE;
    }
    return NOERROR;
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
