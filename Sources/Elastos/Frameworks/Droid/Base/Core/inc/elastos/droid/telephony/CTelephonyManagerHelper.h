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

#ifndef __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGERHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGERHELPER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Telephony_CTelephonyManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CTelephonyManagerHelper)
    , public Singleton
    , public ITelephonyManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDefault(
        /* [out] */ ITelephonyManager** res);

    CARAPI From(
        /* [in] */ IContext* ctx,
        /* [out] */ ITelephonyManager** res);

    /**
     * This function returns the type of the phone, depending
     * on the network mode.
     *
     * @param networkMode
     * @return Phone Type
     *
     * @hide
     */
    CARAPI GetPhoneType(
        /* [in] */ Int32 networkMode,
        /* [out] */ Int32* val);

    CARAPI GetLteOnCdmaModeStatic(
        /* [out] */ Int32* val);

    CARAPI GetNetworkClass(
        /* [in] */ Int32 networkType,
        /* [out] */ Int32* val);

    CARAPI GetNetworkTypeName(
        /* [in] */ Int32 type,
        /* [out] */ String* res);

    /**
     * Sets the telephony property with the value specified.
     *
     * @hide
     */
    CARAPI SetTelephonyProperty(
        /* [in] */ const String& property,
        /* [in] */ Int64 subId,
        /* [in] */ const String& value);

    /**
     * Convenience function for retrieving a value from the secure settings
     * value list as an integer.  Note that internally setting values are
     * always stored as strings; this function converts the string to an
     * integer for you.
     * <p>
     * This version does not take a default value.  If the setting has not
     * been set, or the string value is not a number,
     * it throws {@link SettingNotFoundException}.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to retrieve.
     * @param index The index of the list
     *
     * @throws SettingNotFoundException Thrown if a setting by the given
     * name can't be found or the setting value is not an integer.
     *
     * @return The value at the given index of settings.
     * @hide
     */
    CARAPI GetInt32AtIndex(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ Int32 index,
        /* [out] */ Int32* val);

    /**
     * Convenience function for updating settings value as coma separated
     * integer values. This will either create a new entry in the table if the
     * given name does not exist, or modify the value of the existing row
     * with that name.  Note that internally setting values are always
     * stored as strings, so this function converts the given value to a
     * string before storing it.
     *
     * @param cr The ContentResolver to access.
     * @param name The name of the setting to modify.
     * @param index The index of the list
     * @param value The new value for the setting to be added to the list.
     * @return true if the value was set, false on database errors
     * @hide
     */
    CARAPI PutInt32AtIndex(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& name,
        /* [in] */ Int32 index,
        /* [in] */ Int32 val,
        /* [out] */ Boolean* result);

    /**
     * Gets the telephony property.
     *
     * @hide
     */
    CARAPI GetTelephonyProperty(
        /* [in] */ const String& property,
        /* [in] */ Int64 subId,
        /* [in] */ const String& defaultVal,
        /* [out] */ String* result);
};

}
}
}

#endif // __ELASTOS_DROID_TELEPHONY_CTELEPHONYMANAGERHELPER_H__
