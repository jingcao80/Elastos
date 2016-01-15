
#include "CCellLocationHelper.h"
#include "CellLocation.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {


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