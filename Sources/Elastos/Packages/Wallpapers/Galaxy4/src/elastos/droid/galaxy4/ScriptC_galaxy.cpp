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

#include "ScriptC_galaxy.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode ScriptC_galaxy::constructor(
    /* [in] */ RenderScript* rs,
    /* [in] */ IResources* resources,
    /* [in] */ Int32 id)
{
    return ScriptC::constructor(rs, resources, id);
}

ECode ScriptC_galaxy::Set_spaceCloudsMesh(
    /* [in] */ Mesh* v)
{
    FAIL_RETURN(SetVar(mExportVarIdx_spaceCloudsMesh, v));
    mExportVar_spaceCloudsMesh = v;
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
