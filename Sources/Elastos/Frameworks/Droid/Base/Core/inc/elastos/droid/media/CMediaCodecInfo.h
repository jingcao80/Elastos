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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFO_H__

#include "_Elastos_Droid_Media_CMediaCodecInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IRational;
using Elastos::Droid::Utility::IRange;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfo)
    , public Object
    , public IMediaCodecInfo
{
private:
    class Feature
        : public Object
    {
    public:
        Feature(
            /* [in] */ const String& name,
            /* [in] */ Int32 value,
            /* [in] */ Boolean def)
            : mName(name)
            , mValue(value)
            , mDefault(def)
        {}

    public:
        String mName;
        Int32 mValue;
        Boolean mDefault;
    };

public:
    friend class CMediaCodecInfoAudioCapabilities;
    friend class CMediaCodecInfoCodecCapabilities;
    friend class CMediaCodecInfoEncoderCapabilities;
    friend class CMediaCodecInfoVideoCapabilities;

    CMediaCodecInfo();

    virtual ~CMediaCodecInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Boolean isEncoder,
        /* [in] */ ArrayOf<IMediaCodecInfoCodecCapabilities*>* caps);

    /**
     * Retrieve the codec name.
     */
    CARAPI GetName(
        /* [out] */ String* result);

    /**
     * Query if the codec is an encoder.
     */
    CARAPI IsEncoder(
        /* [out] */ Boolean* result);

    /**
     * Query the media types supported by the codec.
     */
    CARAPI GetSupportedTypes(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI GetCapabilitiesForType(
        /* [in] */ const String& type,
        /* [out] */ IMediaCodecInfoCodecCapabilities** result);

    CARAPI MakeRegular(
        /* [out] */ IMediaCodecInfo** result);

private:
    static CARAPI CheckPowerOfTwo(
        /* [in] */ Int32 value,
        /* [in] */ const String& message,
        /* [out] */ Int32* result);

    static CARAPI_(void) Init();

private:
    // COMMON CONSTANTS
    static AutoPtr<IRange> POSITIVE_INTEGERS;
    static AutoPtr<IRange> POSITIVE_LONGS;
    static AutoPtr<IRange> POSITIVE_RATIONALS;
    static AutoPtr<IRange> SIZE_RANGE;
    static AutoPtr<IRange> FRAME_RATE_RANGE;

    // found stuff that is not supported by framework (=> this should not happen)
    static const Int32 ERROR_UNRECOGNIZED;
    // found profile/level for which we don't have capability estimates
    static const Int32 ERROR_UNSUPPORTED;
    // have not found any profile/level for which we don't have capability estimate
    static const Int32 ERROR_NONE_SUPPORTED;

    Boolean mIsEncoder;
    String mName;
    AutoPtr<IMap> mCaps;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECINFO_H__
