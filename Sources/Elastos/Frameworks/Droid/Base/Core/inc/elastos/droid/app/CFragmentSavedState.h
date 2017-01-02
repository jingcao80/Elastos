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

#ifndef __ELASTOS_DROID_APP_CFRAGMENTSAVEDSTATE_H__
#define __ELASTOS_DROID_APP_CFRAGMENTSAVEDSTATE_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CFragmentSavedState.h"
#include <elastos/core/Object.h>

using Elastos::Core::IClassLoader;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CFragmentSavedState)
    , public Object
    , public IFragmentSavedState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBundle* state);

    CARAPI constructor(
        /* [in] */ IParcel* in,
        /* [in] */ IClassLoader* loader);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetState(
        /* [out] */ IBundle** state);

    CARAPI SetState(
        /* [in] */ IBundle* state);

public:
    AutoPtr<IBundle> mState;
    AutoPtr<IClassLoader> mLoader;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CFRAGMENTSAVEDSTATE_H__
