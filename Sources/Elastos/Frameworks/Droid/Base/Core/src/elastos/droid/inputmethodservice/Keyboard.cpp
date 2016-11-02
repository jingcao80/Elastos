
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/utility/CTypedValue.h"
// #include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/inputmethodservice/Keyboard.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::R;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

const String Keyboard::TAG("Keyboard");
const String Keyboard::TAG_KEYBOARD("Keyboard");
const String Keyboard::TAG_ROW("Row");
const String Keyboard::TAG_KEY("Key");

const Int32 Keyboard::GRID_WIDTH = 10;
const Int32 Keyboard::GRID_HEIGHT = 5;
const Int32 Keyboard::GRID_SIZE = GRID_WIDTH * GRID_HEIGHT;
Float Keyboard::SEARCH_DISTANCE = 1.8f;

CAR_INTERFACE_IMPL(Keyboard::Row, Object, IKeyboardRow);
Keyboard::Row::Row(
    /* [in] */ Keyboard* parent)
    : mDefaultWidth(0)
    , mDefaultHeight(0)
    , mDefaultHorizontalGap(0)
    , mVerticalGap(0)
    , mRowEdgeFlags(0)
    , mMode(0)
    , mParent(parent)
{
    CArrayList::New((IArrayList**)&mKeys);
}

Keyboard::Row::Row(
    /* [in] */ IResources* res,
    /* [in] */ Keyboard* parent,
    /* [in] */ IXmlResourceParser* parser)
    : mDefaultWidth(0)
    , mDefaultHeight(0)
    , mDefaultHorizontalGap(0)
    , mVerticalGap(0)
    , mRowEdgeFlags(0)
    , mMode(0)
    , mParent(parent)
{
    CArrayList::New((IArrayList**)&mKeys);

    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::Keyboard);

    AutoPtr<ITypedArray> a;
    res->ObtainAttributes(Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), layout, (ITypedArray**)&a);
    mDefaultWidth = Keyboard::GetDimensionOrFraction(a,
            R::styleable::Keyboard_keyWidth,
            parent->mDisplayWidth, parent->mDefaultWidth);
    mDefaultHeight = Keyboard::GetDimensionOrFraction(a,
            R::styleable::Keyboard_keyHeight,
            parent->mDisplayHeight, parent->mDefaultHeight);
    mDefaultHorizontalGap = Keyboard::GetDimensionOrFraction(a,
            R::styleable::Keyboard_horizontalGap,
            parent->mDisplayWidth, parent->mDefaultHorizontalGap);
    mVerticalGap = Keyboard::GetDimensionOrFraction(a,
            R::styleable::Keyboard_verticalGap,
            parent->mDisplayHeight, parent->mDefaultVerticalGap);
    a->Recycle();
    a = NULL;

    layout = TO_ATTRS_ARRAYOF(R::styleable::Keyboard_Row);

    res->ObtainAttributes(Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), layout, (ITypedArray**)&a);
    a->GetInt32(R::styleable::Keyboard_Row_rowEdgeFlags,
            0, &mRowEdgeFlags);
    a->GetResourceId(R::styleable::Keyboard_Row_keyboardMode,
            0, &mMode);
    a->Recycle();
}

ECode Keyboard::Row::GetDefaultWidth(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mDefaultWidth;
    return NOERROR;
}

ECode Keyboard::Row::SetDefaultWidth(
    /* [in] */ Int32 value)
{
    mDefaultWidth = value;
    return NOERROR;
}

ECode Keyboard::Row::GetDefaultHeight(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mDefaultHeight;
    return NOERROR;
}

ECode Keyboard::Row::SetDefaultHeight(
    /* [in] */ Int32 value)
{
    mDefaultHeight = value;
    return NOERROR;
}

ECode Keyboard::Row::GetDefaultHorizontalGap(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mDefaultHorizontalGap;
    return NOERROR;
}

ECode Keyboard::Row::SetDefaultHorizontalGap(
    /* [in] */ Int32 value)
{
    mDefaultHorizontalGap = value;
    return NOERROR;
}

ECode Keyboard::Row::GetVerticalGap(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mVerticalGap;
    return NOERROR;
}

ECode Keyboard::Row::SetVerticalGap(
    /* [in] */ Int32 value)
{
    mVerticalGap = value;
    return NOERROR;
}

ECode Keyboard::Row::GetRowEdgeFlags(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mRowEdgeFlags;
    return NOERROR;
}

ECode Keyboard::Row::SetRowEdgeFlags(
    /* [in] */ Int32 value)
{
    mRowEdgeFlags = value;
    return NOERROR;
}

ECode Keyboard::Row::GetMode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mMode;
    return NOERROR;
}

