
#ifndef __ELASTOS_DROID_TEXT_UTILITY_CLINKIFY_H__
#define __ELASTOS_DROID_TEXT_UTILITY_CLINKIFY_H__

#include "_Elastos_Droid_Text_Utility_CLinkify.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IMatcher;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Utility {

CarClass(CLinkify)
    , public Singleton
    , public ILinkify
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetStaticUrlMatchFilter(
        /* [out] */ ILinkifyMatchFilter** ret);

    CARAPI GetStaticPhoneNumberMatchFilter(
        /* [out] */ ILinkifyMatchFilter** ret);

    CARAPI GetStaticPhoneNumberTransformFilter(
        /* [out] */ ILinkifyTransformFilter** ret);

    CARAPI AddLinks(
        /* [in] */ ISpannable* text,
        /* [in] */ Int32 mask,
        /* [out] */ Boolean* ret);

    CARAPI AddLinks(
        /* [in] */ ITextView* text,
        /* [in] */ Int32 mask,
        /* [out] */ Boolean* ret);

    CARAPI AddLinks(
        /* [in] */ ITextView* text,
        /* [in] */ IPattern* pattern,
        /* [in] */ const String& scheme);

    CARAPI AddLinks(
        /* [in] */ ITextView* text,
        /* [in] */ IPattern* p,
        /* [in] */ const String& scheme,
        /* [in] */ ILinkifyMatchFilter* matchFilter,
        /* [in] */ ILinkifyTransformFilter* transformFilter);

    CARAPI AddLinks(
        /* [in] */ ISpannable* text,
        /* [in] */ IPattern* pattern,
        /* [in] */ const String& scheme,
        /* [out] */ Boolean* ret);

    CARAPI AddLinks(
        /* [in] */ ISpannable* s,
        /* [in] */ IPattern* p,
        /* [in] */ const String& scheme,
        /* [in] */ ILinkifyMatchFilter* matchFilter,
        /* [in] */ ILinkifyTransformFilter* transformFilter,
        /* [out] */ Boolean* ret);

};

} // namespace Utility
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_UTILITY_CLINKIFY_H__
