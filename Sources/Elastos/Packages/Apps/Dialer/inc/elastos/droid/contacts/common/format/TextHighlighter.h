
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_FORMAT_TEXTHIGHLIGHTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_FORMAT_TEXTHIGHLIGHTER_H__

#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::Style::ICharacterStyle;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::Object;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Format {

/**
 * Highlights the text in a text field.
 */
class TextHighlighter : public Object
{
public:
    TextHighlighter(
        /* [in] */ Int32 textStyle);

    /**
     * Sets the text on the given text view, highlighting the word that matches the given prefix.
     *
     * @param view the view on which to set the text
     * @param text the string to use as the text
     * @param prefix the prefix to look for
     */
    CARAPI_(void) SetPrefixText(
        /* [in] */ ITextView* view,
        /* [in] */ const String& text,
        /* [in] */ const String& prefix);

    /**
     * Applies highlight span to the text.
     * @param text Text sequence to be highlighted.
     * @param start Start position of the highlight sequence.
     * @param end End position of the highlight sequence.
     */
    CARAPI_(void) ApplyMaskingHighlight(
        /* [in] */ ISpannableString* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Returns a CharSequence which highlights the given prefix if found in the given text.
     *
     * @param text the text to which to apply the highlight
     * @param prefix the prefix to look for
     */
    CARAPI_(AutoPtr<ICharSequence>) ApplyPrefixHighlight(
        /* [in] */ ICharSequence* text,
        /* [in] */ const String& prefix);

private:
    CARAPI_(AutoPtr<ICharacterStyle>) GetStyleSpan();

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    Int32 mTextStyle;

    AutoPtr<ICharacterStyle> mTextStyleSpan;
};

} // Format
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_FORMAT_TEXTHIGHLIGHTER_H__