ECode Keyboard::Row::SetMode(
    /* [in] */ Int32 value)
{
    mMode = value;
    return NOERROR;
}


Int32 Keyboard::Key::KEY_STATE_NORMAL_ON[] = {
    R::attr::state_checkable,
    R::attr::state_checked
};

Int32 Keyboard::Key::KEY_STATE_PRESSED_ON[] = {
    R::attr::state_pressed,
    R::attr::state_checkable,
    R::attr::state_checked
};

Int32 Keyboard::Key::KEY_STATE_NORMAL_OFF[] = {
    R::attr::state_checkable
};

Int32 Keyboard::Key::KEY_STATE_PRESSED_OFF[] = {
    R::attr::state_pressed,
    R::attr::state_checkable
};

Int32 Keyboard::Key::KEY_STATE_NORMAL[] = {
};

Int32 Keyboard::Key::KEY_STATE_PRESSED[] = {
    R::attr::state_pressed,
};

CAR_INTERFACE_IMPL(Keyboard::Key, Object, IKeyboardKey);
Keyboard::Key::Key(
    /* [in] */ Row* parent)
    : mWidth(parent->mDefaultWidth)
    , mHeight(parent->mDefaultHeight)
    , mGap(parent->mDefaultHorizontalGap)
    , mSticky(FALSE)
    , mX(0)
    , mY(0)
    , mPressed(FALSE)
    , mOn(FALSE)
    , mEdgeFlags(parent->mRowEdgeFlags)
    , mModifier(FALSE)
    , mPopupResId(0)
    , mRepeatable(FALSE)
    , mKeyboard(parent->mParent)
{}

Keyboard::Key::Key(
    /* [in] */ IResources* res,
    /* [in] */ Row* parent,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IXmlResourceParser* parser)
    : mWidth(parent->mDefaultWidth)
    , mHeight(parent->mDefaultHeight)
    , mGap(parent->mDefaultHorizontalGap)
    , mSticky(FALSE)
    , mX(x)
    , mY(y)
    , mPressed(FALSE)
    , mOn(FALSE)
    , mEdgeFlags(parent->mRowEdgeFlags)
    , mModifier(FALSE)
    , mPopupResId(0)
    , mRepeatable(FALSE)
    , mKeyboard(parent->mParent)
{
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::Keyboard);

    AutoPtr<ITypedArray> a;
    res->ObtainAttributes(Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), layout, (ITypedArray**)&a);

    mWidth = Keyboard::GetDimensionOrFraction(a,
            R::styleable::Keyboard_keyWidth,
            mKeyboard->mDisplayWidth, parent->mDefaultWidth);
    mHeight = Keyboard::GetDimensionOrFraction(a,
            R::styleable::Keyboard_keyHeight,
            mKeyboard->mDisplayHeight, parent->mDefaultHeight);
    mGap = Keyboard::GetDimensionOrFraction(a,
            R::styleable::Keyboard_horizontalGap,
            mKeyboard->mDisplayWidth, parent->mDefaultHorizontalGap);
    a->Recycle();
    a = NULL;
    layout = TO_ATTRS_ARRAYOF(R::styleable::Keyboard_Key);

    res->ObtainAttributes(Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), layout, (ITypedArray**)&a);
    mX += mGap;
    AutoPtr<ITypedValue> codesValue;
    CTypedValue::New((ITypedValue**)&codesValue);
    Boolean ret = FALSE;
    a->GetValue(R::styleable::Keyboard_Key_codes,
            codesValue, &ret);
    Int32 type;
    codesValue->GetType(&type);
    if (type == ITypedValue::TYPE_INT_DEC
            || type == ITypedValue::TYPE_INT_HEX) {
        mCodes = ArrayOf<Int32>::Alloc(1);
        Int32 data;
        codesValue->GetData(&data);
        (*mCodes)[0] = data;
    }
    else if (type == ITypedValue::TYPE_STRING) {
        AutoPtr<ICharSequence> string;
        codesValue->GetString((ICharSequence**)&string);
        String str;
        string->ToString(&str);
        mCodes = ParseCSV(str);
    }

    a->GetDrawable(R::styleable::Keyboard_Key_iconPreview,
            (IDrawable**)&mIconPreview);
    if (mIconPreview != NULL) {
        Int32 intrinsicWidth = 0, intrinsicHeight = 0;
        mIconPreview->GetIntrinsicWidth(&intrinsicWidth);
        mIconPreview->GetIntrinsicHeight(&intrinsicHeight);
        mIconPreview->SetBounds(0, 0, intrinsicWidth, intrinsicHeight);
    }
    a->GetText(
            R::styleable::Keyboard_Key_popupCharacters,
            (ICharSequence**)&mPopupCharacters);
    a->GetResourceId(
            R::styleable::Keyboard_Key_popupKeyboard, 0, &mPopupResId);
    a->GetBoolean(
            R::styleable::Keyboard_Key_isRepeatable, FALSE, &mRepeatable);
    a->GetBoolean(
            R::styleable::Keyboard_Key_isModifier, FALSE, &mModifier);
    a->GetBoolean(
            R::styleable::Keyboard_Key_isSticky, FALSE, &mSticky);
    a->GetInt32(R::styleable::Keyboard_Key_keyEdgeFlags, 0, &mEdgeFlags);
    mEdgeFlags |= parent->mRowEdgeFlags;

    a->GetDrawable(
            R::styleable::Keyboard_Key_keyIcon, (IDrawable**)&mIcon);
    if (mIcon != NULL) {
        Int32 intrinsicWidth = 0, intrinsicHeight = 0;
        mIcon->GetIntrinsicWidth(&intrinsicWidth);
        mIcon->GetIntrinsicHeight(&intrinsicHeight);
        mIcon->SetBounds(0, 0, intrinsicWidth, intrinsicHeight);
    }
    a->GetText(R::styleable::Keyboard_Key_keyLabel,
            (ICharSequence**)&mLabel);
    a->GetText(R::styleable::Keyboard_Key_keyOutputText,
            (ICharSequence**)&mText);

    Int32 len;
    if (mCodes == NULL && mLabel != NULL && (mLabel->GetLength(&len), len > 0)/* !TextUtils::IsEmpty(mLabel)*/) {
        mCodes = ArrayOf<Int32>::Alloc(1);
        Char32 c = 0;
        mLabel->GetCharAt(0, &c);
        (*mCodes)[0] = c;
    }
    a->Recycle();
}

