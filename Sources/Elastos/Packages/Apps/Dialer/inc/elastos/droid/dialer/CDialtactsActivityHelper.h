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

#ifndef __ELASTOS_DROID_DIALER_CDIALTACTSACTIVITYHELPER_H__
#define __ELASTOS_DROID_DIALER_CDIALTACTSACTIVITYHELPER_H__

#include "_Elastos_Droid_Dialer_CDialtactsActivityHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Core.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence

namespace Elastos {
namespace Droid {
namespace Dialer {

CarClass(CDialtactsActivityHelper)
    , public Singleton
    , public IDialtactsActivityHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /** Returns an Intent to launch Call Settings screen */
    CARAPI GetCallSettingsIntent(
        /* [out] */ IIntent** intent);

    CARAPI GetAddNumberToContactIntent(
        /* [in] */ICharSequence* text,
        /* [out] */ IIntent** intent);
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CDIALTACTSACTIVITYHELPER_H__
