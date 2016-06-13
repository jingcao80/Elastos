
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccServiceTable.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           IccServiceTable
//=====================================================================
CAR_INTERFACE_IMPL(IccServiceTable, Object, IIccServiceTable);

IccServiceTable::IccServiceTable()
{
}

ECode IccServiceTable::constructor(
    /* [in] */ ArrayOf<Byte>* table)
{
    // ==================before translated======================
    // mServiceTable = table;
    return NOERROR;
}

ECode IccServiceTable::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Object[] values = getValues();
    // int numBytes = mServiceTable.length;
    // StringBuilder builder = new StringBuilder(getTag()).append('[')
    //         .append(numBytes * 8).append("]={ ");
    //
    // boolean addComma = false;
    // for (int i = 0; i < numBytes; i++) {
    //     byte currentByte = mServiceTable[i];
    //     for (int bit = 0; bit < 8; bit++) {
    //         if ((currentByte & (1 << bit)) != 0) {
    //             if (addComma) {
    //                 builder.append(", ");
    //             } else {
    //                 addComma = true;
    //             }
    //             int ordinal = (i * 8) + bit;
    //             if (ordinal < values.length) {
    //                 builder.append(values[ordinal]);
    //             } else {
    //                 builder.append('#').append(ordinal + 1);    // service number (one-based)
    //             }
    //         }
    //     }
    // }
    // return builder.append(" }").toString();
    assert(0);
    return NOERROR;
}

Boolean IccServiceTable::IsAvailable(
    /* [in] */ Int32 service)
{
    // ==================before translated======================
    // int offset = service / 8;
    // if (offset >= mServiceTable.length) {
    //     // Note: Enums are zero-based, but the TS service numbering is one-based
    //     Rlog.e(getTag(), "isAvailable for service " + (service + 1) + " fails, max service is " +
    //             (mServiceTable.length * 8));
    //     return false;
    // }
    // int bit = service % 8;
    // return (mServiceTable[offset] & (1 << bit)) != 0;
    assert(0);
    return FALSE;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
