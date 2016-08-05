#ifndef __ELASTOS_DROID_TEXT_METHOD_REPLACEMENTTRANSFORMATIONMETHOD_H__
#define __ELASTOS_DROID_TEXT_METHOD_REPLACEMENTTRANSFORMATIONMETHOD_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::IGetChars;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * This transformation method causes the characters in the {@link #getOriginal}
 * array to be replaced by the corresponding characters in the
 * {@link #getReplacement} array.
 */
//public abstract
class ECO_PUBLIC ReplacementTransformationMethod
    : public Object
    , public IReplacementTransformationMethod
    , public ITransformationMethod
{
private:
    class ECO_LOCAL ReplacementCharSequence
        : public Object
        , public ICharSequence
        , public IGetChars
    {
    public:
        CAR_INTERFACE_DECL()

        ReplacementCharSequence();

        ~ReplacementCharSequence();

        CARAPI constructor(
            /* [in] */ ICharSequence* source,
            /* [in] */ ArrayOf<Char32>* original,
            /* [in] */ ArrayOf<Char32>* replacement);

        CARAPI GetLength(
            /* [out] */ Int32* number);

        CARAPI GetCharAt(
            /* [in] */ Int32 i,
            /* [out] */ Char32* cr);

        CARAPI SubSequence(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ ICharSequence** csq);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI GetChars(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ ArrayOf<Char32>* dest,
            /* [out] */ Int32 off);

    private:
        AutoPtr<ArrayOf<Char32> > mOriginal, mReplacement;
        AutoPtr<ICharSequence> mSource;
    };

    //static
    class ECO_LOCAL SpannedReplacementCharSequence
        : public ReplacementCharSequence
        , public ISpanned
    {
    public:
        CAR_INTERFACE_DECL()

        SpannedReplacementCharSequence();

        ~SpannedReplacementCharSequence();

        CARAPI constructor(
            /* [in] */ ISpanned* source,
            /* [in] */ ArrayOf<Char32>* original,
            /* [in] */ ArrayOf<Char32>* replacement);

        CARAPI GetLength(
            /* [out] */ Int32* number);

        CARAPI GetCharAt(
            /* [in] */ Int32 i,
            /* [out] */ Char32* cr);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI GetSpans(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ const InterfaceID& type,
            /* [out, callee] */ ArrayOf<IInterface*>** objs);

        CARAPI GetSpanStart(
            /* [in] */ IInterface* tag,
            /* [out] */ Int32* start);

        CARAPI GetSpanEnd(
            /* [in] */ IInterface* tag,
            /* [out] */ Int32* end);

        CARAPI GetSpanFlags(
            /* [in] */ IInterface* tag,
            /* [out] */ Int32* flags);

        CARAPI NextSpanTransition(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ const InterfaceID& type,
            /* [out] */ Int32* offset);

        CARAPI SubSequence(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [out] */ ICharSequence** csq);

    private:
        AutoPtr<ISpanned> mSpanned;
    };

public:

    CAR_INTERFACE_DECL()

    ReplacementTransformationMethod();

    virtual ~ReplacementTransformationMethod();

    /**
     * Returns a CharSequence that will mirror the contents of the
     * source CharSequence but with the characters in {@link #getOriginal}
     * replaced by ones from {@link #getReplacement}.
     */
    CARAPI GetTransformation(
        /* [in] */ ICharSequence* source,
        /* [in] */ IView* v,
        /* [out] */ ICharSequence** ret);

    CARAPI OnFocusChanged(
        /* [in] */ IView* view,
        /* [in] */ ICharSequence* sourceText,
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

protected:
    /**
     * Returns the list of characters that are to be replaced by other
     * characters when displayed.
     */
    virtual CARAPI_(AutoPtr< ArrayOf<Char32> >) GetOriginal() = 0;
    /**
     * Returns a parallel array of replacement characters for the ones
     * that are to be replaced.
     */
    virtual CARAPI_(AutoPtr< ArrayOf<Char32> >) GetReplacement() = 0;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_REPLACEMENTTRANSFORMATIONMETHOD_H__
