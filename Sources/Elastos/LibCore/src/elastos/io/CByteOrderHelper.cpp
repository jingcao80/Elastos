
#include "CByteOrderHelper.h"

namespace Elastos {
namespace IO {

ByteOrder CByteOrderHelper::sNativeOrder;
ByteOrder CByteOrderHelper::sBigEndian;
ByteOrder CByteOrderHelper::sLittleEndian;
Boolean CByteOrderHelper::sIsLittleEndian = FALSE;
Boolean CByteOrderHelper::sIsInited = FALSE;

CAR_INTERFACE_IMPL(CByteOrderHelper, Singleton, IByteOrderHelper)

CAR_SINGLETON_IMPL(CByteOrderHelper)

Boolean CByteOrderHelper::IsLittleEndian()
{
    int i = 1;
    return *reinterpret_cast<Byte*>(&i) == 1;
}

void CByteOrderHelper::Init()
{
    if (sIsInited == FALSE) {
        sIsInited = TRUE;

        sIsLittleEndian = IsLittleEndian();
        sBigEndian      = ByteOrder_BIG_ENDIAN;
        sLittleEndian   = ByteOrder_LITTLE_ENDIAN;
        sNativeOrder    = sIsLittleEndian ? sLittleEndian : sBigEndian;
    }
}

ECode CByteOrderHelper::GetNativeOrder(
        /* [out] */ByteOrder* nativeOrder)
{
    VALIDATE_NOT_NULL(nativeOrder);

    *nativeOrder = _GetNativeOrder();

    return NOERROR;
}

ECode CByteOrderHelper::IsNeedsSwap(
    /* [in] */ ByteOrder order,
    /* [out] */ Boolean* isNeedsSwap)
{
    VALIDATE_NOT_NULL(isNeedsSwap)

    if (order == ByteOrder_BIG_ENDIAN || order == ByteOrder_LITTLE_ENDIAN) {
        *isNeedsSwap = _IsNeedsSwap(order);
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ByteOrder CByteOrderHelper::_GetNativeOrder()
{
    Init();
    return sNativeOrder;
}

Boolean CByteOrderHelper::_IsNeedsSwap(
    /* [in] */ ByteOrder order)
{
    Init();
    if (order == ByteOrder_BIG_ENDIAN) {
        return sIsLittleEndian;
    }
    else if (order == ByteOrder_LITTLE_ENDIAN) {
        return !sIsLittleEndian;
    }
    else {
        assert(0);
        return FALSE;
    }
}

} // namespace IO
} // namespace Elastos