Keyboard::Key::~Key()
{}

ECode Keyboard::Key::OnPressed()
{
    mPressed = !mPressed;
    return NOERROR;
}

ECode Keyboard::Key::OnReleased(
    /* [in] */ Boolean inside)
{
    mPressed = !mPressed;
    if (mSticky) {
        mOn = !mOn;
    }
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > Keyboard::Key::ParseCSV(
    /* [in] */ const String& value)
{
    AutoPtr< ArrayOf<String> > strValues;
    StringUtils::Split(value, String(","), (ArrayOf<String>**)&strValues);

    AutoPtr< ArrayOf<Int32> > values;
    if (strValues && strValues->GetLength() > 0) {
        Int32 count = strValues->GetLength();
        Int32 value = 0;
        values = ArrayOf<Int32>::Alloc(count);
        for (Int32 i = 0; i < count; ++i) {
            value = StringUtils::ParseInt32((*strValues)[i]);
            values->Set(i, value);
        }
    }

    return values;
}

ECode Keyboard::Key::IsInside(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* isInside)
{
    VALIDATE_NOT_NULL(isInside);
    Boolean leftEdge = (mEdgeFlags & IKeyboard::EDGE_LEFT) > 0;
    Boolean rightEdge = (mEdgeFlags & IKeyboard::EDGE_RIGHT) > 0;
    Boolean topEdge = (mEdgeFlags & IKeyboard::EDGE_TOP) > 0;
    Boolean bottomEdge = (mEdgeFlags & IKeyboard::EDGE_BOTTOM) > 0;
    if ((x >= mX || (leftEdge && x <= mX + mWidth))
            && (x < mX + mWidth || (rightEdge && x >= mX))
            && (y >= mY || (topEdge && y <= mY + mHeight))
            && (y < mY + mHeight || (bottomEdge && y >= mY))) {
        *isInside = TRUE;
        return NOERROR;
    }
    else {
        *isInside = FALSE;
        return NOERROR;
    }
}

ECode Keyboard::Key::SquaredDistanceFrom(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Int32* dis)
{
    VALIDATE_NOT_NULL(dis);
    Int32 xDist = mX + mWidth / 2 - x;
    Int32 yDist = mY + mHeight / 2 - y;
    *dis = xDist * xDist + yDist * yDist;
    return NOERROR;
}

ECode Keyboard::Key::GetCurrentDrawableState(
    /* [out, callee] */ ArrayOf<Int32>** drawableStates)
{
    VALIDATE_NOT_NULL(drawableStates);
    AutoPtr< ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(0);

    if (mOn) {
        if (mPressed) {
            states = ArrayOf<Int32>::Alloc(3);
            (*states)[0] = KEY_STATE_PRESSED_ON[0];
            (*states)[1] = KEY_STATE_PRESSED_ON[1];
            (*states)[2] = KEY_STATE_PRESSED_ON[2];
        }
        else {
            states = ArrayOf<Int32>::Alloc(2);
            (*states)[0] = KEY_STATE_NORMAL_ON[0];
            (*states)[1] = KEY_STATE_NORMAL_ON[1];
        }
    }
    else {
        if (mSticky) {
            if (mPressed) {
                states = ArrayOf<Int32>::Alloc(2);
                (*states)[0] = KEY_STATE_PRESSED_OFF[0];
                (*states)[1] = KEY_STATE_PRESSED_OFF[1];
            }
            else {
                states = ArrayOf<Int32>::Alloc(1);
                (*states)[0] = KEY_STATE_NORMAL_OFF[0];
            }
        }
        else {
            if (mPressed) {
                states = ArrayOf<Int32>::Alloc(1);
                (*states)[0] = KEY_STATE_PRESSED[0];
            }
        }
    }
    *drawableStates = states;
    REFCOUNT_ADD(*drawableStates);
    return NOERROR;
}

ECode Keyboard::Key::GetCodes(
    /* [out, callee] */ ArrayOf<Int32>** codes)
{
    VALIDATE_NOT_NULL(codes);
    *codes = mCodes;
    REFCOUNT_ADD(*codes);
    return NOERROR;
}

ECode Keyboard::Key::SetCodes(
    /* [in] */ ArrayOf<Int32>* codes)
{
    mCodes = codes;
    return NOERROR;
}

ECode Keyboard::Key::GetLabel(
    /* [in] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode Keyboard::Key::SetLabel(
    /* [in] */ ICharSequence* label)
{
    mLabel = label;
    return NOERROR;
}

ECode Keyboard::Key::GetIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode Keyboard::Key::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode Keyboard::Key::GetIconPreview(
    /* [out] */ IDrawable** preview)
{
    VALIDATE_NOT_NULL(preview);
    *preview = mIconPreview;
    REFCOUNT_ADD(*preview);
    return NOERROR;
}

ECode Keyboard::Key::SetIconPreview(
    /* [in] */ IDrawable* preview)
{
    mIconPreview = preview;
    return NOERROR;
}

ECode Keyboard::Key::GetWidth(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mWidth;
    return NOERROR;
}

ECode Keyboard::Key::SetWidth(
    /* [in] */ Int32 value)
{
    mWidth = value;
    return NOERROR;
}

ECode Keyboard::Key::GetHeight(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mHeight;
    return NOERROR;
}

ECode Keyboard::Key::SetHeight(
    /* [in] */ Int32 value)
{
    mHeight = value;
    return NOERROR;
}

ECode Keyboard::Key::GetGap(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mGap;
    return NOERROR;
}

ECode Keyboard::Key::SetGap(
    /* [in] */ Int32 value)
{
    mGap = value;
    return NOERROR;
}

ECode Keyboard::Key::IsSticky(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mOn;
    return NOERROR;
}

ECode Keyboard::Key::SetSticky(
    /* [in] */ Boolean value)
{
    mOn = value;
    return NOERROR;
}

ECode Keyboard::Key::GetX(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mX;
    return NOERROR;
}

ECode Keyboard::Key::SetX(
    /* [in] */ Int32 value)
{
    mX = value;
    return NOERROR;
}

ECode Keyboard::Key::GetY(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mY;
    return NOERROR;
}

ECode Keyboard::Key::SetY(
    /* [in] */ Int32 value)
{
    mY = value;
    return NOERROR;
}

ECode Keyboard::Key::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode Keyboard::Key::GetPopupCharacters(
    /* [out] */ ICharSequence** popupCharacters)
{
    VALIDATE_NOT_NULL(popupCharacters);
    *popupCharacters = mPopupCharacters;
    REFCOUNT_ADD(*popupCharacters);
    return NOERROR;
}

ECode Keyboard::Key::GetPopupResId(
    /* [out] */ Int32* popupResId)
{
    VALIDATE_NOT_NULL(popupResId);
    *popupResId = mPopupResId;
    return NOERROR;
}

ECode Keyboard::Key::IsRepeatable(
    /* [out] */ Boolean* repeatable)
{
    VALIDATE_NOT_NULL(repeatable);
    *repeatable = mRepeatable;
    return NOERROR;
}

CAR_INTERFACE_IMPL(Keyboard, Object, IKeyboard);
Keyboard::Keyboard()
    : mDefaultHorizontalGap(0)
    , mDefaultWidth(0)
    , mDefaultHeight(0)
    , mDefaultVerticalGap(0)
    , mShifted(FALSE)
    , mKeyWidth(0)
    , mKeyHeight(0)
    , mTotalHeight(0)
    , mTotalWidth(0)
    , mDisplayWidth(0)
    , mDisplayHeight(0)
    , mKeyboardMode(0)
    , mCellWidth(0)
    , mCellHeight(0)
    , mProximityThreshold(0)
{}

ECode Keyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 xmlLayoutResId)
{
    return constructor(context, xmlLayoutResId, 0);
}

