
#ifndef __ELASTOS_SECURITY_SPEC_ENCODEDKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_ENCODEDKEYSPEC_H__

#include "_Elastos_Security_Spec_CX509EncodedKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

class EncodedKeySpec
    : public Object
    , public IEncodedKeySpec
    , public IKeySpec
{
public:
    CAR_INTERFACE_DECL();

    EncodedKeySpec();

    EncodedKeySpec(
        /* [in] */ ArrayOf<Byte> *encodedKey);

    /**
     * Returns the encoded key bytes.
     *
     * @return the encoded key bytes.
     */
    virtual CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> **encodedKey);

    virtual CARAPI GetFormat(
        /* [out] */ String *format) = 0;

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> *encodedKey);
private:
    // Encoded key
    const AutoPtr<ArrayOf<Byte> > mEncodedKey;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_ENCODEDKEYSPEC_H__
