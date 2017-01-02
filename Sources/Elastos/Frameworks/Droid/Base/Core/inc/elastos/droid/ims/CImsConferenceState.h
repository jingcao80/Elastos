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

#ifndef __ELASTOS_DROID_IMS_CIMSCONFERENCESTATE_H__
#define __ELASTOS_DROID_IMS_CIMSCONFERENCESTATE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Ims_CImsConferenceState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsConferenceState)
    , public Object
    , public IImsConferenceState
    , public IParcelable
{
public:
    CImsConferenceState();

    virtual ~CImsConferenceState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    AutoPtr<IHashMap> mParticipants;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSCONFERENCESTATE_H__
