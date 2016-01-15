#ifndef __ELASTOS_TEXT_BREAKITERATOR_H__
#define __ELASTOS_TEXT_BREAKITERATOR_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Object.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::ILocale;
using Elastos::Text::ICharacterIterator;
using Libcore::ICU::INativeBreakIterator;

namespace Elastos {
namespace Text {

class BreakIterator
    : public Object
    , public IBreakIterator
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI GetAvailableLocales(
        /* [out] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetCharacterInstance(
        /* [out] */ IBreakIterator** instance);

    static CARAPI GetCharacterInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);

    static CARAPI GetLineInstance(
        /* [out] */ IBreakIterator** instance);

    static CARAPI GetLineInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);

    static CARAPI GetSentenceInstance(
        /* [out] */ IBreakIterator** instance);

    static CARAPI GetSentenceInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);

    static CARAPI GetWordInstance(
        /* [out] */ IBreakIterator** instance);

    static CARAPI GetWordInstance(
        /* [in] */ ILocale* where,
        /* [out] */ IBreakIterator** instance);

    virtual CARAPI IsBoundary(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean* isBoundary);

    virtual CARAPI GetPreceding(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* position);

    virtual CARAPI SetText(
        /* [in] */ const String& newText);

    virtual CARAPI GetCurrent(
        /* [out] */ Int32* position) = 0;

    virtual CARAPI GetFirst(
        /* [out] */ Int32* position) = 0;

    virtual CARAPI GetFollowing(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* position) = 0;

    virtual CARAPI GetText(
        /* [out] */ ICharacterIterator** text) = 0;

    virtual CARAPI GetLast(
        /* [out] */ Int32* position) = 0;

    virtual CARAPI GetNext(
        /* [out] */ Int32* position) = 0;

    virtual CARAPI GetNext(
        /* [in] */ Int32 n,
        /* [out] */ Int32* position) = 0;

    virtual CARAPI GetPrevious(
        /* [out] */ Int32* position) = 0;

    virtual CARAPI SetText(
        /* [in] */ ICharacterIterator* newText) = 0;

    virtual CARAPI Clone(
        /* [out] */ IInterface ** object) = 0;

    virtual CARAPI CloneImpl(
        /* [in] */ IBreakIterator* object);

    CARAPI constructor(
        /* [in] */ INativeBreakIterator* iterator);
protected:
    BreakIterator();

    BreakIterator(
        /* [in] */ INativeBreakIterator* iterator);

public:
    AutoPtr<INativeBreakIterator> mWrapped;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_BREAKITERATOR_H__
