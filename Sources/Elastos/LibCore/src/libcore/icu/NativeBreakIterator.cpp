
#include "Elastos.CoreLibrary.Utility.h"
#include "NativeBreakIterator.h"
#include "AutoLock.h"
#include "CStringCharacterIterator.h"
#include "StringBuilder.h"
#include "ElStringByteSink.h"
#include "unicode/brkiter.h"
#include "unicode/ubrk.h"

using Elastos::Core::StringBuilder;
using Libcore::ICU::EIID_INativeBreakIterator;
using Libcore::ICU::NativeBreakIterator;
using Elastos::Text::IStringCharacterIterator;
using Elastos::Text::CStringCharacterIterator;
using Elastos::Text::EIID_ICharacterIterator;

namespace Libcore {
namespace ICU {

extern "C" const _ELASTOS ClassID ECLSID_NativeBreakIterator = {
    { 0xf969b4e7, 0xecc2, 0x483a, { 0x94, 0x7e, 0x2d, 0x3b, 0xc4, 0x57, 0x39, 0xf6 } },
    (char *)c_pElastos_CoreLibraryUunm,
    0xf6f1637e };

const Int32 NativeBreakIterator::BI_CHAR_INSTANCE = 1;
const Int32 NativeBreakIterator::BI_WORD_INSTANCE = 2;
const Int32 NativeBreakIterator::BI_LINE_INSTANCE = 3;
const Int32 NativeBreakIterator::BI_SENT_INSTANCE = 4;

CAR_INTERFACE_IMPL(NativeBreakIterator, Object, INativeBreakIterator)

Object NativeBreakIterator::mLock;

NativeBreakIterator::NativeBreakIterator(
    /* [in] */ Int64 address,
    /* [in] */ Int32 type)
    : mAddress(address)
    , mType(type)
{
    AutoPtr<IStringCharacterIterator> stringCharIter;
    ASSERT_SUCCEEDED(CStringCharacterIterator::New(String(""),
            (IStringCharacterIterator**)&stringCharIter));
    mCharIterator = (ICharacterIterator*)stringCharIter->Probe(EIID_ICharacterIterator);
}

NativeBreakIterator::~NativeBreakIterator()
{
    CloseImpl(mAddress);
}


ECode NativeBreakIterator::Clone(
    /* [out] */ IInterface ** outiter)
{
    VALIDATE_NOT_NULL(outiter);

    Int64 cloneAddr = CloneImpl(mAddress);
    AutoPtr<NativeBreakIterator> clone = new NativeBreakIterator(cloneAddr, mType);
    clone->mString = mString;
    // The RI doesn't clone the CharacterIterator.
    clone->mCharIterator = mCharIterator;
    *outiter = clone->Probe(EIID_IInterface);
    REFCOUNT_ADD(*outiter);

    return NOERROR;
}

ECode NativeBreakIterator::Equals(
    /* [in] */ IInterface * object,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(object)
    VALIDATE_NOT_NULL(value)

    if (object == (IInterface*)this->Probe(EIID_IInterface)) {
        *value = TRUE;
        return NOERROR;
    }
    AutoPtr<INativeBreakIterator> rhs = INativeBreakIterator::Probe(object);
    if (!rhs) {
        *value = FALSE;
        return NOERROR;
    }
    // TODO: is this sufficient? shouldn't we be checking the underlying rules?
    NativeBreakIterator* rv = (NativeBreakIterator*)rhs.Get();
    Boolean typeEqual = mType == rv->mType;
    if (typeEqual == FALSE) {
        *value = FALSE;
        return NOERROR;
    }
    if (mCharIterator == NULL && rv->mCharIterator == NULL) {
        *value = TRUE;
        return NOERROR;
    }
    if (mCharIterator == NULL || rv->mCharIterator == NULL) {
        *value = FALSE;
        return NOERROR;
    }
    AutoPtr<IObject> obj = IObject::Probe(mCharIterator);
    Boolean itEqual;
    obj->Equals(rv->mCharIterator ,&itEqual);
    if (itEqual)
        *value = TRUE;
    return NOERROR;
}

ECode NativeBreakIterator::GetHashCode(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = 42; // No-one uses BreakIterator as a hash key.
    return NOERROR;
}

ECode NativeBreakIterator::GetCurrent(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = CurrentImpl(mAddress, mString);
    return NOERROR;
}

ECode NativeBreakIterator::GetFirst(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = FirstImpl(mAddress, mString);
    return NOERROR;
}

ECode NativeBreakIterator::GetFollowing(
    /* [in] */ Int32 offset,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = FollowingImpl(mAddress, mString, offset);
    return NOERROR;
}

ECode NativeBreakIterator::GetText(
    /* [out] */ ICharacterIterator ** outiter)
{
    VALIDATE_NOT_NULL(outiter)

    Int32 newLocation = CurrentImpl(mAddress, mString);
    Char32 c;
    mCharIterator->SetIndex(newLocation, &c);
    *outiter = mCharIterator;
    REFCOUNT_ADD(*outiter);

    return NOERROR;
}

ECode NativeBreakIterator::GetLast(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = LastImpl(mAddress, mString);
    return NOERROR;
}

ECode NativeBreakIterator::GetNext(
    /* [in] */ Int32 n,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = NextImpl(mAddress, mString, n);
    return NOERROR;
}

ECode NativeBreakIterator::GetNext(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = NextImpl(mAddress, mString, 1);
    return NOERROR;
}

ECode NativeBreakIterator::GetPrevious(
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = PreviousImpl(mAddress, mString);
    return NOERROR;
}

ECode NativeBreakIterator::SetText(
    /* [in] */ ICharacterIterator * newText)
{
    StringBuilder sb;
    Char32 c;
    for (newText->GetFirst(&c); c != ICharacterIterator::DONE; newText->GetNext(&c)) {
        sb += c;
    }
    SetText(sb.ToString(), newText);
    return NOERROR;
}

ECode NativeBreakIterator::SetText(
    /* [in] */ const String& newText)
{
    AutoPtr<IStringCharacterIterator> stringCharIter;
    ASSERT_SUCCEEDED(CStringCharacterIterator::New(newText, (IStringCharacterIterator**)&stringCharIter));
    SetText(newText, (ICharacterIterator*)stringCharIter->Probe(EIID_ICharacterIterator));
    return NOERROR;
}

void NativeBreakIterator::SetText(
    /* [in] */ const String& s,
    /* [in] */ ICharacterIterator* it)
{
    mString = s;
    mCharIterator = it;
    SetTextImpl(mAddress, mString);
}

ECode NativeBreakIterator::HasText(
    /* [out] */ Boolean* hasText)
{
    *hasText = (mString != NULL);
    return NOERROR;
}

ECode NativeBreakIterator::IsBoundary(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)

