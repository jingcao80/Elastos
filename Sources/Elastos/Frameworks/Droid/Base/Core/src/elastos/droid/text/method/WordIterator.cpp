
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/text/method/WordIterator.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Text::IBreakIteratorHelper;
using Elastos::Text::CBreakIteratorHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

const Int32 WordIterator::WINDOW_WIDTH = 50;

CAR_INTERFACE_IMPL_2(WordIterator, Object, IWordIterator, ISelectionPositionIterator)

WordIterator::WordIterator()
    : mOffsetShift(0)
{}

WordIterator::~WordIterator()
{}

ECode WordIterator::constructor()
{
    AutoPtr<ILocaleHelper> localeHelp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelp);
    AutoPtr<ILocale> locale;
    localeHelp->GetDefault((ILocale**)&locale);
    return constructor(locale.Get());
}

ECode WordIterator::constructor(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IBreakIteratorHelper> bih;
    CBreakIteratorHelper::AcquireSingleton((IBreakIteratorHelper**)&bih);
    return bih->GetWordInstance(locale, (IBreakIterator**)&mIterator);
}

ECode WordIterator::SetCharSequence(
    /* [in] */ ICharSequence* charSequence,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mOffsetShift = Elastos::Core::Math::Max(0, start - WINDOW_WIDTH);
    Int32 csLen;
    const Int32 windowEnd = Elastos::Core::Math::Min((charSequence->GetLength(&csLen), csLen), end + WINDOW_WIDTH);

    AutoPtr<ISpannableStringBuilder> ssb = ISpannableStringBuilder::Probe(charSequence);
    if (ssb != NULL) {
        ssb->Substring(mOffsetShift, windowEnd, &mString);
    }
    else {
        AutoPtr<ICharSequence> cs;
        charSequence->SubSequence(mOffsetShift, windowEnd, (ICharSequence**)&cs);
        cs->ToString(&mString);
    }
//    mIterator->SetText(mString);
    return NOERROR;
}

ECode WordIterator::Preceding(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 shiftedOffset = offset - mOffsetShift;
    do {
        mIterator->GetPreceding(shiftedOffset, &shiftedOffset);
        if (shiftedOffset == IBreakIterator::DONE) {
            *ret = IBreakIterator::DONE;
            return NOERROR;
        }
        if (IsOnLetterOrDigit(shiftedOffset)) {
            *ret = shiftedOffset + mOffsetShift;
            return NOERROR;
        }
    } while(TRUE);
}

ECode WordIterator::Following(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 shiftedOffset = offset - mOffsetShift;
    do {
       mIterator->GetFollowing(shiftedOffset, &shiftedOffset);
        if (shiftedOffset == IBreakIterator::DONE) {
            *ret = IBreakIterator::DONE;
            return NOERROR;
        }
        if (IsAfterLetterOrDigit(shiftedOffset)) {
            *ret = shiftedOffset + mOffsetShift;
            return NOERROR;
        }
    } while (TRUE);
}

ECode WordIterator::GetBeginning(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    const Int32 shiftedOffset = offset - mOffsetShift;
    CheckOffsetIsValid(shiftedOffset);

    if (IsOnLetterOrDigit(shiftedOffset)) {
        Boolean bIsBoundary = FALSE;
        mIterator->IsBoundary(shiftedOffset, &bIsBoundary);
        if (bIsBoundary) {
            *ret = shiftedOffset + mOffsetShift;
            return NOERROR;
        } else {
            Int32 preceding = 0;
            mIterator->GetPreceding(shiftedOffset, &preceding);
            *ret = preceding + mOffsetShift;
            return NOERROR;
        }
    }
    else {
        if (IsAfterLetterOrDigit(shiftedOffset)) {
            Int32 preceding;
            mIterator->GetPreceding(shiftedOffset, &preceding);
            *ret = preceding + mOffsetShift;
            return NOERROR;
        }
    }

    *ret = IBreakIterator::DONE;
    return NOERROR;
}

ECode WordIterator::GetEnd(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    const Int32 shiftedOffset = offset - mOffsetShift;
    CheckOffsetIsValid(shiftedOffset);

    if (IsAfterLetterOrDigit(shiftedOffset)) {
        Boolean bIsBoundary = FALSE;
        mIterator->IsBoundary(shiftedOffset, &bIsBoundary);
        if (bIsBoundary) {
            *ret = shiftedOffset + mOffsetShift;
            return NOERROR;
        } else {
            Int32 following = 0;
            mIterator->GetFollowing(shiftedOffset, &following);
            *ret = following + mOffsetShift;
            return NOERROR;
        }
    } else {
        if (IsOnLetterOrDigit(shiftedOffset)) {
            Int32 following = 0;
            mIterator->GetFollowing(shiftedOffset, &following);
            *ret = following + mOffsetShift;
            return NOERROR;
        }
    }

    *ret = IBreakIterator::DONE;
    return NOERROR;
}

Boolean WordIterator::IsAfterLetterOrDigit(
    /* [in] */ Int32 shiftedOffset)
{
    if (shiftedOffset >= 1 && shiftedOffset <= mString.GetLength()) {
        //Java:    const Int32 codePoint = mString.codePointBefore(shiftedOffset);
        const Int32 codePoint = (Int32)mString.GetChar(shiftedOffset-1);
        if (Character::IsLetterOrDigit(codePoint)){
            return TRUE;
        }
    }
    return FALSE;
}

Boolean WordIterator::IsOnLetterOrDigit(
    /* [in] */ Int32 shiftedOffset)
{
    if (shiftedOffset >= 0 && shiftedOffset < mString.GetLength()) {
        //Java:    final int codePoint = mString.codePointAt(shiftedOffset);
        const Int32 codePoint = (Int32)mString.GetChar(shiftedOffset-1);
        if (Character::IsLetterOrDigit(codePoint)){
            return TRUE;
        }
    }
    return FALSE;
}

void WordIterator::CheckOffsetIsValid(
    /* [in] */ Int32 shiftedOffset)
{
    if (shiftedOffset < 0 || shiftedOffset > mString.GetLength()) {
        /*
        throw new IllegalArgumentException("Invalid offset: " + (shiftedOffset + mOffsetShift) +
                ". Valid range is [" + mOffsetShift + ", " + (mString.length() + mOffsetShift) +
                "]");
        */
        StringBuilder sb("Invalid offset: ");
        sb += StringUtils::ToString(shiftedOffset + mOffsetShift);
        sb += ". Valid range is [";
        sb += StringUtils::ToString(mOffsetShift);
        sb += ", ";
        sb += StringUtils::ToString(mString.GetLength() + mOffsetShift);
        sb += "]";
        Logger::E(String("WordIterator"), sb.ToString());
    }
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
