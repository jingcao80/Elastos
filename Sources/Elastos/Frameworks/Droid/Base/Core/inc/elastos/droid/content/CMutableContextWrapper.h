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

#ifndef __ELASTOS_DROID_CONTENT_CMUTABLECONTEXTWRAPPER_H__
#define __ELASTOS_DROID_CONTENT_CMUTABLECONTEXTWRAPPER_H__

#include "_Elastos_Droid_Content_CMutableContextWrapper.h"
#include "ContextWrapper.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CMutableContextWrapper)
    , public ContextWrapper
    , public IMutableContextWrapper
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetBaseContext(
        /* [out] */ IContext** context);

    /**
     * Change the base context for this ContextWrapper. All calls will then be
     * delegated to the base context.  Unlike ContextWrapper, the base context
     * can be changed even after one is already set.
     *
     * @param base The new base context for this wrapper.
     */
    CARAPI SetBaseContext(
        /* [in] */ IContext* base);

    CARAPI constructor(
        /* [in] */ IContext* base);

private:
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CMUTABLECONTEXTWRAPPER_H__