    *value = IsBoundaryImpl(mAddress, mString, offset);
    return NOERROR;
}

ECode NativeBreakIterator::GetPreceding(
    /* [in] */ Int32 offset,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    *value = PrecedingImpl(mAddress, mString, offset);
    return NOERROR;
}

AutoPtr<INativeBreakIterator> NativeBreakIterator::GetCharacterInstance(
    /* [in] */ ILocale* locale)
{
    String str;
    locale->ToLanguageTag(&str);
    AutoPtr<INativeBreakIterator> nbi = new NativeBreakIterator(GetCharacterInstanceImpl(str), BI_CHAR_INSTANCE);
    return nbi;
}

AutoPtr<INativeBreakIterator> NativeBreakIterator::GetLineInstance(
    /* [in] */ ILocale* locale)
{
    String str;
    locale->ToLanguageTag(&str);
    AutoPtr<INativeBreakIterator> nbi = new NativeBreakIterator(GetLineInstanceImpl(str), BI_LINE_INSTANCE);
    return nbi;
}

AutoPtr<INativeBreakIterator> NativeBreakIterator::GetSentenceInstance(
    /* [in] */ ILocale* locale)
{
    String str;
    locale->ToLanguageTag(&str);
    AutoPtr<INativeBreakIterator> nbi = new NativeBreakIterator(GetSentenceInstanceImpl(str), BI_SENT_INSTANCE);
    return nbi;
}

AutoPtr<INativeBreakIterator> NativeBreakIterator::GetWordInstance(
    /* [in] */ ILocale* locale)
{
    String str;
    locale->ToLanguageTag(&str);
    AutoPtr<INativeBreakIterator> nbi = new NativeBreakIterator(GetWordInstanceImpl(str), BI_WORD_INSTANCE);
    return nbi;
}

// ICU documentation: http://icu-project.org/apiref/icu4c/classBreakIterator.html

static BreakIterator* toBreakIterator(Int64 address) {
  return reinterpret_cast<BreakIterator*>(static_cast<uintptr_t>(address));
}

/**
 * We use ICU4C's BreakIterator class, but our input is on the Java heap and potentially moving
 * around between calls. This wrapper class ensures that our RegexMatcher is always pointing at
 * the current location of the char[]. Earlier versions of Android simply copied the data to the
 * native heap, but that's wasteful and hides allocations from the garbage collector.
 */
class BreakIteratorAccessor
{
public:
    BreakIteratorAccessor(Int64 address, const String& javaInput, Boolean reset)
    {
        init(address);
        mJavaInput = javaInput;

        if (mJavaInput == NULL) {
            return;
        }

        UnicodeString us = UnicodeString::fromUTF8(mJavaInput.string());
        utext_openUnicodeString(mUText, &us, &mStatus);
        if (mUText == NULL) {
            return;
        }

        if (reset) {
            mBreakIterator->setText(mUText, mStatus);
        } else {
            mBreakIterator->refreshInputText(mUText, mStatus);
        }
    }

    BreakIteratorAccessor(Int64 address)
    {
        init(address);
    }

    ~BreakIteratorAccessor()
    {
        utext_close(mUText);
    }

    BreakIterator* operator->()
    {
        return mBreakIterator;
    }

