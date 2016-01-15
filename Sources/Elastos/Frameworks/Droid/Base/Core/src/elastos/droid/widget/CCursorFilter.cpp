
#include "elastos/droid/widget/CCursorFilter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

ECode CCursorFilter::SetDelayer(
    /* [in] */ IFilterDelayer * pDelayer)
{
    // TODO: Add your code here
    return CursorFilter::SetDelayer(pDelayer);
}

ECode CCursorFilter::DoFilter(
    /* [in] */ ICharSequence * pConstraint)
{
    // TODO: Add your code here
    return CursorFilter::DoFilter(pConstraint);
}

ECode CCursorFilter::DoFilter(
    /* [in] */ ICharSequence * pConstraint,
    /* [in] */ IFilterListener * pListener)
{
    // TODO: Add your code here
    return CursorFilter::DoFilter(pConstraint, pListener);
}

ECode CCursorFilter::ConvertResultToString(
    /* [in] */ IInterface * pResultValue,
    /* [out] */ ICharSequence ** ppCs)
{
    // TODO: Add your code here
    return CursorFilter::ConvertResultToString(pResultValue, ppCs);
}

ECode CCursorFilter::constructor(
    /* [in] */ ICursorFilterClient * pClient)
{
    // TODO: Add your code here
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
