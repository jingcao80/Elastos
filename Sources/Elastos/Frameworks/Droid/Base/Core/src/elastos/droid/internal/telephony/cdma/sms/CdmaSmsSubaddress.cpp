#include "elastos/droid/internal/telephony/cdma/sms/CdmaSmsSubaddress.h"


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

//=====================================================================
//                            CdmaSmsSubaddress
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSmsSubaddress, Object, ICdmaSmsSubaddress);

ECode CdmaSmsSubaddress::SetType(
     /* [in] */ Int32 _type)
{
    type = _type;
    return NOERROR;
}

ECode CdmaSmsSubaddress::GetType(
     /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = type;
    return NOERROR;
}

ECode CdmaSmsSubaddress::SetOdd(
     /* [in] */ Byte _odd)
{
    odd = _odd;
    return NOERROR;
}

ECode CdmaSmsSubaddress::GetOdd(
     /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = odd;
    return NOERROR;
}

ECode CdmaSmsSubaddress::SetOrigBytes(
     /* [in] */ ArrayOf<Byte>* _origBytes)
{
    origBytes = _origBytes;
    return NOERROR;
}

ECode CdmaSmsSubaddress::GetOrigBytes(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = origBytes;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
