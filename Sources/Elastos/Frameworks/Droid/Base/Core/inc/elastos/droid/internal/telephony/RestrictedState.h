
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
