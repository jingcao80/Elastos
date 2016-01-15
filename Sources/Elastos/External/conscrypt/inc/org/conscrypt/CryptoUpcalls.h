
#ifndef __CRYPTOUPCALLS_H__
#define __CRYPTOUPCALLS_H__

#include "Org.Conscrypt.h"

using Elastos::Security::IPrivateKey;

namespace Org {
namespace Conscrypt {

class  CryptoUpcalls
{
public:
    static CARAPI RawSignDigestWithPrivateKey(
        /* [in] */ IPrivateKey* key,
        /* [in] */ ArrayOf<Byte>* message,
        /* [out, callee] */ ArrayOf<Byte>** result);

    static CARAPI RawCipherWithPrivateKey(
        /* [in] */ IPrivateKey* key,
        /* [in] */ Boolean encrypt,
        /* [in] */ ArrayOf<Byte>* input,
        /* [out, callee] */ ArrayOf<Byte>** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__CRYPTOUPCALLS_H__
