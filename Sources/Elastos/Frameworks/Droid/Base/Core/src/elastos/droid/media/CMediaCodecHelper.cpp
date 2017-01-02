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
#include "elastos/droid/media/CMediaCodec.h"
#include "elastos/droid/media/CMediaCodecHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecHelper, Singleton, IMediaCodecHelper)

CAR_SINGLETON_IMPL(CMediaCodecHelper)

ECode CMediaCodecHelper::CreateDecoderByType(
    /* [in] */ const String& type,
    /* [out] */ IMediaCodec** result)
{
    return CMediaCodec::CreateDecoderByType(type, result);
}

ECode CMediaCodecHelper::CreateEncoderByType(
    /* [in] */ const String& type,
    /* [out] */ IMediaCodec** result)
{
    return CMediaCodec::CreateEncoderByType(type, result);
}

ECode CMediaCodecHelper::CreateByCodecName(
    /* [in] */ const String& name,
    /* [out] */ IMediaCodec** result)
{
    return CMediaCodec::CreateByCodecName(name, result);
}


} // namespace Media
} // namepsace Droid
} // namespace Elastos
