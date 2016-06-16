
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_COPERATORINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_COPERATORINFO_H__

#include "_Elastos_Droid_Internal_Telephony_COperatorInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
CarClass(COperatorInfo)
    , public Object
    , public IOperatorInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    COperatorInfo();

    CARAPI constructor();

    CARAPI GetOperatorAlphaLong(
        /* [out] */ String* result);

    CARAPI GetOperatorAlphaShort(
        /* [out] */ String* result);

    CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    CARAPI GetState(
        /* [out] */ IOperatorInfoState* result);

    CARAPI GetRadioTech(
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ const String& operatorAlphaLong,
        /* [in] */ const String& operatorAlphaShort,
        /* [in] */ const String& operatorNumeric,
        /* [in] */ IOperatorInfoState state);

    CARAPI constructor(
        /* [in] */ const String& operatorAlphaLong,
        /* [in] */ const String& operatorAlphaShort,
        /* [in] */ const String& operatorNumeric,
        /* [in] */ const String& stateString);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Implement the Parcelable interface.
     * Method to serialize a OperatorInfo object.
     */
    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    /**
     * See state strings defined in ril.h RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
     */
    static CARAPI RilStateToState(
        /* [in] */ const String& s,
        /* [out] */ IOperatorInfoState* result);

private:
    String mOperatorAlphaLong;
    String mOperatorAlphaShort;
    String mOperatorNumeric;
    IOperatorInfoState mState;
    String mRadioTech;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_COPERATORINFO_H__
