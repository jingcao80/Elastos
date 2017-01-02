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

#ifndef __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAMHELPER_H__
#define __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAMHELPER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>
#include "_Elastos_Droid_Speech_Srec_CUlawEncoderInputStreamHelper.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CarClass(CUlawEncoderInputStreamHelper)
    , public Singleton
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    CARAPI Encode(
        /* [in] */ ArrayOf<Byte>* pcmBuf,
        /* [in] */ Int32 pcmOffset,
        /* [in] */ ArrayOf<Byte>* ulawBuf,
        /* [in] */ Int32 ulawOffset,
        /* [in] */ Int32 length,
        /* [in] */ Int32 max);

    CARAPI MaxAbsPcm(
        /* [in] */ ArrayOf<Byte>* pcmBuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* ret);
};

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_CULAWENCODERINPUTSTREAMHELPER_H__
