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

#include "elastos/droid/opengl/GLWrapperBase.h"

using Elastosx::Microedition::Khronos::Opengles::EIID_IGL;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11ExtensionPack;

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL_6(GLWrapperBase, Object, IGL, IGL10, IGL10Ext, IGL11, IGL11Ext, IGL11ExtensionPack)

GLWrapperBase::GLWrapperBase(
    /* [in] */ IGL* gl)
{
    mgl = IGL10::Probe(gl);
    if (IGL10Ext::Probe(gl) != NULL) {
        mgl10Ext = IGL10Ext::Probe(gl);
    }
    if (IGL11::Probe(gl) != NULL) {
        mgl11 = IGL11::Probe(gl);
    }
    if (IGL11Ext::Probe(gl) != NULL) {
        mgl11Ext = IGL11Ext::Probe(gl);
    }
    if (IGL11ExtensionPack::Probe(gl) != NULL) {
        mgl11ExtensionPack = IGL11ExtensionPack::Probe(gl);
    }
}


} // namespace Opengl
} // namespace Droid
} // namespace Elastos

