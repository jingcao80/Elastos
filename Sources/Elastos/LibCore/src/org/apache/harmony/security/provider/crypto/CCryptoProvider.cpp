
#include "CCryptoProvider.h"
#include "CoreUtils.h"

using Elastos::Core::CoreUtils;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CCryptoProvider)
ECode CCryptoProvider::constructor()
{
    Provider::constructor(String("Crypto"), 1.0,
        String("HARMONY (SHA1 digest; SecureRandom; SHA1withDSA signature)"));

    Put(CoreUtils::Convert(String("SecureRandom.SHA1PRNG")),
            CoreUtils::Convert(String("Org.Apache.Harmony.Security.Provider.Crypto.CSHA1PRNG_SecureRandomImpl")));
    Put(CoreUtils::Convert(String("SecureRandom.SHA1PRNG ImplementedIn")), CoreUtils::Convert(String("Software")));
    return NOERROR;
}

}
}
}
}
}
}
