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

#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_UPDATECOUNTTYSERVICE_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LOCATION_UPDATECOUNTTYSERVICE_H__

#include "elastos/droid/app/IntentService.h"

using Elastos::Droid::App::IntentService;

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Location {

/**
 * Service used to perform asynchronous geocoding from within a broadcast receiver. Given a
 * {@link Location}, convert it into a country code, and save it in shared preferences.
 */
class UpdateCountryService
    : public IntentService
    , public IUpdateCountryService
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    static CARAPI_(void) UpdateCountry(
        /* [in] */ IContext* context,
        /* [in] */ ILocation* location);

protected:
    // @Override
    CARAPI OnHandleIntent(
        /* [in] */ IIntent* intent);

private:
    /**
     * Given a {@link Location}, return a country code.
     *
     * @return the ISO 3166-1 two letter country code
     */
    CARAPI_(String) GetCountryFromLocation(
        /* [in] */ IContext* context,
        /* [in] */ ILocation* location);

private:
    static const String TAG; // = UpdateCountryService.class.getSimpleName();

    static const String ACTION_UPDATE_COUNTRY; // = "saveCountry";

    static const String KEY_INTENT_LOCATION; // = "location";
};

} // Location
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LOCATION_UPDATECOUNTTYSERVICE_H__
