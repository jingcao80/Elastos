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

#include "elastos/droid/speech/srec/CUlawEncoderInputStreamHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/speech/srec/UlawEncoderInputStream.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

CAR_SINGLETON_IMPL(CUlawEncoderInputStreamHelper)

CAR_INTERFACE_IMPL(CUlawEncoderInputStreamHelper, Singleton, IUlawEncoderInputStreamHelper)

ECode CUlawEncoderInputStreamHelper::Encode(
    /* [in] */ ArrayOf<Byte>* pcmBuf,
    /* [in] */ Int32 pcmOffset,
    /* [in] */ ArrayOf<Byte>* ulawBuf,
    /* [in] */ Int32 ulawOffset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 max)
{
    return UlawEncoderInputStream::Encode(pcmBuf, pcmOffset, ulawBuf, ulawOffset, length, max);
}

ECode CUlawEncoderInputStreamHelper::MaxAbsPcm(
    /* [in] */ ArrayOf<Byte>* pcmBuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* ret)
{
    return UlawEncoderInputStream::MaxAbsPcm(pcmBuf, offset, length, ret);
}

} // namespace Srec
} // namespace Speech
} // namespace Droid
} // namespace Elastos

