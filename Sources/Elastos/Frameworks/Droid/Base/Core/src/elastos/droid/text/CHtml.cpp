
#include "elastos/droid/text/CHtml.h"
#include "elastos/droid/text/Html.h"

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(CHtml, Singleton, IHtml)

CAR_SINGLETON_IMPL(CHtml)

ECode CHtml::FromHtml(
    /* [in] */ const String& source,
    /* [out] */ ISpanned** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ISpanned> spanned = Html::FromHtml(source);
    *ret = spanned;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CHtml::FromHtml(
    /* [in] */ const String& source,
    /* [in] */ IHtmlImageGetter* imageGetter,
    /* [in] */ IHtmlTagHandler* tagHandler,
    /* [out] */ ISpanned** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ISpanned> spanned = Html::FromHtml(source, imageGetter, tagHandler);
    *ret = spanned;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CHtml::ToHtml(
    /* [in] */ ISpanned* text,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Html::ToHtml(text);
    return NOERROR;
}

ECode CHtml::EscapeHtml(
    /* [in] */ ICharSequence* text,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = Html::EscapeHtml(text);
    return NOERROR;
}

} // namespace Text
} // namespace Droid
} // namespace Elastos