ECode Keyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 xmlLayoutResId,
    /* [in] */ Int32 modeId,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mShiftKeys = ArrayOf<Key*>::Alloc(2);
    mShiftKeys->Set(0, NULL);
    mShiftKeys->Set(1, NULL);
    mShiftKeyIndices = ArrayOf<Int32>::Alloc(2);
    (*mShiftKeyIndices)[0] = -1;
    (*mShiftKeyIndices)[1] = -1;
    CArrayList::New((IArrayList**)&mRows);

    mDisplayWidth = width;
    mDisplayHeight = height;

    mDefaultHorizontalGap = 0;
    mDefaultWidth = mDisplayWidth / 10;
    mDefaultVerticalGap = 0;
    mDefaultHeight = mDefaultWidth;
    CArrayList::New((IList**)&mKeys);
    CArrayList::New((IList**)&mModifierKeys);
    mKeyboardMode = modeId;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IXmlResourceParser> parser;
    res->GetXml(xmlLayoutResId, (IXmlResourceParser**)&parser);
    LoadKeyboard(context, parser);
    return NOERROR;
}

ECode Keyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 xmlLayoutResId,
    /* [in] */ Int32 modeId)
{
    mShiftKeys = ArrayOf<Key*>::Alloc(2);
    mShiftKeys->Set(0, NULL);
    mShiftKeys->Set(1, NULL);
    mShiftKeyIndices = ArrayOf<Int32>::Alloc(2);
    (*mShiftKeyIndices)[0] = -1;
    (*mShiftKeyIndices)[1] = -1;
    CArrayList::New((IArrayList**)&mRows);

    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res));
    AutoPtr<IDisplayMetrics> dm;
    FAIL_RETURN(res->GetDisplayMetrics((IDisplayMetrics**)&dm));
    dm->GetWidthPixels(&mDisplayWidth);
    dm->GetHeightPixels(&mDisplayHeight);
    //Log.v(TAG, "keyboard's display metrics:" + dm);

    mDefaultHorizontalGap = 0;
    mDefaultWidth = mDisplayWidth / 10;
    mDefaultVerticalGap = 0;
    mDefaultHeight = mDefaultWidth;
    CArrayList::New((IList**)&mKeys);
    CArrayList::New((IList**)&mModifierKeys);
    mKeyboardMode = modeId;
    AutoPtr<IXmlResourceParser> parser;
    FAIL_RETURN(res->GetXml(xmlLayoutResId, (IXmlResourceParser**)&parser));
    LoadKeyboard(context, parser);
    return NOERROR;
}

