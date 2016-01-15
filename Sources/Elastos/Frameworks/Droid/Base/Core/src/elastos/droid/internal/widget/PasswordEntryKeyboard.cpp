
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/widget/PasswordEntryKeyboard.h"
#include "elastos/droid/R.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::InputMethodService::IKeyboard;

using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//0c08c3b7-3cd7-4772-8340-d42f0613bd47
extern "C" const InterfaceID EIID_LatinKey =
        { 0x0c08c3b7, 0x3cd7, 0x4772, { 0x83, 0x40, 0xd4, 0x2f, 0x06, 0x13, 0xbd, 0x47 } };

const String PasswordEntryKeyboard::TAG("PasswordEntryKeyboard");

const Int32 PasswordEntryKeyboard::KEYCODE_SPACE = ' ';
Int32 PasswordEntryKeyboard::sSpacebarVerticalCorrection;
const Int32 PasswordEntryKeyboard::SHIFT_OFF = 0;
const Int32 PasswordEntryKeyboard::SHIFT_ON = 1;
const Int32 PasswordEntryKeyboard::SHIFT_LOCKED = 2;

//===============================================================
// PasswordEntryKeyboard::LatinKey::
//===============================================================
PasswordEntryKeyboard::LatinKey::LatinKey(
    /* [in] */ IResources* res,
    /* [in] */ Row* parent,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IXmlResourceParser* parser)
    : Key(res, parent, x, y, parser)
    , mShiftLockEnabled(FALSE)
    , mEnabled(FALSE)
{
    Int32 length = 0;
    if (mPopupCharacters != NULL && (mPopupCharacters->GetLength(&length), length) == 0) {
        // If there is a keyboard with no keys specified in popupCharacters
        mPopupResId = 0;
    }
}

PInterface PasswordEntryKeyboard::LatinKey::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_LatinKey) {
        return reinterpret_cast<PInterface>(this);
    }
    else {
        return Key::Probe(riid);
    }
}

void PasswordEntryKeyboard::LatinKey::SetEnabled(
    /* [in] */ Boolean enabled)
{
    mEnabled = enabled;
}

void PasswordEntryKeyboard::LatinKey::EnableShiftLock()
{
    mShiftLockEnabled = TRUE;
}

ECode PasswordEntryKeyboard::LatinKey::OnReleased(
    /* [in] */ Boolean inside)
{
    if (!mShiftLockEnabled) {
        return Key::OnReleased(inside);
    }
    else {
        mPressed = !mPressed;
    }
    return NOERROR;
}

ECode PasswordEntryKeyboard::LatinKey::IsInside(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* isInside)
{
    VALIDATE_NOT_NULL(isInside)
    if (!mEnabled) {
        return FALSE;
    }
    Int32 code = (*mCodes)[0];
    if (code == KEYCODE_SHIFT || code == KEYCODE_DELETE) {
        y -= mHeight / 10;
        if (code == KEYCODE_SHIFT) x += mWidth / 6;
        if (code == KEYCODE_DELETE) x -= mWidth / 6;
    }
    else if (code == KEYCODE_SPACE) {
        y += PasswordEntryKeyboard::sSpacebarVerticalCorrection;
    }
    return Key::IsInside(x, y, isInside);
}

//===============================================================
// PasswordEntryKeyboard::
//===============================================================
CAR_INTERFACE_IMPL(PasswordEntryKeyboard, Keyboard, IPasswordEntryKeyboard)

PasswordEntryKeyboard::PasswordEntryKeyboard()
    : mShiftState(SHIFT_OFF)
{}

ECode PasswordEntryKeyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 xmlLayoutResId)
{
    return constructor(context, xmlLayoutResId, 0);
}

ECode PasswordEntryKeyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 xmlLayoutResId,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return constructor(context, xmlLayoutResId, 0, width, height);
}

ECode PasswordEntryKeyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 xmlLayoutResId,
    /* [in] */ Int32 mode)
{
//    super(context, xmlLayoutResId, mode);
    Init(context);
    return NOERROR;
}

ECode PasswordEntryKeyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 xmlLayoutResId,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
//     super(context, xmlLayoutResId, mode, width, height);
    Init(context);
    return NOERROR;
}

void PasswordEntryKeyboard::Init(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    context->GetDrawable(R::drawable::sym_keyboard_shift, (IDrawable**)&mShiftIcon);
    context->GetDrawable(R::drawable::sym_keyboard_shift_locked, (IDrawable**)&mShiftLockIcon);
    res->GetDimensionPixelOffset(
            R::dimen::password_keyboard_spacebar_vertical_correction, &sSpacebarVerticalCorrection);
}

ECode PasswordEntryKeyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layoutTemplateResId,
    /* [in] */ ICharSequence* characters,
    /* [in] */ Int32 columns,
    /* [in] */ Int32 horizontalPadding)
{
//     super(context, layoutTemplateResId, characters, columns, horizontalPadding);
    return NOERROR;
}

