
#include "security/cert/PKIXCertPathChecker.h"

using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL_2(PKIXCertPathChecker, Object, IPKIXCertPathChecker, ICloneable)

PKIXCertPathChecker::PKIXCertPathChecker()
{}

}
}
}
