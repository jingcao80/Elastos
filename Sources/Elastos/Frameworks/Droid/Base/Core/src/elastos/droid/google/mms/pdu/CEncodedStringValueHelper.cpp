#include "elastos/droid/google/mms/pdu/CEncodedStringValueHelper.h"
#include "elastos/droid/google/mms/pdu/EncodedStringValue.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CAR_INTERFACE_IMPL(CEncodedStringValueHelper, Singleton, IEncodedStringValueHelper)
CAR_SINGLETON_IMPL(CEncodedStringValueHelper)


ECode CEncodedStringValueHelper::Extract(
    /* [in] */ const String& src,
    /* [out, callee] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<IEncodedStringValue*> > array = EncodedStringValue::Extract(src);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CEncodedStringValueHelper::Concat(
    /* [in] */ ArrayOf<IEncodedStringValue*>* addr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = EncodedStringValue::Concat(addr);
    return NOERROR;
}

ECode CEncodedStringValueHelper::Copy(
    /* [in] */ IEncodedStringValue* value,
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IEncodedStringValue> esv = EncodedStringValue::Copy(value);
    *result = esv;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CEncodedStringValueHelper::EncodeStrings(
    /* [in] */ ArrayOf<String>* array,
    /* [out, callee] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<IEncodedStringValue*> > res = EncodedStringValue::EncodeStrings(array);
    *result = res;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
