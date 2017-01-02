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

#ifndef __ELASTOS_DROID_GALAXY4_SCRIPTC_H__
#define __ELASTOS_DROID_GALAXY4_SCRIPTC_H__

#include "Elastos.Droid.Content.h"
#include "Script.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class ScriptC : public Script
{
protected:
    CARAPI constructor(
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* resources,
        /* [in] */ Int32 id);

private:
    static CARAPI InternalCreate(
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* resources,
        /* [in] */ Int32 resourceID,
        /* [out] */ Int64* id);

private:
    static const String CACHE_PATH;
    static String sCachePath;
    static Object sLock;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_SCRIPTC_H__
