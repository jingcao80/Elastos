
#ifndef __ELASTOS_DROID_NET_WIFI_CSCANRESULTINFORMATIONELEMENT_H__
#define __ELASTOS_DROID_NET_WIFI_CSCANRESULTINFORMATIONELEMENT_H__

#include "_Elastos_Droid_Wifi_CScanResultInformationElement.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CScanResultInformationElement)
    , public Object
    , public IScanResultInformationElement
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IScanResultInformationElement* rhs);

    CARAPI GetId(
        /* [out] */ Int32* result);

    CARAPI SetId(
        /* [in] */ Int32 id);

    CARAPI GetBytes(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI SetBytes(
        /* [in] */ ArrayOf<Byte>* bytes);

private:
    Int32 mId;
    AutoPtr< ArrayOf<Byte> > mBytes;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CSCANRESULTINFORMATIONELEMENT_H__
