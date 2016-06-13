
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/UsimServiceTable.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           UsimServiceTable
//=====================================================================
UsimServiceTable::UsimServiceTable()
{
}

ECode UsimServiceTable::constructor(
    /* [in] */ ArrayOf<Byte>* table)
{
    // ==================before translated======================
    // super(table);
    return NOERROR;
}

ECode UsimServiceTable::IsAvailable(
    /* [in] */ UsimService service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return super.isAvailable(service.ordinal());
    assert(0);
    return NOERROR;
}

ECode UsimServiceTable::GetTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return "UsimServiceTable";
    assert(0);
    return NOERROR;
}

ECode UsimServiceTable::GetValues(
    /* [out] */ ArrayOf<IInterface*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return UsimService.values();
    assert(0);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
