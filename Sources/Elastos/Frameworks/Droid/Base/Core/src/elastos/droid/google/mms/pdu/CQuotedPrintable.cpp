#include "elastos/droid/google/mms/pdu/CQuotedPrintable.h"
#include "elastos/droid/google/mms/pdu/QuotedPrintable.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CAR_INTERFACE_IMPL(CQuotedPrintable, Singleton, IQuotedPrintable)
CAR_SINGLETON_IMPL(CQuotedPrintable)

ECode CQuotedPrintable::DecodeQuotedPrintable(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = QuotedPrintable::DecodeQuotedPrintable(bytes);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
