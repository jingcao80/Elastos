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

#ifndef __ELASTOS_DROID_OPENGL_CGLES10EXT_H__
#define __ELASTOS_DROID_OPENGL_CGLES10EXT_H__

#include "Elastos.Droid.Opengl.h"
#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_Droid_Opengl_CGLES10Ext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IBuffer;
using Elastos::IO::IInt32Buffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES10Ext)
    , public Singleton
    , public IGLES10Ext
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GlQueryMatrixxOES(
        /* [in] */ ArrayOf<Int32>* mantissa_ref,
        /* [in] */ Int32 mantissaOffset,
        /* [in] */ ArrayOf<Int32>* exponent,
        /* [in] */ Int32 exponentOffset,
        /* [out] */ Int32* matrixx);

    CARAPI GlQueryMatrixxOES(
        /* [in] */ Elastos::IO::IInt32Buffer* mantissa_buf,
        /* [in] */ Elastos::IO::IInt32Buffer* exponent_buf,
        /* [out] */ Int32* matrixx);

private:
    CARAPI GetPointer(
        /* [in] */ IBuffer* buffer,
        /* [in, out] */ HANDLE* array,
        /* [in, out] */ Int32* remaining,
        /* [in, out] */ Int32* offset,
        /* [out] */ HANDLE* rst);
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES10EXT_H__
