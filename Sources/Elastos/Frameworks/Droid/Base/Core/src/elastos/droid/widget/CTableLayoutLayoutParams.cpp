#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTableLayoutLayoutParams.h"

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEWGROUPLP_METHODS_IMPL(CTableLayoutLayoutParams, TableLayoutLayoutParams)
IVIEWGROUPMARGINLP_METHODS_IMPL(CTableLayoutLayoutParams, TableLayoutLayoutParams)
ILINEARLAYOUTLP_METHODS_IMPL(CTableLayoutLayoutParams, TableLayoutLayoutParams)

ECode CTableLayoutLayoutParams::constructor()
{
    return TableLayoutLayoutParams::Init(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT);
}

ECode CTableLayoutLayoutParams::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TableLayoutLayoutParams::Init(context, attrs);
}

ECode CTableLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return TableLayoutLayoutParams::Init(width, height);
}

ECode CTableLayoutLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Float weight)
{
    return TableLayoutLayoutParams::Init(width, height, weight);
}

ECode CTableLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return TableLayoutLayoutParams::Init(source);
}

ECode CTableLayoutLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return TableLayoutLayoutParams::Init(source);
}


} // namespace Widget
} // namespace Droid
} // namespace Elastos
