
#ifndef __ELASTOS_DROID_TEXT_CHTML_H__
#define __ELASTOS_DROID_TEXT_CHTML_H__

#include "_Elastos_Droid_Text_CHtml.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Text::IHtmlImageGetter;
using Elastos::Droid::Text::IHtmlTagHandler;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CHtml)
    , public Singleton
    , public IHtml
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FromHtml(
        /* [in] */ const String& source,
        /* [out] */ ISpanned** ret);

    CARAPI FromHtml(
        /* [in] */ const String& source,
        /* [in] */ IHtmlImageGetter* imageGetter,
        /* [in] */ IHtmlTagHandler* tagHandler,
        /* [out] */ ISpanned** ret);

    CARAPI ToHtml(
        /* [in] */ ISpanned* text,
        /* [out] */ String* ret);

    CARAPI EscapeHtml(
        /* [in] */ ICharSequence* text,
        /* [out] */ String* ret);
};

} // namespace Text
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_CHTML_H__
