
#ifndef __LIBCORE_ICU_NATIVEBREAKITERATOR_H__
#define __LIBCORE_ICU_NATIVEBREAKITERATOR_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include <Object.h>

using Elastos::Core::Object;
using Elastos::Utility::ILocale;
using Libcore::ICU::INativeBreakIterator;
using Elastos::Text::ICharacterIterator;

namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_NativeBreakIterator;

class NativeBreakIterator
    : public Object
    , public INativeBreakIterator
{
public:
    CAR_INTERFACE_DECL()

    ~NativeBreakIterator();

    CARAPI Clone(
        /* [out] */ IInterface ** outiter);

    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI GetClassID(
        /* [out] */ ClassID *pCLSID);

    CARAPI Equals(
        /* [in] */ IInterface * object,
        /* [out] */ Boolean * value);

    CARAPI GetHashCode(
        /* [out] */ Int32 * value);

    CARAPI GetCurrent(
        /* [out] */ Int32 * value);

    CARAPI GetFirst(
        /* [out] */ Int32 * value);

    CARAPI GetFollowing(
        /* [in] */ Int32 offset,
        /* [out] */ Int32 * value);

    CARAPI GetText(
        /* [out] */ ICharacterIterator ** outiter);

    CARAPI GetLast(
        /* [out] */ Int32 * value);

    CARAPI GetNext(
        /* [in] */ Int32 n,
        /* [out] */ Int32 * value);

    CARAPI GetNext(
        /* [out] */ Int32 * value);

    CARAPI GetPrevious(
        /* [out] */ Int32 * value);

    CARAPI SetText(
        /* [in] */ ICharacterIterator * newText);

    CARAPI SetText(
        /* [in] */ const String& newText);

    CARAPI HasText(
        /* [out] */ Boolean* hasText);

    CARAPI IsBoundary(
        /* [in] */ Int32 offset,
        /* [out] */ Boolean * value);

    CARAPI GetPreceding(
        /* [in] */ Int32 offset,
        /* [out] */ Int32 * value);

    static CARAPI_(AutoPtr<INativeBreakIterator>) GetCharacterInstance(
        /* [in] */ ILocale* where);

    static CARAPI_(AutoPtr<INativeBreakIterator>) GetLineInstance(
        /* [in] */ ILocale* where);

    static CARAPI_(AutoPtr<INativeBreakIterator>) GetSentenceInstance(
        /* [in] */ ILocale* where);

    static CARAPI_(AutoPtr<INativeBreakIterator>) GetWordInstance(
        /* [in] */ ILocale* where);

private:
    NativeBreakIterator(
        /* [in] */ Int64 address,
        /* [in] */ Int32 type);

    CARAPI_(void) SetText(
        /* [in] */ const String& s,
        /* [in] */ ICharacterIterator* it);

    static CARAPI_(Int64) GetCharacterInstanceImpl(
        /* [in] */ const String& locale);

    static CARAPI_(Int64) GetWordInstanceImpl(
        /* [in] */ const String& locale);

    static CARAPI_(Int64) GetLineInstanceImpl(
        /* [in] */ const String& locale);

    static CARAPI_(Int64) GetSentenceInstanceImpl(
        /* [in] */ const String& locale);

    static CARAPI_(void) CloseImpl(
        /* [in] */ Int64 address);

    static CARAPI_(void) SetTextImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text);

    static CARAPI_(Int64) CloneImpl(
        /* [in] */ Int64 address);

    static CARAPI_(Int32) PrecedingImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text,
        /* [in] */ Int32 offset);

    static CARAPI_(Boolean) IsBoundaryImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text,
        /* [in] */ Int32 offset);

    static CARAPI_(Int32) NextImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text,
        /* [in] */ Int32 n);

    static CARAPI_(Int32) PreviousImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text);

    static CARAPI_(Int32) CurrentImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text);

    static CARAPI_(Int32) FirstImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text);

    static CARAPI_(Int32) FollowingImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text,
        /* [in] */ Int32 offset);

    static CARAPI_(Int32) LastImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text);

private:
    // Acceptable values for the 'type' field.
    static const Int32 BI_CHAR_INSTANCE;
    static const Int32 BI_WORD_INSTANCE;
    static const Int32 BI_LINE_INSTANCE;
    static const Int32 BI_SENT_INSTANCE;

    // The address of the native peer.
    // Uses of this must be manually synchronized to avoid native crashes.
    Int64 mAddress;
    String mString;
    Int32 mType;
    AutoPtr<ICharacterIterator> mCharIterator;

    static Object mLock;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_NATIVEBREAKITERATOR_H__