AutoPtr<Keyboard::Key> PasswordEntryKeyboard::CreateKeyFromXml(
    /* [in] */ IResources* res,
    /* [in] */ Row* parent,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IXmlResourceParser* parser)
{
    AutoPtr<LatinKey> key = new LatinKey(res, parent, x, y, parser);
    Int32 code = (*(key->mCodes))[0];
    if (code >= 0 && code != '\n' && (code < 32 || code > 127)) {
        String str;
        key->mLabel->ToString(&str);
        Logger::W(TAG, "Key code for %s is not latin-1", (const char*)str);
        CString::New(String(" "), (ICharSequence**)(&(key->mLabel)));
        key->SetEnabled(FALSE);
    }
    switch ((*(key->mCodes))[0]) {
        case 10:
            mEnterKey = key;
            break;
        case -103://PasswordEntryKeyboardView::KEYCODE_F1:
            mF1Key = key;
            break;
        case 32:
            mSpaceKey = key;
            break;
    }
    return key;
}

ECode PasswordEntryKeyboard::SetEnterKeyResources(
    /* [in] */ IResources* res,
    /* [in] */ Int32 previewId,
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 labelId)
{
    if (mEnterKey != NULL) {
        // Reset some of the rarely used attributes.
        mEnterKey->mPopupCharacters = NULL;
        mEnterKey->mPopupResId = 0;
        mEnterKey->mText = NULL;

        mEnterKey->mIconPreview = NULL;
        mEnterKey->mIcon = NULL;
        mEnterKey->mLabel = NULL;
        res->GetDrawable(previewId, (IDrawable**)(&(mEnterKey->mIconPreview)));
        res->GetDrawable(iconId, (IDrawable**)(&(mEnterKey->mIcon)));
        res->GetText(labelId, (ICharSequence**)(&(mEnterKey->mLabel)));

        // Set the initial size of the preview icon
        if (mEnterKey->mIconPreview != NULL) {
            Int32 wd, ht;
            mEnterKey->mIconPreview->GetIntrinsicWidth(&wd);
            mEnterKey->mIconPreview->GetIntrinsicHeight(&ht);
            mEnterKey->mIconPreview->SetBounds(0, 0, wd, ht);
        }
    }
    return NOERROR;
}

ECode PasswordEntryKeyboard::EnableShiftLock()
{
    Int32 i = 0;
    AutoPtr<ArrayOf<Int32> > indices;
    GetShiftKeyIndices((ArrayOf<Int32>**)&indices);
    for (Int32 j = 0; j < indices->GetLength(); j++) {
        Int32 index = (*indices)[j];
        if (index >= 0 && i < mShiftKeys->GetLength()) {
            List<AutoPtr<Key> > keys = GetKeys();
            mShiftKeys->Set(i, keys[index]);
            if ((*mShiftKeys)[i]->Probe(EIID_LatinKey)) {
                AutoPtr<LatinKey> latinKey = reinterpret_cast<LatinKey*>((*mShiftKeys)[i]->Probe(EIID_LatinKey));
                latinKey->EnableShiftLock();
            }
            mOldShiftIcons->Set(i, (*mShiftKeys)[i]->mIcon);
            i++;
        }
    }
    return NOERROR;
}

ECode PasswordEntryKeyboard::SetShiftLocked(
    /* [in] */ Boolean shiftLocked)
{
    for (Int32 i = 0; i < mShiftKeys->GetLength(); i++) {
        AutoPtr<Key> shiftKey = (*mShiftKeys)[i];

        if (shiftKey != NULL) {
            shiftKey->mOn = shiftLocked;
            shiftKey->mIcon = mShiftLockIcon;
        }
    }
    mShiftState = shiftLocked ? SHIFT_LOCKED : SHIFT_ON;
    return NOERROR;
}

ECode PasswordEntryKeyboard::SetShifted(
    /* [in] */ Boolean shiftState,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    Boolean shiftChanged = FALSE;
    if (shiftState == FALSE) {
        shiftChanged = mShiftState != SHIFT_OFF;
        mShiftState = SHIFT_OFF;
    }
    else if (mShiftState == SHIFT_OFF) {
        shiftChanged = mShiftState == SHIFT_OFF;
        mShiftState = SHIFT_ON;
    }
    for (Int32 i = 0; i < mShiftKeys->GetLength(); i++) {
        if ((*mShiftKeys)[i] != NULL) {
            if (shiftState == FALSE) {
                (*mShiftKeys)[i]->mOn = FALSE;
                (*mShiftKeys)[i]->mIcon = (*mOldShiftIcons)[i];
            }
            else if (mShiftState == SHIFT_OFF) {
                (*mShiftKeys)[i]->mOn = FALSE;
                (*mShiftKeys)[i]->mIcon = mShiftIcon;
            }
        }
        else {
            Keyboard::SetShifted(shiftState, rst);
            return NOERROR;
        }
    }
    *rst = shiftChanged;
    return NOERROR;
}

ECode PasswordEntryKeyboard::IsShifted(
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    if ((*mShiftKeys)[0] != NULL) {
        *rst = mShiftState != SHIFT_OFF;
    }
    else {
        return Keyboard::IsShifted(rst);
    }
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
