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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_CGEOUTIL_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_CGEOUTIL_H__

#include "_Elastos_Apps_Contacts_Common_CGeoUtil.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {

/**
 * Static methods related to Geo.
 */
CarClass(CGeoUtil)
    , public singleton
    , public IGeoUtil
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the country code of the country the user is currently in. Before calling this
     * method, make sure that {@link CountryDetector#initialize(Context)} has already been called
     * in {@link Application#onCreate()}.
     * @return The ISO 3166-1 two letters country code of the country the user
     *         is in.
     */
    CARAPI GetCurrentCountryIso(
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    CARAPI GetGeocodedLocationFor(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber,
        /* [out] */ String* result);
};

} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_CGEOUTIL_H__
