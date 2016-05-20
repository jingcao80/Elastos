
#ifndef __ELASTOS_SECURITY_SPEC_CX509ENCODEDKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CX509ENCODEDKEYSPEC_H__

#include "_Elastos_Security_Spec_CX509EncodedKeySpec.h"
#include "EncodedKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CX509EncodedKeySpec)
    , public EncodedKeySpec
    , public IX509EncodedKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> **encoded);

    CARAPI GetFormat(
        /* [out] */ String *format);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> *encodedKey);

private:
    // TODO: Add your private member variables here.
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CX509ENCODEDKEYSPEC_H__
