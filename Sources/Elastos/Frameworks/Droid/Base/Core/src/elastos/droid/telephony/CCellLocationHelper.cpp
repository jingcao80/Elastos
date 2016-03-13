
#include "elastos/droid/telephony/CCellLocationHelper.h"
#include "elastos/droid/telephony/CellLocation.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CCellLocationHelper, Singleton, ICellLocationHelper)

CAR_SINGLETON_IMPL(CCellLocationHelper)

ECode CCellLocationHelper::RequestLocationUpdate()
{
    CellLocation::RequestLocationUpdate();
    return NOERROR;
}

ECode CCellLocationHelper::NewFromBundle(
    /* [in] */ IBundle* bundle,
    /* [out] */ ICellLocation** cl)
{
    VALIDATE_NOT_NULL(cl);
    CellLocation::NewFromBundle(bundle, cl);
    return NOERROR;
}

ECode CCellLocationHelper::GetEmpty(
    /* [out] */ ICellLocation** cl)
{
    VALIDATE_NOT_NULL(cl);
    CellLocation::GetEmpty(cl);
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Telephony