    UErrorCode& status()
    {
        return mStatus;
    }

private:
    void init(Int64 address)
    {
        mJavaInput = NULL;
        mBreakIterator = toBreakIterator(address);
        mStatus = U_ZERO_ERROR;
        mUText = NULL;
    }

    String mJavaInput;
    BreakIterator* mBreakIterator;
    UErrorCode mStatus;
    UText* mUText;

    // Disallow copy and assignment.
    BreakIteratorAccessor(const BreakIteratorAccessor&);
    void operator=(const BreakIteratorAccessor&);
};

#define MAKE_BREAK_ITERATOR_INSTANCE(F) \
  Locale icuLocale; \
  icuLocale.setToBogus(); \
  if (locale == NULL) return 0; \
  icuLocale = Locale::createFromName(locale); \
  if (icuLocale.isBogus()) return 0; \
  UErrorCode status = U_ZERO_ERROR; \
  BreakIterator* it = F(icuLocale, status); \
  if(!U_SUCCESS(status)) return 0; \
  return reinterpret_cast<uintptr_t>(it)

Int64 NativeBreakIterator::GetCharacterInstanceImpl(
    /* [in] */ const String& locale)
{
    MAKE_BREAK_ITERATOR_INSTANCE(BreakIterator::createCharacterInstance);
}

Int64 NativeBreakIterator::GetWordInstanceImpl(
    /* [in] */ const String& locale)
{
    MAKE_BREAK_ITERATOR_INSTANCE(BreakIterator::createWordInstance);
}

Int64 NativeBreakIterator::GetLineInstanceImpl(
    /* [in] */ const String& locale)
{
    MAKE_BREAK_ITERATOR_INSTANCE(BreakIterator::createLineInstance);
}

Int64 NativeBreakIterator::GetSentenceInstanceImpl(
    /* [in] */ const String& locale)
{
    MAKE_BREAK_ITERATOR_INSTANCE(BreakIterator::createSentenceInstance);
}

void NativeBreakIterator::CloseImpl(
    /* [in] */ Int64 address)
{
    synchronized(mLock) {
        delete toBreakIterator(address);
    }
}

Int64 NativeBreakIterator::CloneImpl(
    /* [in] */ Int64 address)
{
    Int64 rev=0;
    synchronized(mLock) {
        BreakIteratorAccessor it(address);
        rev = reinterpret_cast<uintptr_t>(it->clone());
    }
    return rev;
}

void NativeBreakIterator::SetTextImpl(
    /* [in] */ Int64 address,
    /* [in] */ const String& text)
{
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, true);
    }
}

Boolean NativeBreakIterator::IsBoundaryImpl(
    /* [in] */ Int64 address,
    /* [in] */ const String& text,
    /* [in] */ Int32 offset)
{
    Boolean rev = false;
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, false);
        rev = it->isBoundary(offset);
    }
    return rev;
}

Int32 NativeBreakIterator::NextImpl(
    /* [in] */ Int64 address,
    /* [in] */ const String& text,
    /* [in] */ Int32 n)
{
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, false);
        if (n < 0) {
            while (n++ < -1) {
                it->previous();
            }
            return it->previous();
        }
        else if (n == 0) {
            return it->current();
        }
        else {
            while (n-- > 1) {
                it->next();
            }
            return it->next();
        }
    }
    return -1;
}

Int32 NativeBreakIterator::PrecedingImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text,
        /* [in] */ Int32 offset)
{
    Int32 rev = 0;
    synchronized(mLock){
        BreakIteratorAccessor it(address, text, false);
        rev = it->preceding(offset);
    }
    return rev;
}

Int32 NativeBreakIterator::CurrentImpl(
    /* [in] */ Int64 address,
    /* [in] */ const String& text)
{
    Int32 rev = 0;
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, false);
        rev = it->current();
    }
    return rev;
}

Int32 NativeBreakIterator::FirstImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text)
{
    Int32 rev = 0;
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, false);
        rev = it->first();
    }
    return rev;
}

Int32 NativeBreakIterator::FollowingImpl(
    /* [in] */ Int64 address,
    /* [in] */ const String& text,
    /* [in] */ Int32 offset)
{
    Int32 rev = 0;
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, false);
        rev = it->following(offset);
    }
    return rev;
}

Int32 NativeBreakIterator::PreviousImpl(
        /* [in] */ Int64 address,
        /* [in] */ const String& text)
{
    Int32 rev = 0;
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, false);
        rev = it->previous();
    }
    return rev;
}

Int32 NativeBreakIterator::LastImpl(
    /* [in] */ Int64 address,
    /* [in] */ const String& text)
{
    Int32 rev;
    synchronized(mLock) {
        BreakIteratorAccessor it(address, text, false);
        rev = it->last();
    }
    return rev;
}

ECode NativeBreakIterator::ToString(
        /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)

    *info = String("CollationElementIteratorICU");
    return NOERROR;
}

ECode NativeBreakIterator::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    VALIDATE_NOT_NULL(pCLSID)

    *pCLSID = ECLSID_NativeBreakIterator;
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
