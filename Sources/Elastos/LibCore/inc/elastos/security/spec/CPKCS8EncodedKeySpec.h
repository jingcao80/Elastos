
#ifndef __ELASTOS_SECURITY_SPEC_CPKCS8ENCODEDKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CPKCS8ENCODEDKEYSPEC_H__

#include "_Elastos_Security_Spec_CPKCS8EncodedKeySpec.h"
#include "EncodedKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CPKCS8EncodedKeySpec)
    , public EncodedKeySpec
    , public IPKCS8EncodedKeySpec
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI GetFormat(
        /* [out] */ String *format);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> *encodedKey);
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CPKCS8ENCODEDKEYSPEC_H__