ECode Keyboard::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layoutTemplateResId,
    /* [in] */ ICharSequence* characters,
    /* [in] */ Int32 columns,
    /* [in] */ Int32 horizontalPadding)
{
    constructor(context, layoutTemplateResId);
    Int32 x = 0;
    Int32 y = 0;
    Int32 column = 0;
    mTotalWidth = 0;

    AutoPtr<Row> row = new Row(this);
    row->mDefaultHeight = mDefaultHeight;
    row->mDefaultWidth = mDefaultWidth;
    row->mDefaultHorizontalGap = mDefaultHorizontalGap;
    row->mVerticalGap = mDefaultVerticalGap;
    row->mRowEdgeFlags = EDGE_TOP | EDGE_BOTTOM;
    Int32 maxColumns = columns == -1 ? Elastos::Core::Math::INT32_MAX_VALUE : columns;
    Int32 len = 0;
    characters->GetLength(&len);
    for (Int32 i = 0; i < len; i++) {
        Char32 c = 0;
        characters->GetCharAt(i, &c);
        if (column >= maxColumns
                || x + mDefaultWidth + horizontalPadding > mDisplayWidth) {
            x = 0;
            y += mDefaultVerticalGap + mDefaultHeight;
            column = 0;
        }
        AutoPtr<Key> key = new Key(row);
        key->mX = x;
        key->mY = y;
        String str("");
        str.Append(c);
        AutoPtr<ICharSequence> label;
        CString::New(str, (ICharSequence**)&label);
        key->mLabel = label;
        key->mCodes = ArrayOf<Int32>::Alloc(1);
        (*key->mCodes)[0] = c;
        column++;
        x += key->mWidth + key->mGap;
        mKeys->Add((IKeyboardKey*)key);
        row->mKeys->Add((IKeyboardKey*)key);
        if (x > mTotalWidth) {
            mTotalWidth = x;
        }
    }
    mTotalHeight = y + mDefaultHeight;
    mRows->Add((IKeyboardRow*)row);
    return NOERROR;
}

Keyboard::~Keyboard()
{
    mGridNeighbors = NULL;
}

