#include "elastos/droid/text/method/DigitsKeyListener.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/method/CDigitsKeyListener.h"

using Elastos::Droid::Text::Method::CDigitsKeyListener;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

const Char32 DigitsKeyListener::CHARACTERS0[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
const Char32 DigitsKeyListener::CHARACTERS1[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+' };
const Char32 DigitsKeyListener::CHARACTERS2[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };
const Char32 DigitsKeyListener::CHARACTERS3[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+', '.' };
const Char32* DigitsKeyListener::CHARACTERS[] ={(Char32*)CHARACTERS0, (Char32*)CHARACTERS1, (Char32*)CHARACTERS2, (Char32*)CHARACTERS3};
const Int32 DigitsKeyListener::SIGN = 1;
const Int32 DigitsKeyListener::DECIMAL = 2;

static AutoPtr< ArrayOf< IDigitsKeyListener* > > InitStatic() {
    AutoPtr< ArrayOf< IDigitsKeyListener* > > instance = ArrayOf<IDigitsKeyListener*>::Alloc(4);
    return instance;
}

AutoPtr< ArrayOf<IDigitsKeyListener*> > DigitsKeyListener::sInstance = InitStatic();

AutoPtr< ArrayOf<Char32> > DigitsKeyListener::GetAcceptedChars()
{
    return mAccepted;
}

DigitsKeyListener::DigitsKeyListener()
    : mSign(FALSE)
    , mDecimal(FALSE)
{}

DigitsKeyListener::~DigitsKeyListener()
{}

CAR_INTERFACE_IMPL_6(DigitsKeyListener, Object, IDigitsKeyListener, INumberKeyListener, IBaseKeyListener, IInputFilter, IMetaKeyKeyListener, IKeyListener)

ECode DigitsKeyListener::constructor()
{
    return constructor(FALSE, FALSE);
}

ECode DigitsKeyListener::constructor(
    /* [in] */ Boolean sign,
    /* [in] */ Boolean decimal)
{
    mSign = sign;
    mDecimal = decimal;

    Int32 kind = (sign ? SIGN : 0) | (decimal ? DECIMAL : 0);
    if( 0 > kind && kind > 3 ) {
        return E_NOT_IMPLEMENTED;
    }

    Int32 acceptedLen = 10;
    if(kind == 3) {
        acceptedLen = 12;
    }

    if( kind == 1 && kind == 2 ) {
        acceptedLen = 11;
    }

    AutoPtr< ArrayOf<Char32> > charactersR = ArrayOf<Char32>::Alloc(acceptedLen);
    for(Int32 i = 0; i < acceptedLen; i++) {
        (*charactersR)[i] = CHARACTERS[kind][i];
    }

    mAccepted = charactersR;
    return NOERROR;
}

ECode DigitsKeyListener::GetInstance(
    /* [out] */ IDigitsKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret);
    return GetInstance(FALSE, FALSE, ret);
}

ECode DigitsKeyListener::GetInstance(
    /* [in] */ Boolean sign,
    /* [in] */ Boolean decimal,
    /* [out] */ IDigitsKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 kind = (sign ? SIGN : 0) | (decimal ? DECIMAL : 0);
    if ((*sInstance)[kind] == NULL) {
        AutoPtr<IDigitsKeyListener> instance;
        CDigitsKeyListener::New(sign, decimal, (IDigitsKeyListener**)&instance);
        sInstance->Set(kind, instance);
    }

    *ret = (*sInstance)[kind];
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode DigitsKeyListener::GetInstance(
    /* [in] */ const String& accepted,
    /* [out] */ IDigitsKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<CDigitsKeyListener> dim;
    CDigitsKeyListener::NewByFriend((CDigitsKeyListener**)&dim);

    dim->mAccepted = ArrayOf<Char32>::Alloc(accepted.GetLength());
    dim->mAccepted = accepted.GetChars(0, accepted.GetLength());
    *ret = (IDigitsKeyListener*)dim.Get();
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode DigitsKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    Int32 contentType = IInputType::TYPE_CLASS_NUMBER;
    if (mSign) {
        contentType |= IInputType::TYPE_NUMBER_FLAG_SIGNED;
    }

    if (mDecimal) {
        contentType |= IInputType::TYPE_NUMBER_FLAG_DECIMAL;
    }

    *ret = contentType;
    return NOERROR;
}

ECode DigitsKeyListener::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<ICharSequence> out;
    NumberKeyListener::Filter(source, start, end, dest, dstart, dend, (ICharSequence**)&out);

    if (mSign == FALSE && mDecimal == FALSE) {
        *ret = out;
        REFCOUNT_ADD(*ret);
        return NOERROR;
    }

    if (out != NULL) {
        source = out;
        start = 0;
        out->GetLength(&end);
    }

    Int32 sign = -1;
    Int32 decimal = -1;
    Int32 dlen;
    ICharSequence::Probe(dest)->GetLength(&dlen);
    /*
     * Find out if the existing text has a sign or decimal point characters.
     */

    for (Int32 i = 0; i < dstart; i++) {
        Char32 c;
        ICharSequence::Probe(dest)->GetCharAt(i, &c);

        if (IsSignChar(c)) {
            sign = i;
        }
        else if (IsDecimalPointChar(c)) {
            decimal = i;
        }
    }
    for (Int32 i = dend; i < dlen; i++) {
        Char32 c;
        ICharSequence::Probe(dest)->GetCharAt(i, &c);

        if (IsSignChar(c)) {
            *ret = NULL;
            return NOERROR;    // Nothing can be inserted in front of a '-'.
        }
        else if (IsDecimalPointChar(c)) {
            decimal = i;
        }
    }

    /*
     * If it does, we must strip them out from the source.
     * In addition, a sign character must be the very first character,
     * and nothing can be inserted before an existing '-'.
     * Go in reverse order so the offsets are stable.
     */

    AutoPtr<ISpannableStringBuilder> stripped;

    for (Int32 i = end - 1; i >= start; i--) {
        Char32 c;
        source->GetCharAt(i, &c);
        Boolean strip = FALSE;
        if (IsSignChar(c)) {
            if (i != start || dstart != 0) {
                strip = TRUE;
            }
            else if (sign >= 0) {
                strip = TRUE;
            }
            else {
                sign = i;
            }
        }
        else if (IsDecimalPointChar(c)) {
            if (decimal >= 0) {
                strip = TRUE;
            }
            else {
                decimal = i;
            }
        }

        if (strip) {
            if (end == start + 1) {
                *ret = NULL;
                return NOERROR;  // Only one character, and it was stripped.
            }

            if (stripped == NULL) {
                CSpannableStringBuilder::New(source, start, end, (ISpannableStringBuilder**)&stripped);
            }

            IEditable::Probe(stripped)->Delete(i - start, i + 1 - start);
        }
    }

    if (stripped != NULL) {
        *ret = ICharSequence::Probe(stripped);
        REFCOUNT_ADD(*ret);
        return NOERROR;
    }
    else if (out != NULL) {
        *ret = out;
        REFCOUNT_ADD(*ret);
        return NOERROR;
    }
    else {
        *ret = NULL;
        return NOERROR;
    }
}

Boolean DigitsKeyListener::IsSignChar(
    /* [in] */ const Char32 c)
{
    return c == '-' || c == '+';
}

 // TODO: Needs internationalization
Boolean DigitsKeyListener::IsDecimalPointChar(
    /* [in] */ const Char32 c)
{
    return c == '.';
}

 //override
ECode DigitsKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

CARAPI DigitsKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return MetaKeyKeyListener::ClearMetaKeyState(view, content, states);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
