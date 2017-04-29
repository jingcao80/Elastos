//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Text::EIID_ITextDirectionAlgorithm;
using Elastos::Droid::View::IView;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICharBuffer;
using Elastos::IO::ICharBufferHelper;
using Elastos::IO::CCharBufferHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Text {

INIT_PROI_3 AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::LTR =
        new TextDirectionHeuristics::TextDirectionHeuristicInternal(NULL /* no algorithm */, FALSE);

INIT_PROI_3 AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::RTL =
        new TextDirectionHeuristics::TextDirectionHeuristicInternal(NULL /* no algorithm */, TRUE);

INIT_PROI_3 AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::FIRSTSTRONG_LTR =
       new TextDirectionHeuristics::TextDirectionHeuristicInternal(
            TextDirectionHeuristics::FirstStrong::INSTANCE, FALSE);

INIT_PROI_3 AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::FIRSTSTRONG_RTL =
       new TextDirectionHeuristics::TextDirectionHeuristicInternal(
            TextDirectionHeuristics::FirstStrong::INSTANCE, TRUE);

INIT_PROI_3 AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::ANYRTL_LTR =
       new TextDirectionHeuristics::TextDirectionHeuristicInternal(
            TextDirectionHeuristics::AnyStrong::INSTANCE_RTL, FALSE);

INIT_PROI_3 AutoPtr<ITextDirectionHeuristic> TextDirectionHeuristics::LOCALE =
        TextDirectionHeuristics::TextDirectionHeuristicLocale::INSTANCE;

const Int32 TextDirectionHeuristics::STATE_TRUE = 0;
const Int32 TextDirectionHeuristics::STATE_FALSE = 1;
const Int32 TextDirectionHeuristics::STATE_UNKNOWN = 2;

INIT_PROI_2 AutoPtr<TextDirectionHeuristics::FirstStrong> TextDirectionHeuristics::FirstStrong::INSTANCE = new FirstStrong();

INIT_PROI_2 AutoPtr<TextDirectionHeuristics::AnyStrong> TextDirectionHeuristics::AnyStrong::INSTANCE_RTL = new AnyStrong(TRUE);
INIT_PROI_2 AutoPtr<TextDirectionHeuristics::AnyStrong> TextDirectionHeuristics::AnyStrong::INSTANCE_LTR = new AnyStrong(FALSE);

INIT_PROI_2 AutoPtr<TextDirectionHeuristics::TextDirectionHeuristicLocale> TextDirectionHeuristics::TextDirectionHeuristicLocale::INSTANCE = new TextDirectionHeuristicLocale();

//============================================================
// TextDirectionHeuristics::TextDirectionHeuristicImpl
//============================================================

CAR_INTERFACE_IMPL(TextDirectionHeuristics::TextDirectionHeuristicImpl, Object, ITextDirectionHeuristic)

TextDirectionHeuristics::TextDirectionHeuristicImpl::TextDirectionHeuristicImpl(
    /* [in] */ ITextDirectionAlgorithm* algorithm)
    : mAlgorithm(algorithm)
{}

TextDirectionHeuristics::TextDirectionHeuristicImpl::~TextDirectionHeuristicImpl()
{}

ECode TextDirectionHeuristics::TextDirectionHeuristicImpl::IsRtl(
    /* [in] */ ArrayOf<Char32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (array == NULL || start < 0 || count < 0 || array->GetLength() - count < start) {
//                throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mAlgorithm == NULL) {
        *result = DefaultIsRtl();
        return NOERROR;
    }
    *result = DoCheck(array, start, count);
    return NOERROR;
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


    AutoPtr< ArrayOf<Char32> > chars = CoreUtils::Unbox(cs).GetChars();
    *result = DoCheck(chars, start, count);

    return NOERROR;
}

Boolean TextDirectionHeuristics::TextDirectionHeuristicImpl::DoCheck(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count)
{
    Int32 state;
    mAlgorithm->CheckRtl(chars, start, count, &state);
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
// TextDirectionHeuristics::TextDirectionHeuristicInternal
//============================================================

TextDirectionHeuristics::TextDirectionHeuristicInternal::TextDirectionHeuristicInternal(
    /* [in] */ ITextDirectionAlgorithm* algorithm,
    /* [in] */ Boolean defaultIsRtl)
    : TextDirectionHeuristicImpl(algorithm)
    , mDefaultIsRtl(defaultIsRtl)
{}

TextDirectionHeuristics::TextDirectionHeuristicInternal::~TextDirectionHeuristicInternal()
{}

Boolean TextDirectionHeuristics::TextDirectionHeuristicInternal::DefaultIsRtl()
{
    return mDefaultIsRtl;
}

//============================================================
// TextDirectionHeuristics::FirstStrong
//============================================================

CAR_INTERFACE_IMPL(TextDirectionHeuristics::FirstStrong, Object, ITextDirectionAlgorithm)

TextDirectionHeuristics::FirstStrong::FirstStrong()
{}

TextDirectionHeuristics::FirstStrong::~FirstStrong()
{}

ECode TextDirectionHeuristics::FirstStrong::CheckRtl(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 result = TextDirectionHeuristics::STATE_UNKNOWN;
    for (Int32 i = start, e = start + count; i < e && result == TextDirectionHeuristics::STATE_UNKNOWN; ++i) {
        result = IsRtlTextOrFormat(Character::GetDirectionality((*chars)[i]));
    }
    *res = result;
    return NOERROR;
}

//============================================================
// TextDirectionHeuristics::AnyStrong
//============================================================

CAR_INTERFACE_IMPL(TextDirectionHeuristics::AnyStrong, Object, ITextDirectionAlgorithm)

TextDirectionHeuristics::AnyStrong::AnyStrong(
    /* [in] */ Boolean lookForRtl)
    : mLookForRtl(lookForRtl)
{}

TextDirectionHeuristics::AnyStrong::~AnyStrong()
{}

ECode TextDirectionHeuristics::AnyStrong::CheckRtl(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean haveUnlookedFor = FALSE;
    for (Int32 i = start, e = start + count; i < e; ++i) {
        switch (IsRtlText(Character::GetDirectionality((*chars)[i]))) {
            case TRUE:
                if (mLookForRtl) {
                    *result = TextDirectionHeuristics::STATE_TRUE;
                    return NOERROR;
                }
                haveUnlookedFor = TRUE;
                break;
            case FALSE:
                if (!mLookForRtl) {
                    *result = TextDirectionHeuristics::STATE_FALSE;
                    return NOERROR;
                }
                haveUnlookedFor = TRUE;
                break;
            default:
                break;
        }
    }
    if (haveUnlookedFor) {
        *result = mLookForRtl ? TextDirectionHeuristics::STATE_FALSE : TextDirectionHeuristics::STATE_TRUE;
        return NOERROR;
    }
    *result = TextDirectionHeuristics::STATE_UNKNOWN;
    return NOERROR;
}

//============================================================
// TextDirectionHeuristics::TextDirectionHeuristicLocale
//============================================================

TextDirectionHeuristics::TextDirectionHeuristicLocale::TextDirectionHeuristicLocale()
    : TextDirectionHeuristicImpl(NULL)
{}

TextDirectionHeuristics::TextDirectionHeuristicLocale::~TextDirectionHeuristicLocale()
{}

Boolean TextDirectionHeuristics::TextDirectionHeuristicLocale::DefaultIsRtl()
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    Int32 dir = TextUtils::GetLayoutDirectionFromLocale(locale);
    return (dir == IView::LAYOUT_DIRECTION_RTL);
}

//============================================================
// TextDirectionHeuristics
//============================================================

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

} // namespace Text
} // namepsace Droid
} // namespace Elastos
