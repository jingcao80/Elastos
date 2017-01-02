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

#ifndef __ELASTOS_DROID_GALAXY4_GALAXYRS_H__
#define __ELASTOS_DROID_GALAXY4_GALAXYRS_H__

#include "Elastos.Droid.Content.h"
#include "Mesh.h"
#include "RenderScript.h"
#include "ScriptC_galaxy.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class GalaxyRS
    : public Object
{
public:
    CARAPI Init(
        /* [in] */ Int32 dpi,
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* res,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI CreateProgramVertex();

    CARAPI Start();

    CARAPI Stop();

    CARAPI Resize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    AutoPtr<IResources> mRes;

    AutoPtr<RenderScript> mRS;
    AutoPtr<ScriptC_galaxy> mScript;

    AutoPtr<Mesh> mSpaceCloudsMesh;

    Int32 mHeight;
    Int32 mWidth;
    Boolean mInited;
    Int32 mDensityDPI;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_GALAXYRS_H__
