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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECBUFFERINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECBUFFERINFO_H__

#include "_Elastos_Droid_Media_CMediaCodecBufferInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecBufferInfo)
    , public Object
    , public IMediaCodecBufferInfo
{
public:
    CMediaCodecBufferInfo();

    virtual ~CMediaCodecBufferInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI Set(
        /* [in] */ Int32 newOffset,
        /* [in] */ Int32 newSize,
        /* [in] */ Int64 newTimeUs,
        /* [in] */ Int32 newFlags);

    CARAPI SetOffset(
        /* [in] */ Int32 offset);

    CARAPI SetSize(
        /* [in] */ Int32 newSize);

    CARAPI SetPresentationTimeUs(
        /* [in] */ Int64 newTimeUs);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetOffset(
        /* [out] */ Int32* offset);

    CARAPI GetSize(
        /* [out] */ Int32* newSize);

    CARAPI GetPresentationTimeUs(
        /* [out] */ Int64* newTimeUs);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

private:
    Int32 mOffset;
    Int32 mSize;
    Int64 mPresentationTimeUs;
    Int32 mFlags;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECBUFFERINFO_H__
