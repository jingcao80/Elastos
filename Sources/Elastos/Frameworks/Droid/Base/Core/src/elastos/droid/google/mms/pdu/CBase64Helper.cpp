#include "elastos/droid/google/mms/pdu/CBase64Helper.h"
#include "elastos/droid/google/mms/pdu/Base64.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CAR_INTERFACE_IMPL(CBase64Helper, Singleton, IBase64Helper)
CAR_SINGLETON_IMPL(CBase64Helper)

ECode CBase64Helper::DecodeBase64(
    /* [in] */ ArrayOf<Byte>* base64Data,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = Base64::DecodeBase64(base64Data);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBase64Helper::DiscardNonBase64(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = Base64::DiscardNonBase64(data);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
