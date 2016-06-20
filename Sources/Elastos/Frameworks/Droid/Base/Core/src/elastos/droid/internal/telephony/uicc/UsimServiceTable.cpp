
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
CAR_INTERFACE_IMPL(UsimServiceTable, IccServiceTable, IUsimServiceTable);

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

String UsimServiceTable::GetTag()
{
    return String("UsimServiceTable");
}

AutoPtr<ArrayOf<IInterface*> > UsimServiceTable::GetValues()
{
    // ==================before translated======================
    // return UsimService.values();
    assert(0);
    return NULL;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
