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

#ifndef __ELASTOS_DROID_APP_CFRAGMENTMANAGERSTATE_H__
#define __ELASTOS_DROID_APP_CFRAGMENTMANAGERSTATE_H__

#include "_Elastos_Droid_App_CFragmentManagerState.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CFragmentManagerState)
    , public Object
    , public IFragmentManagerState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFragmentManagerState();

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    AutoPtr< ArrayOf<IFragmentState*> > mActive;
    AutoPtr< ArrayOf<Int32> > mAdded;
    AutoPtr< ArrayOf<IBackStackState*> > mBackStack;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CFRAGMENTMANAGERSTATE_H__
