#include "elastos/droid/text/style/TabStopSpan.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {


//=====================================================================
// TabStopSpan
//=====================================================================

CAR_INTERFACE_IMPL_2(TabStopSpan, Object, ITabStopSpan, IParagraphStyle)

TabStopSpan::TabStopSpan()
{}

TabStopSpan::~TabStopSpan()
{}

//=====================================================================
// TabStopSpanStandard
//=====================================================================
CAR_INTERFACE_IMPL(TabStopSpanStandard, TabStopSpan, ITabStopSpanStandard)

TabStopSpanStandard::TabStopSpanStandard()
    : mTab(0)
{}

TabStopSpanStandard::~TabStopSpanStandard()
{}

ECode TabStopSpanStandard::constructor(
    /* [in] */ Int32 where)
{
    mTab = where;
    return NOERROR;
}

ECode TabStopSpanStandard::GetTabStop(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = mTab;
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos