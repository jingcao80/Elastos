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

#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALNAMEMATCHERHELPER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALNAMEMATCHERHELPER_H__

#include "_Elastos_Droid_Dialer_Dialpad_CSmartDialNameMatcherHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CSmartDialNameMatcherHelper)
    , public Singleton
    , public ISmartDialNameMatcherHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Strips a phone number of unnecessary characters (spaces, dashes, etc.)
     *
     * @param number Phone number we want to normalize
     * @return Phone number consisting of digits from 0-9
     */
    CARAPI NormalizeNumber(
        /* [in] */ const String& number,
        /* [in] */ ISmartDialMap* map,
        /* [out] */ String* result);

    /**
     * Strips a phone number of unnecessary characters (spaces, dashes, etc.)
     *
     * @param number Phone number we want to normalize
     * @param offset Offset to start from
     * @return Phone number consisting of digits from 0-9
     */
    CARAPI NormalizeNumber(
        /* [in] */ String number,
        /* [in] */ Int32 offset,
        /* [in] */ ISmartDialMap* map,
        /* [out] */ String* result);

    CARAPI GetLATIN_SMART_DIAL_MAP(
        /* [out] */ ISmartDialMap** map);
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALNAMEMATCHERHELPER_H__
