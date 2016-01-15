#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CAlteredCharSequence.h"
#include "elastos/droid/text/CAnnotation.h"
#include "elastos/droid/text/CBoringLayout.h"
#include "elastos/droid/text/CDynamicLayout.h"
#include "elastos/droid/text/CStaticLayout.h"
#include "elastos/droid/text/CNoCopySpanConcrete.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/CTextPaint.h"

namespace Elastos {
namespace Droid {
namespace Text {

ECode CNoCopySpanConcrete::constructor()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNoCopySpanConcrete, Object, INoCopySpan)
CAR_OBJECT_IMPL(CNoCopySpanConcrete)

CAR_OBJECT_IMPL(CAlteredCharSequence)
CAR_OBJECT_IMPL(CAnnotation)
CAR_OBJECT_IMPL(CBoringLayout)
CAR_OBJECT_IMPL(CDynamicLayout)
CAR_OBJECT_IMPL(CStaticLayout)
CAR_OBJECT_IMPL(CSpannableStringBuilder)
CAR_OBJECT_IMPL(CTextPaint)


} // namespace Text
} // namepsace Droid
} // namespace Elastos