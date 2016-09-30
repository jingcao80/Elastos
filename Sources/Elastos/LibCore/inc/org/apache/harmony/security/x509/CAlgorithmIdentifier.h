
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CALGORITHMIDENTIFIER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CALGORITHMIDENTIFIER_H__

#include "_Org_Apache_Harmony_Security_X509_CAlgorithmIdentifier.h"
#include "org/apache/harmony/security/x509/AlgorithmIdentifier.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CAlgorithmIdentifier)
    , public AlgorithmIdentifier
{
public:
    CAR_OBJECT_DECL()
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CALGORITHMIDENTIFIER_H__
