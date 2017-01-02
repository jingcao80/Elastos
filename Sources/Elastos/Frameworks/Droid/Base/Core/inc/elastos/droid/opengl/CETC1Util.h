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

#ifndef __ELASTOS_DROID_OPENGL_CETC1UTIL_H__
#define __ELASTOS_DROID_OPENGL_CETC1UTIL_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CETC1Util.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CETC1Util)
    , public Singleton
    , public IETC1Util
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI LoadTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 border,
        /* [in] */ Int32 fallbackFormat,
        /* [in] */ Int32 fallbackType,
        /* [in] */ Elastos::IO::IInputStream* input);

    CARAPI LoadTexture(
        /* [in] */ Int32 target,
        /* [in] */ Int32 level,
        /* [in] */ Int32 border,
        /* [in] */ Int32 fallbackFormat,
        /* [in] */ Int32 fallbackType,
        /* [in] */ IETC1Texture* texture);

    CARAPI IsETC1Supported(
        /* [out] */ Boolean* isSupported);

    CARAPI CreateTexture(
        /* [in] */ Elastos::IO::IInputStream* input,
        /* [out] */ IETC1Texture** texture);

    CARAPI CompressTexture(
        /* [in] */ Elastos::IO::IBuffer* input,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 pixelSize,
        /* [in] */ Int32 stride,
        /* [out] */ IETC1Texture** texture);

    CARAPI WriteTexture(
        /* [in] */ IETC1Texture* texture,
        /* [in] */ Elastos::IO::IOutputStream* output);

private:
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CETC1UTIL_H__