void Keyboard::Resize(
    /* [in] */ Int32 newWidth,
    /* [in] */ Int32 newHeight)
{
    Int32 numRows;
    mRows->GetSize(&numRows);
    for (Int32 rowIndex = 0; rowIndex < numRows; ++rowIndex) {
        AutoPtr<IInterface> obj;
        mRows->Get(rowIndex, (IInterface**)&obj);
        Row* row = (Row*)IKeyboardRow::Probe(obj);
        Int32 numKeys;
        row->mKeys->GetSize(&numKeys);
        Int32 totalGap = 0;
        Int32 totalWidth = 0;
        for (Int32 keyIndex = 0; keyIndex < numKeys; ++keyIndex) {
            AutoPtr<IInterface> tmp;
            row->mKeys->Get(keyIndex, (IInterface**)&tmp);
            Key* key = (Key*)IKeyboardKey::Probe(tmp);
            if (keyIndex > 0) {
                totalGap += key->mGap;
            }
            totalWidth += key->mWidth;
        }
        if (totalGap + totalWidth > newWidth) {
            Int32 x = 0;
            Float scaleFactor = (Float)(newWidth - totalGap) / totalWidth;
            for (Int32 keyIndex = 0; keyIndex < numKeys; ++keyIndex) {
                AutoPtr<IInterface> tmp;
                row->mKeys->Get(keyIndex, (IInterface**)&tmp);
                Key* key = (Key*)IKeyboardKey::Probe(tmp);

                key->mWidth *= scaleFactor;
                key->mX = x;
                x += key->mWidth + key->mGap;
            }
        }
    }
    mTotalWidth = newWidth;
    // TODO: This does not adjust the vertical placement according to the new size.
    // The main problem in the previous code was horizontal placement/size, but we should
    // also recalculate the vertical sizes/positions when we get this resize call.
}

