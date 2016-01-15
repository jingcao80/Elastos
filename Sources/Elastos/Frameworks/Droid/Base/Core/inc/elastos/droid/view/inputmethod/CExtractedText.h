
#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CExtractedText_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CExtractedText_H__

#include "_Elastos_Droid_View_InputMethod_CExtractedText.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CExtractedText)
    , public Object
    , public IExtractedText
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CExtractedText();

    CARAPI constructor();

    CARAPI GetStartOffset(
        /* [out] */ Int32* startOffset);

    CARAPI SetStartOffset(
        /* [in] */ Int32 startOffset);

    CARAPI SetText(
        /* [in] */ ICharSequence* text);

    CARAPI GetText(
        /* [out] */ ICharSequence** text);

    CARAPI SetPartialStartOffset(
        /* [in] */ Int32 offset);

    CARAPI GetPartialStartOffset(
        /* [out] */ Int32* offset);

    CARAPI SetPartialEndOffset(
        /* [in] */ Int32 offset);

    CARAPI GetPartialEndOffset(
        /* [out] */ Int32* offset);

    CARAPI SetSelectionStart(
        /* [in] */ Int32 value);

    CARAPI GetSelectionStart(
        /* [out] */ Int32* value);

    CARAPI SetSelectionEnd(
        /* [in] */ Int32 value);

    CARAPI GetSelectionEnd(
        /* [out] */ Int32* value);

    CARAPI SetFlags(
        /* [in] */ Int32 value);

    CARAPI GetFlags(
        /* [out] */ Int32* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

public:
    /**
     * The text that has been extracted.
     */
    AutoPtr<ICharSequence> mText;

    /**
     * The offset in the overall text at which the extracted text starts.
     */
    Int32 mStartOffset;

    /**
     * If the content is a report of a partial text change, this is the
     * offset where the change starts and it runs until
     * {@link #partialEndOffset}.  If the content is the full text, this
     * field is -1.
     */
    Int32 mPartialStartOffset;

    /**
     * If the content is a report of a partial text change, this is the offset
     * where the change ends.  Note that the actual text may be larger or
     * smaller than the difference between this and {@link #partialEndOffset},
     * meaning a reduction or increase, respectively, in the total text.
     */
    Int32 mPartialEndOffset;

    /**
     * The offset where the selection currently starts within the extracted
     * text.  The real selection start position is at
     * <var>startOffset</var>+<var>selectionStart</var>.
     */
    Int32 mSelectionStart;

    /**
     * The offset where the selection currently ends within the extracted
     * text.  The real selection end position is at
     * <var>startOffset</var>+<var>selectionEnd</var>.
     */
    Int32 mSelectionEnd;

    /**
     * Bit for {@link #flags}: set if the text being edited can only be on
     * a single line.
     */
    static const Int32 FLAG_SINGLE_LINE;

    /**
     * Bit for {@link #flags}: set if the editor is currently in selection mode.
     */
    static const Int32 FLAG_SELECTING;

    /**
     * Additional bit flags of information about the edited text.
     */
    Int32 mFlags;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTMETHOD_CExtractedText_H__
