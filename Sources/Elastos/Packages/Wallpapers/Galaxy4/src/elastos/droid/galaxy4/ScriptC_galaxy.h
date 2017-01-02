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

#ifndef __ELASTOS_DROID_GALAXY4_SCRIPTC_GALAXY_H__
#define __ELASTOS_DROID_GALAXY4_SCRIPTC_GALAXY_H__

#include "Elastos.Droid.Content.h"
#include "Mesh.h"
#include "ScriptC.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class ScriptC_galaxy : public ScriptC
{
public:
    ScriptC_galaxy() {}

    CARAPI constructor(
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* resources,
        /* [in] */ Int32 id);

    CARAPI Set_spaceCloudsMesh(
        /* [in] */ Mesh* v);

private:
    const static Int32 mExportVarIdx_spaceCloudsMesh = 4;
    AutoPtr<Mesh> mExportVar_spaceCloudsMesh;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_SCRIPTC_GALAXY_H__