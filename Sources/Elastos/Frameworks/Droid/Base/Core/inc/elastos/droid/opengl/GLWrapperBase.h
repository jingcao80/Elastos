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

#ifndef __ELASTOS_DROID_OPENGL_GLWRAPPERBASE_H__
#define __ELASTOS_DROID_OPENGL_GLWRAPPERBASE_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;

using Elastosx::Microedition::Khronos::Opengles::IGL;
using Elastosx::Microedition::Khronos::Opengles::IGL10;
using Elastosx::Microedition::Khronos::Opengles::IGL10Ext;
using Elastosx::Microedition::Khronos::Opengles::IGL11;
using Elastosx::Microedition::Khronos::Opengles::IGL11Ext;
using Elastosx::Microedition::Khronos::Opengles::IGL11ExtensionPack;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLWrapperBase
    : public Object
    , public IGL
    , public IGL10
    , public IGL10Ext
    , public IGL11
    , public IGL11Ext
    , public IGL11ExtensionPack
{
public:
    GLWrapperBase(
        /* [in] */ IGL* gl);

    CAR_INTERFACE_DECL()

protected:
    AutoPtr<IGL10> mgl;
    AutoPtr<IGL10Ext> mgl10Ext;
    AutoPtr<IGL11> mgl11;
    AutoPtr<IGL11Ext> mgl11Ext;
    AutoPtr<IGL11ExtensionPack> mgl11ExtensionPack;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif
