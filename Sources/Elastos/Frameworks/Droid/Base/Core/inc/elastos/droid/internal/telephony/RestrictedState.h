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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_RESTRICTEDSTATE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_RESTRICTEDSTATE_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class RestrictedState
    : public Object
    , public IRestrictedState
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * @param csEmergencyRestricted the csEmergencyRestricted to set
     */
    CARAPI SetCsEmergencyRestricted(
        /* [in] */ Boolean csEmergencyRestricted);

    /**
     * @return the csEmergencyRestricted
     */
    CARAPI IsCsEmergencyRestricted(
        /* [out] */ Boolean* result);

    /**
     * @param csNormalRestricted the csNormalRestricted to set
     */
    CARAPI SetCsNormalRestricted(
        /* [in] */ Boolean csNormalRestricted);

    /**
     * @return the csNormalRestricted
     */
    CARAPI IsCsNormalRestricted(
        /* [out] */ Boolean* result);

    /**
     * @param psRestricted the psRestricted to set
     */
    CARAPI SetPsRestricted(
        /* [in] */ Boolean psRestricted);

    /**
     * @return the psRestricted
     */
    CARAPI IsPsRestricted(
        /* [out] */ Boolean* result);

    CARAPI IsCsRestricted(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * Set TRUE to block packet data access due to restriction
     */
    Boolean mPsRestricted;

    /**
     * Set TRUE to block all normal voice/SMS/USSD/SS/AV64 due to restriction
     */
    Boolean mCsNormalRestricted;

    /**
     * Set TRUE to block emergency call due to restriction
     */
    Boolean mCsEmergencyRestricted;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_RESTRICTEDSTATE_H__
