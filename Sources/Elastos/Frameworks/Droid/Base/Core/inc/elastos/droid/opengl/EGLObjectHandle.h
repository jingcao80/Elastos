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

#ifndef __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__
#define __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__

#include <elastos/core/Object.h>
#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class EGLObjectHandle
    : public Object
    , public IEGLObjectHandle
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetHandle(
        /* [out] */ Int32* handle);

    CARAPI GetNativeHandle(
        /* [out] */ Int64* handle);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

protected:
    EGLObjectHandle(
        /* [in] */ Int32 handle);

    EGLObjectHandle(
        /* [in] */ Int64 handle);

private:
    Int64 mHandle;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__