ECode Keyboard::GetKeys(
    /* [out] */ IList** keys)
{
    VALIDATE_NOT_NULL(keys)

    *keys = mKeys;
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode Keyboard::GetModifierKeys(
    /* [out] */ IList** keys)
{
    VALIDATE_NOT_NULL(keys)

    *keys = mModifierKeys;
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode Keyboard::GetHorizontalGap(
    /* [out] */ Int32* hGap)
{
    assert(hGap != NULL);
    *hGap = mDefaultHorizontalGap;
    return NOERROR;
}

ECode Keyboard::SetHorizontalGap(
    /* [in] */ Int32 gap)
{
    mDefaultHorizontalGap = gap;
    return NOERROR;
}

ECode Keyboard::GetVerticalGap(
    /* [out] */ Int32* vGap)
{
    assert(vGap != NULL);
    *vGap = mDefaultVerticalGap;
    return NOERROR;
}

ECode Keyboard::SetVerticalGap(
    /* [in] */ Int32 gap)
{
    mDefaultVerticalGap = gap;
    return NOERROR;
}

ECode Keyboard::GetKeyHeight(
    /* [out] */ Int32* h)
{
    assert(h != NULL);
    *h = mDefaultHeight;
    return NOERROR;
}

ECode Keyboard::SetKeyHeight(
    /* [in] */ Int32 height)
{
    mDefaultHeight = height;
    return NOERROR;
}

ECode Keyboard::GetKeyWidth(
    /* [out] */ Int32* width)
{
    assert(width != NULL);
    *width = mDefaultWidth;
    return NOERROR;
}

ECode Keyboard::SetKeyWidth(
    /* [in] */ Int32 width)
{
    mDefaultWidth = width;
    return NOERROR;
}

ECode Keyboard::GetHeight(
    /* [out] */ Int32* h)
{
    assert(h != NULL);
    *h = mTotalHeight;
    return NOERROR;
}

ECode Keyboard::GetMinWidth(
    /* [out] */ Int32* minW)
{
    assert(minW != NULL);
    *minW = mTotalWidth;
    return NOERROR;
}

ECode Keyboard::SetShifted(
    /* [in] */ Boolean shiftState,
    /* [out] */ Boolean* res)
{
    assert(res != NULL);
    for (Int32 i = 0; i < mShiftKeys->GetLength(); i++) {
        AutoPtr<Key> shiftKey = (*mShiftKeys)[i];
        if (shiftKey != NULL) {
            shiftKey->mOn = shiftState;
        }
    }
    if (mShifted != shiftState) {
        mShifted = shiftState;
        *res = TRUE;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

ECode Keyboard::IsShifted(
    /* [out] */ Boolean* res)
{
    assert(res != NULL);
    *res = mShifted;
    return NOERROR;
}

ECode Keyboard::GetShiftKeyIndices(
    /* [out, callee] */ ArrayOf<Int32>** keyIndices)
{
    assert(keyIndices != NULL);
    *keyIndices = mShiftKeyIndices;
    REFCOUNT_ADD(*keyIndices);
    return NOERROR;
}

ECode Keyboard::GetShiftKeyIndex(
    /* [out] */ Int32* index)
{
    assert(index != NULL);
    *index = (*mShiftKeyIndices)[0];
    return NOERROR;
}

void Keyboard::ComputeNearestNeighbors()
{
    // Round-up so we don't have any pixels outside the grid
    Int32 w = 0, h = 0;
    GetMinWidth(&w);
    GetHeight(&h);
    mCellWidth = (w + GRID_WIDTH - 1) / GRID_WIDTH;
    mCellHeight = (h + GRID_HEIGHT - 1) / GRID_HEIGHT;
    mGridNeighbors = ArrayOf< AutoPtr<ArrayOf<Int32> > >::Alloc(GRID_SIZE);
    Int32 size;
    mKeys->GetSize(&size);
    AutoPtr< ArrayOf<Int32> > indices = ArrayOf<Int32>::Alloc(size);
    Int32 gridWidth = GRID_WIDTH * mCellWidth;
    Int32 gridHeight = GRID_HEIGHT * mCellHeight;
    for (Int32 x = 0; x < gridWidth; x += mCellWidth) {
        for (Int32 y = 0; y < gridHeight; y += mCellHeight) {
            Int32 count = 0;
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mKeys->Get(i, (IInterface**)&obj);
                Key* key = (Key*)IKeyboardKey::Probe(obj);

                Int32 dis1 = 0, dis2 = 0, dis3 = 0, dis4 = 0;
                key->SquaredDistanceFrom(x, y, &dis1);
                key->SquaredDistanceFrom(x + mCellWidth - 1, y, &dis2);
                key->SquaredDistanceFrom(x + mCellWidth - 1, y + mCellHeight - 1, &dis3);
                key->SquaredDistanceFrom(x, y + mCellHeight - 1, &dis4);
                if (dis1 < mProximityThreshold ||
                    dis2 < mProximityThreshold ||
                    dis3 < mProximityThreshold ||
                    dis4 < mProximityThreshold) {
                    (*indices)[count++] = i;
                }
            }
            AutoPtr<ArrayOf<Int32> > cell = ArrayOf<Int32>::Alloc(count);
            cell->Copy(indices, count);
            mGridNeighbors->Set((y / mCellHeight) * GRID_WIDTH + (x / mCellWidth), cell);
        }
    }
}

ECode Keyboard::GetNearestKeys(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out, callee] */ ArrayOf<Int32>** nearestKeys)
{
    if (mGridNeighbors == NULL) {
        ComputeNearestNeighbors();
    }
    Int32 w = 0, h = 0;
    GetMinWidth(&w);
    GetHeight(&h);
    if (x >= 0 && x < w && y >= 0 && y < h) {
        Int32 index = (y / mCellHeight) * GRID_WIDTH + (x / mCellWidth);
        if (index < GRID_SIZE) {
            AutoPtr<ArrayOf<Int32> > keys = (*mGridNeighbors)[index];
            *nearestKeys = keys;
            REFCOUNT_ADD(*nearestKeys);
            return NOERROR;
        }
    }
    *nearestKeys = ArrayOf<Int32>::Alloc(0);
    REFCOUNT_ADD(*nearestKeys);
    return NOERROR;
}

AutoPtr<Keyboard::Row> Keyboard::CreateRowFromXml(
    /* [in] */ IResources* res,
    /* [in] */ IXmlResourceParser* parser)
{
    AutoPtr<Row> row = new Row(res, this, parser);
    return row;
}

AutoPtr<Keyboard::Key> Keyboard::CreateKeyFromXml(
    /* [in] */ IResources* res,
    /* [in] */ Row* parent,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IXmlResourceParser* parser)
{
    AutoPtr<Key> key = new Key(res, parent, x, y, parser);
    return key;
}

void Keyboard::LoadKeyboard(
    /* [in] */ IContext* context,
    /* [in] */ IXmlResourceParser* parser)
{
    Boolean inKey = FALSE;
    Boolean inRow = FALSE;
    Boolean UNUSED(leftMostKey) = FALSE;
    Int32 row = 0;
    Int32 x = 0;
    Int32 y = 0;
    AutoPtr<Key> key;
    AutoPtr<Row> currentRow;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean skipRow = FALSE;

    // try{
    Int32 event = 0;
    while (IXmlPullParser::Probe(parser)->Next(&event), event != IXmlPullParser::END_DOCUMENT) {
        if (event == IXmlPullParser::START_TAG) {
            String tag;
            IXmlPullParser::Probe(parser)->GetName(&tag);
            if (TAG_ROW.Equals(tag)) {
                inRow = TRUE;
                x = 0;
                currentRow = CreateRowFromXml(res, parser);
                mRows->Add((IKeyboardRow*)currentRow);
                skipRow = currentRow->mMode != 0 && currentRow->mMode != mKeyboardMode;
                if (skipRow) {
                    SkipToEndOfRow(parser);
                    inRow = FALSE;
                }
           }
           else if (TAG_KEY.Equals(tag)) {
                inKey = TRUE;
                key = CreateKeyFromXml(res, currentRow, x, y, parser);
                mKeys->Add((IKeyboardKey*)key);
                ArrayOf<Int32>* codes = key->mCodes;
                if ((*codes)[0] == IKeyboard::KEYCODE_SHIFT) {
                    // Find available shift key slot and put this shift key in it
                    for (Int32 i = 0; i < mShiftKeys->GetLength(); i++) {
                        if ((*mShiftKeys)[i] == NULL) {
                            mShiftKeys->Set(i, key);
                            Int32 size;
                            (*mShiftKeyIndices)[i] = (mKeys->GetSize(&size), size) - 1;
                            break;
                        }
                    }
                    mModifierKeys->Add((IKeyboardKey*)key);
                }
                else if ((*codes)[0] == IKeyboard::KEYCODE_ALT) {
                    mModifierKeys->Add((IKeyboardKey*)key);
                }
                currentRow->mKeys->Add((IKeyboardKey*)key);
            }
            else if (TAG_KEYBOARD.Equals(tag)) {
                ParseKeyboardAttributes(res, parser);
            }
        }
        else if (event == IXmlPullParser::END_TAG) {
            if (inKey) {
                inKey = FALSE;
                x += key->mGap + key->mWidth;
                if (x > mTotalWidth) {
                    mTotalWidth = x;
                }
            }
            else if (inRow) {
                inRow = FALSE;
                y += currentRow->mVerticalGap;
                y += currentRow->mDefaultHeight;
                row++;
            }
            else {
                // TODO: error or extend?
            }
        }
    }
    // } catch (Exception e) {
    //     Log.e(TAG, "Parse error:" + e);
    //     e.printStackTrace();
    // }
    mTotalHeight = y - mDefaultVerticalGap;
}

void Keyboard::SkipToEndOfRow(
    /* [in] */ IXmlResourceParser* parser)
{
    Int32 event = 0;
    while (IXmlPullParser::Probe(parser)->Next(&event), event != IXmlPullParser::END_DOCUMENT) {
        String tag;
        IXmlPullParser::Probe(parser)->GetName(&tag);
        if (event == IXmlPullParser::END_TAG && TAG_ROW.Equals(tag)) {
            break;
        }
    }
}

void Keyboard::ParseKeyboardAttributes(
    /* [in] */ IResources* res,
    /* [in] */ IXmlResourceParser* parser)
{
    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::Keyboard);

    AutoPtr<ITypedArray> a;
    res->ObtainAttributes(Xml::AsAttributeSet(IXmlPullParser::Probe(parser)), layout, (ITypedArray**)&a);

    mDefaultWidth = GetDimensionOrFraction(a,
            R::styleable::Keyboard_keyWidth,
            mDisplayWidth, mDisplayWidth / 10);
    mDefaultHeight = GetDimensionOrFraction(a,
            R::styleable::Keyboard_keyHeight,
            mDisplayHeight, 50);
    mDefaultHorizontalGap = GetDimensionOrFraction(a,
            R::styleable::Keyboard_horizontalGap,
            mDisplayWidth, 0);
    mDefaultVerticalGap = GetDimensionOrFraction(a,
            R::styleable::Keyboard_verticalGap,
            mDisplayHeight, 0);
    mProximityThreshold = (Int32) (mDefaultWidth * SEARCH_DISTANCE);
    mProximityThreshold = mProximityThreshold * mProximityThreshold; // Square it for comparison
    a->Recycle();
}

Int32 Keyboard::GetDimensionOrFraction(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 index,
    /* [in] */ Int32 base,
    /* [in] */ Int32 defValue)
{
    AutoPtr<ITypedValue> value;
    a->PeekValue(index, (ITypedValue**)&value);
    if (value == NULL) return defValue;
    Int32 type = 0;
    value->GetType(&type);
    if (type == ITypedValue::TYPE_DIMENSION) {
        Int32 retValue = 0;
        a->GetDimensionPixelOffset(index, defValue, &retValue);
        return retValue;
    }
    else if (type == ITypedValue::TYPE_FRACTION) {
        Float tmpValue = 0.0;
        a->GetFraction(index, base, base, defValue, &tmpValue);
        // Round it to avoid values like 47.9999 from getting truncated
        return Elastos::Core::Math::Round(tmpValue);
    }
    return defValue;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
