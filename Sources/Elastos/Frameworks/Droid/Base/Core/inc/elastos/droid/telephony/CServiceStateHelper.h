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

#ifndef __ELASTOS_DROID_TELEPHONY_CSERVICESTATEHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CSERVICESTATEHELPER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_CServiceStateHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CServiceStateHelper)
    , public Singleton
    , public IServiceStateHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewFromBundle(
        /* [in] */ IBundle* m,
        /* [out] */ IServiceState** res);

    CARAPI RilRadioTechnologyToString(
        /* [in] */ Int32 rt,
        /* [out] */ String* res);

    CARAPI IsGsm(
        /* [in] */ Int32 radioTechnology,
        /* [out] */ Boolean* res);

    CARAPI IsCdma(
        /* [in] */ Int32 radioTechnology,
        /* [out] */ Boolean* res);
};

} //namespace Elastos
} //namespace Droid
} //namespace Telephony

#endif // __ELASTOS_DROID_TELEPHONY_CSERVICESTATEHELPER_H__
