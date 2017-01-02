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

#ifndef __ELASTOS_DROID_GALAXY4_BASEOBJ_H__
#define __ELASTOS_DROID_GALAXY4_BASEOBJ_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class RenderScript;

class BaseObj : public Object
{
public:
    CARAPI constructor(
        /* [in] */ Int64 id,
        /* [in] */ RenderScript* rs);

    CARAPI SetID(
        /* [in] */ Int64 id);

    CARAPI GetID(
        /* [in] */ RenderScript* rs,
        /* [out] */ Int64* id);

public:
    RenderScript* mRS;

private:
    Int64 mID;
    Boolean mDestroyed;
    String mName;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_BASEOBJ_H__
