
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::View::IView;

using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::IO::ICharBuffer;
using Elastos::IO::ICharBufferHelper;
// using Elastos::IO::CCharBufferHelper;

namespace Elastos {
namespace Droid {
namespace Text {

const Int32 TextDirectionHeuristics::STATE_TRUE = 0;
const Int32 TextDirectionHeuristics::STATE_FALSE = 1;
const Int32 TextDirectionHeuristics::STATE_UNKNOWN = 2;

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::LTR;
        // new TextDirectionHeuristics::TextDirectionHeuristicInternal(NULL /* no algorithm */, FALSE);

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::RTL;
        // new TextDirectionHeuristics::TextDirectionHeuristicInternal(NULL /* no algorithm */, TRUE);

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::FIRSTSTRONG_LTR;
       // new TextDirectionHeuristics::TextDirectionHeuristicInternal(
       //      TextDirectionHeuristics::FirstStrong::GetInstance(), FALSE);

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::FIRSTSTRONG_RTL;
       // new TextDirectionHeuristics::TextDirectionHeuristicInternal(
       //  TextDirectionHeuristics::FirstStrong::GetInstance(), TRUE);

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::ANYRTL_LTR;
       // new TextDirectionHeuristics::TextDirectionHeuristicInternal(
       //  TextDirectionHeuristics::AnyStrong::GetLTRInstance(), FALSE);

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::LOCALE;
       //  = TextDirectionHeuristics::TextDirectionHeuristicLocale::GetInstance();

AutoPtr<TextDirectionHeuristics::AnyStrong> TextDirectionHeuristics::AnyStrong::sRTLInstance = new TextDirectionHeuristics::AnyStrong(TRUE);
AutoPtr<TextDirectionHeuristics::AnyStrong> TextDirectionHeuristics::AnyStrong::sLTRInstance = new TextDirectionHeuristics::AnyStrong(FALSE);

AutoPtr<TextDirectionHeuristics::TextDirectionHeuristicLocale> TextDirectionHeuristics::TextDirectionHeuristicLocale::sInstance
        = new TextDirectionHeuristics::TextDirectionHeuristicLocale();

AutoPtr<TextDirectionHeuristics::FirstStrong> TextDirectionHeuristics::FirstStrong::sInstance
        = new TextDirectionHeuristics::FirstStrong();


AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::GetLTR()
{
    return new TextDirectionHeuristics::TextDirectionHeuristicInternal(NULL /* no algorithm */, FALSE);
}

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::GetRTL()
{
    return new TextDirectionHeuristics::TextDirectionHeuristicInternal(NULL /* no algorithm */, TRUE);
}

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::GetFIRSTSTRONG_LTR()
{
    return new TextDirectionHeuristics::TextDirectionHeuristicInternal(
        TextDirectionHeuristics::FirstStrong::GetInstance(), FALSE);
}

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::GetFIRSTSTRONG_RTL()
{
    return new TextDirectionHeuristics::TextDirectionHeuristicInternal(
        TextDirectionHeuristics::FirstStrong::GetInstance(), TRUE);
}

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::GetANYRTL_LTR()
{
    return new TextDirectionHeuristics::TextDirectionHeuristicInternal(
        TextDirectionHeuristics::AnyStrong::GetLTRInstance(), FALSE);
}

AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::GetLOCALE()
{
    return TextDirectionHeuristics::TextDirectionHeuristicLocale::GetInstance();
}

Int32 TextDirectionHeuristics::IsRtlText(
    /* [in] */ Int32 directionality)
{
    if (directionality == Character::DIRECTIONALITY_LEFT_TO_RIGHT) {
        return STATE_FALSE;
    }

    if (directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT
        || directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC) {
        return STATE_TRUE;
    }

    return STATE_UNKNOWN;
}

Int32 TextDirectionHeuristics::IsRtlTextOrFormat(
    /* [in] */ Int32 directionality)
{
    if (directionality == Character::DIRECTIONALITY_LEFT_TO_RIGHT
        || directionality == Character::DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING
        || directionality == Character::DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE) {
        return STATE_FALSE;
    }

    if (directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT
        || directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC
        || directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING
        || directionality == Character::DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE) {
        return STATE_TRUE;
    }

    return STATE_UNKNOWN;
}

//============================================================
// TextDirectionHeuristics::TextDirectionHeuristicImpl
//============================================================
CAR_INTERFACE_IMPL(TextDirectionHeuristics::TextDirectionHeuristicImpl, Object, ITextDirectionHeuristic)

ECode TextDirectionHeuristics::TextDirectionHeuristicImpl::IsRtl(
    /* [in] */ ArrayOf<Char32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharBufferHelper> charBuffer;
    assert(0 && "TODO");
    // CCharBufferHelper::AcquireSingleton((ICharBufferHelper**)&charBuffer);
    AutoPtr<ICharBuffer> cb;
    charBuffer->Wrap(array, (ICharBuffer**)&cb);
    return IsRtl(ICharSequence::Probe(cb), start, count, result);
}

ECode TextDirectionHeuristics::TextDirectionHeuristicImpl::IsRtl(
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (cs == NULL || start < 0 || count < 0) {
//                throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 length;
    cs->GetLength(&length);
    if (length - count < start) {
//                throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mAlgorithm == NULL) {
        *result = DefaultIsRtl();
        return NOERROR;
    }

    *result = DoCheck(cs, start, count);

    return NOERROR;
}

Boolean TextDirectionHeuristics::TextDirectionHeuristicImpl::DoCheck(
    /* [in] */ ICharSequence* chars,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    Int32 state = mAlgorithm->CheckRtl(chars, start, count);
    switch(state) {
        case STATE_TRUE:
            return TRUE;
        case STATE_FALSE:
            return FALSE;
        default:
            return DefaultIsRtl();
    }
}

//============================================================
// TextDirectionHeuristics::TextDirectionHeuristicLocale
//============================================================

Boolean TextDirectionHeuristics::TextDirectionHeuristicLocale::DefaultIsRtl()
{
    AutoPtr<ILocale> locale;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    helper->GetDefault((ILocale**)&locale);
   Int32 dir = TextUtils::GetLayoutDirectionFromLocale(locale);
   return (dir == IView::LAYOUT_DIRECTION_RTL);
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
