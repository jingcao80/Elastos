
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSRAWDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSRAWDATA_H__

#include "_Elastos_Droid_Internal_Telephony_CSmsRawData.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsRawData)
    , public Object
    , public ISmsRawData
    , public IParcelable
{
public:
    CSmsRawData();

    virtual ~CSmsRawData();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<ArrayOf<Byte> > mData;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSMSRAWDATA_H__
