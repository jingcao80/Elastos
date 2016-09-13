
#include "security/cert/CRL.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(CRL, Object, ICRL)
CRL::CRL(
    /* [in] */ const String& type)
: mType(type)
{}

ECode CRL::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

} // end Cert
} // end Security
} // end Elastos