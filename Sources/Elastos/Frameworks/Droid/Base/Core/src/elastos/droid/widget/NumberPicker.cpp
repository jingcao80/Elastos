
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/widget/NumberPicker.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/widget/CScroller.h"
#include "elastos/droid/widget/CNumberPicker.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::CCompatibilityInfo;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfo;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::Widget::CNumberPicker;
using Elastos::Droid::Widget::CScroller;
using Elastos::Core::CoreUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::Math;
using Elastos::Core::IAppendable;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CFormatter;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

const String NumberPicker::NUMBERPICKER_NAME("NumberPicker");

const Int32 NumberPicker::SELECTOR_WHEEL_ITEM_COUNT = 3;

const Int64 NumberPicker::DEFAULT_LONG_PRESS_UPDATE_INTERVAL = 300;

const Int32 NumberPicker::SELECTOR_MIDDLE_ITEM_INDEX = SELECTOR_WHEEL_ITEM_COUNT / 2;

const Int32 NumberPicker::SELECTOR_MAX_FLING_VELOCITY_ADJUSTMENT = 8;

const Int32 NumberPicker::SELECTOR_ADJUSTMENT_DURATION_MILLIS = 800;

const Int32 NumberPicker::SNAP_SCROLL_DURATION = 300;

const Float NumberPicker::TOP_AND_BOTTOM_FADING_EDGE_STRENGTH = 0.9f;

const Int32 NumberPicker::UNSCALED_DEFAULT_SELECTION_DIVIDER_HEIGHT = 2;

const Int32 NumberPicker::UNSCALED_DEFAULT_SELECTION_DIVIDERS_DISTANCE = 48;

const Int32 NumberPicker::DEFAULT_LAYOUT_RESOURCE_ID = R::layout::number_picker;

const Int32 NumberPicker::SIZE_UNSPECIFIED = -1;

AutoPtr<INumberPickerFormatter> NumberPicker::GetTwoDigitFormatter()
{
    AutoPtr<TwoDigitFormatter> formatter = new NumberPicker::TwoDigitFormatter();

    return (INumberPickerFormatter*)formatter.Get();
}

AutoPtr<INumberPickerFormatter> NumberPicker::sTwoDigitFormatter = NumberPicker::GetTwoDigitFormatter();

const Char32 NumberPicker::DIGIT_CHARACTERS[]= {
    // Latin digits are the common case
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    // Arabic-Indic
    0x0660/*'\u0660'*/, 0x0661/*'\u0661'*/, 0x0662/*'\u0662'*/, 0x0663/*'\u0663'*/,
    0x0664/*'\u0664'*/, 0x0665/*'\u0665'*/, 0x0666/*'\u0666'*/, 0x0667/*'\u0667'*/,
    0x0668/*'\u0668'*/, 0x0669/*'\u0669'*/,
    // Extended Arabic-Indic
    0x06f0/*'\u06f0'*/, 0x06f1/*'\u06f1'*/, 0x06f2/*'\u06f2'*/, 0x06f3/*'\u06f3'*/,
    0x06f4/*'\u06f4'*/, 0x06f5/*'\u06f5'*/, 0x06f6/*'\u06f6'*/, 0x06f7/*'\u06f7'*/,
    0x06f8/*'\u06f8'*/, 0x06f9/*'\u06f9'*/,
    // Hindi and Marathi (Devanagari script)
    0x0966/*'\u0966'*/, 0x0967/*'\u0967'*/, 0x0968/*'\u0968'*/, 0x0969/*'\u0969'*/,
    0x096a/*'\u096a'*/, 0x096b/*'\u096b'*/, 0x096c/*'\u096c'*/, 0x096d/*'\u096d'*/,
    0x096e/*'\u096e'*/, 0x096f/*'\u096f'*/,
    // Bengali
    0x09e6/*'\u09e6'*/, 0x09e7/*'\u09e7'*/, 0x09e8/*'\u09e8'*/, 0x09e9/*'\u09e9'*/,
    0x09ea/*'\u09ea'*/, 0x09eb/*'\u09eb'*/, 0x09ec/*'\u09ec'*/, 0x09ed/*'\u09ed'*/,
    0x09ee/*'\u09ee'*/, 0x09ef/*'\u09ef'*/,
    // Kannada
    0x0ce6/*'\u0ce6'*/, 0x0ce7/*'\u0ce7'*/, 0x0ce8/*'\u0ce8'*/, 0x0ce9/*'\u0ce9'*/,
    0x0cea/*'\u0cea'*/, 0x0ceb/*'\u0ceb'*/, 0x0cec/*'\u0cec'*/, 0x0ced/*'\u0ced'*/,
    0x0cee/*'\u0cee'*/, 0x0cef/*'\u0cef'*/
};

const Int32 NumberPicker::AccessibilityNodeProviderImpl::UNDEFINED = Elastos::Core::Math::INT32_MIN_VALUE;
const Int32 NumberPicker::AccessibilityNodeProviderImpl::VIRTUAL_VIEW_ID_INCREMENT;
const Int32 NumberPicker::AccessibilityNodeProviderImpl::VIRTUAL_VIEW_ID_INPUT;
const Int32 NumberPicker::AccessibilityNodeProviderImpl::VIRTUAL_VIEW_ID_DECREMENT;

//==============================================================================================
//          NumberPicker::CustomEditText
//==============================================================================================

CAR_INTERFACE_IMPL(NumberPicker::CustomEditText, EditText, INumberPickerCustomEditText);

NumberPicker::CustomEditText::CustomEditText()
{}

NumberPicker::CustomEditText::~CustomEditText()
{}

ECode NumberPicker::CustomEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return EditText::constructor(context, attrs);
}

ECode NumberPicker::CustomEditText::OnEditorAction(
    /* [in] */ Int32 actionCode)
{
    EditText::OnEditorAction(actionCode);
    if (actionCode == IEditorInfo::IME_ACTION_DONE) {
        View::ClearFocus();
    }
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::InputTextFilter
//==============================================================================================

NumberPicker::InputTextFilter::InputTextFilter(
    /* [in] */ NumberPicker* host)
    : mHost(host)
{
}

ECode NumberPicker::InputTextFilter::GetInputType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = IInputType::TYPE_CLASS_TEXT;
    return NOERROR;
}

AutoPtr<ArrayOf<Char32> > NumberPicker::InputTextFilter::GetAcceptedChars()
{
    UInt32 len = sizeof(DIGIT_CHARACTERS) / sizeof(DIGIT_CHARACTERS[0]);
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(len);
    for (Int32 i = 0; i < (Int32)len; i++) {
        (*chars)[i] = DIGIT_CHARACTERS[i];
    }
    return chars;
}

ECode NumberPicker::InputTextFilter::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** sou)
{
    if (!mHost->mDisplayedValues) {
        AutoPtr<ICharSequence> filtered;
        NumberKeyListener::Filter(source, start, end, dest, dstart, dend, (ICharSequence**)&filtered);
        if (!filtered) {
            source->SubSequence(start, end, (ICharSequence**)&filtered);
        }

        AutoPtr<ICharSequence> res;
        ICharSequence::Probe(dest)->SubSequence(0, dstart, (ICharSequence**)&res);
        String subString = String(NULL);
        res->ToString(&subString);
        String result = subString;
        filtered->ToString(&subString);
        result += subString;
        Int32 len = 0;
        ICharSequence::Probe(dest)->GetLength(&len);
        res = NULL;
        ICharSequence::Probe(dest)->SubSequence(dend, len, (ICharSequence**)&res);
        res->ToString(&subString);
        result += subString;

        if (result.Equals("")) {
            AutoPtr<ICharSequence> seq = CoreUtils::Convert(result);
            *sou = seq;
            REFCOUNT_ADD(*sou);
            return NOERROR;
        }

        Int32 val = mHost->GetSelectedPos(result);

        /*
         * Ensure the user can't type in a value greater than the max
         * allowed. We have to allow less than min as the user might
         * want to delete some numbers and then type a new number.
         * And prevent multiple-"0" that exceeds the length of upper
         * bound number.
         */
        if (val > mHost->mMaxValue ||
                result.GetLength() > StringUtils::ToString(mHost->mMaxValue).GetLength()) {
            AutoPtr<ICharSequence> seq = CoreUtils::Convert("");
            *sou = seq;
            REFCOUNT_ADD(*sou);
            return NOERROR;
        }
        else {
            *sou = filtered;
            REFCOUNT_ADD(*sou);
            return NOERROR;
        }
    }
    else {
        AutoPtr<ICharSequence> filtered;
        source->SubSequence(start, end, (ICharSequence**)&filtered);
        if (TextUtils::IsEmpty(filtered)) {
            AutoPtr<ICharSequence> seq = CoreUtils::Convert("");
            *sou = seq;
            REFCOUNT_ADD(*sou);
            return NOERROR;
        }
        AutoPtr<ICharSequence> res;
        ICharSequence::Probe(dest)->SubSequence(0, dstart, (ICharSequence**)&res);
        String subString = String(NULL);
        res->ToString(&subString);
        String result = subString;
        filtered->ToString(&subString);
        result += subString;
        Int32 len = 0;
        ICharSequence::Probe(dest)->GetLength(&len);
        res = NULL;
        ICharSequence::Probe(dest)->SubSequence(dend, len, (ICharSequence**)&res);
        res->ToString(&subString);
        result += subString;

        String str = result.ToLowerCase();
        for (Int32 i = 0; i < mHost->mDisplayedValues->GetLength(); i++) {
            String val = (*mHost->mDisplayedValues)[i];
            String valLowerCase = val.ToLowerCase();
            if (valLowerCase.StartWith(str)) {
                mHost->PostSetSelectionCommand(result.GetLength(), val.GetLength());
                return CoreUtils::Convert(val)->SubSequence(dstart, val.GetLength(), sou);
            }
        }
        AutoPtr<ICharSequence> seq = CoreUtils::Convert("");
        *sou = seq;
        REFCOUNT_ADD(*sou);
        return NOERROR;
    }
}

ECode NumberPicker::InputTextFilter::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return MetaKeyKeyListener::ClearMetaKeyState(view, content, states);
}

ECode NumberPicker::InputTextFilter::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);
    return OnKeyUp(view, content, keyCode, event, ret);
}

//==============================================================================================
//          NumberPicker::PressedStateHelper
//==============================================================================================

NumberPicker::PressedStateHelper::PressedStateHelper(
    /* [in] */ NumberPicker* host)
    : mManagedButton(0)
    , mMode(0)
    , mHost(host)
{
}

void NumberPicker::PressedStateHelper::Cancel()
{
    mMode = 0;
    mManagedButton = 0;
    Boolean res;
    mHost->RemoveCallbacks(this, &res);
    if (mHost->mIncrementVirtualButtonPressed) {
        mHost->mIncrementVirtualButtonPressed = FALSE;
        mHost->Invalidate(0, mHost->mBottomSelectionDividerBottom, mHost->mRight, mHost->mBottom);
    }
    mHost->mDecrementVirtualButtonPressed = FALSE;
    if (mHost->mDecrementVirtualButtonPressed) {
        mHost->Invalidate(0, 0, mHost->mRight, mHost->mTopSelectionDividerTop);
    }
}

void NumberPicker::PressedStateHelper::ButtonPressDelayed(
    /* [in] */ Int32 button)
{
    Cancel();
    mMode = MODE_PRESS;
    mManagedButton = button;
    Boolean res;
    mHost->PostDelayed(this, CViewConfiguration::GetTapTimeout(), &res);
}

void NumberPicker::PressedStateHelper::ButtonTapped(
    /* [in] */ Int32 button)
{
    Cancel();
    mMode = MODE_TAPPED;
    mManagedButton = button;
    Boolean res;
    mHost->Post(this, &res);
}

ECode NumberPicker::PressedStateHelper::Run()
{
    switch (mMode) {
        case MODE_PRESS:
            switch (mManagedButton) {
                case BUTTON_INCREMENT:
                    mHost->mIncrementVirtualButtonPressed = TRUE;
                    mHost->Invalidate(0, mHost->mBottomSelectionDividerBottom, mHost->mRight, mHost->mBottom);
                    break;
                case BUTTON_DECREMENT:
                    mHost->mDecrementVirtualButtonPressed = TRUE;
                    mHost->Invalidate(0, 0, mHost->mRight, mHost->mTopSelectionDividerTop);
            }
            break;
        case MODE_TAPPED:
            switch (mManagedButton) {
                Boolean res;
                case BUTTON_INCREMENT:
                    if (!mHost->mIncrementVirtualButtonPressed) {
                        mHost->PostDelayed(this, CViewConfiguration::GetPressedStateDuration(), &res);
                    }
                    mHost->mIncrementVirtualButtonPressed ^= TRUE;
                    mHost->Invalidate(0, mHost->mBottomSelectionDividerBottom, mHost->mRight, mHost->mBottom);
                    break;
                case BUTTON_DECREMENT:
                    if (!mHost->mDecrementVirtualButtonPressed) {
                        mHost->PostDelayed(this, CViewConfiguration::GetPressedStateDuration(), &res);
                    }
                    mHost->mDecrementVirtualButtonPressed ^= TRUE;
                    mHost->Invalidate(0, 0, mHost->mRight, mHost->mTopSelectionDividerTop);
                    break;
            }
    }
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::SetSelectionCommand
//==============================================================================================

NumberPicker::SetSelectionCommand::SetSelectionCommand(
    /* [in] */ INumberPicker* host)
    : mSelectionStart(0)
    , mSelectionEnd(0)
{
    IWeakReferenceSource::Probe(host)->GetWeakReference((IWeakReference**)&mWeakHost);
    assert(mWeakHost);
}

ECode NumberPicker::SetSelectionCommand::Run()
{
    AutoPtr<IInterface> obj;
    mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<INumberPicker> num = INumberPicker::Probe(obj);
    if (num == NULL) {
        return NOERROR;
    }
    NumberPicker* host = (NumberPicker*)(CNumberPicker*)num.Get();
    host->mInputText->SetSelection(mSelectionStart, mSelectionEnd);
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::ChangeCurrentByOneFromLongPressCommand
//==============================================================================================

NumberPicker::ChangeCurrentByOneFromLongPressCommand::ChangeCurrentByOneFromLongPressCommand(
    /* [in] */ NumberPicker* host)
    : mIncrement(FALSE)
    , mHost(host)
{
}

void NumberPicker::ChangeCurrentByOneFromLongPressCommand::SetStep(
    /* [in] */ Boolean increment)
{
    mIncrement = increment;
}

ECode NumberPicker::ChangeCurrentByOneFromLongPressCommand::Run()
{
    mHost->ChangeValueByOne(mIncrement);
    Boolean res;
    mHost->PostDelayed(this, mHost->mLongPressUpdateInterval, &res);
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::BeginSoftInputOnLongPressCommand
//==============================================================================================

NumberPicker::BeginSoftInputOnLongPressCommand::BeginSoftInputOnLongPressCommand(
    /* [in] */ NumberPicker* host)
    : mHost(host)
{
}

ECode NumberPicker::BeginSoftInputOnLongPressCommand::Run()
{
    Boolean result;
    mHost->PerformLongClick(&result);
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::AccessibilityNodeProviderImpl
//==============================================================================================

NumberPicker::AccessibilityNodeProviderImpl::AccessibilityNodeProviderImpl(
    /* [in] */ NumberPicker* host)
    : mAccessibilityFocusedView(UNDEFINED)
    , mHost(host)
{
    CRect::New((IRect**)&mTempRect);
    mTempArray = ArrayOf<Int32>::Alloc(2);
}

ECode NumberPicker::AccessibilityNodeProviderImpl::CreateAccessibilityNodeInfo(
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    switch (virtualViewId) {
        case IView::NO_ID: {
            AutoPtr<IAccessibilityNodeInfo> nodeInfo = CreateAccessibilityNodeInfoForNumberPicker(mHost->mScrollX, mHost->mScrollY,
                mHost->mScrollX + (mHost->mRight - mHost->mLeft), mHost->mScrollY + (mHost->mBottom - mHost->mTop));
            *info = nodeInfo;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
        case VIRTUAL_VIEW_ID_DECREMENT: {
            AutoPtr<IAccessibilityNodeInfo> nodeInfo = CreateAccessibilityNodeInfoForVirtualButton(VIRTUAL_VIEW_ID_DECREMENT, GetVirtualDecrementButtonText(),
                mHost->mScrollX, mHost->mScrollY, mHost->mScrollX + (mHost->mRight - mHost->mLeft),
                mHost->mTopSelectionDividerTop + mHost->mSelectionDividerHeight);
            *info = nodeInfo;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
        case VIRTUAL_VIEW_ID_INPUT: {
            AutoPtr<IAccessibilityNodeInfo> nodeInfo = CreateAccessibiltyNodeInfoForInputText(mHost->mScrollX,
                    mHost->mTopSelectionDividerTop + mHost->mSelectionDividerHeight,
                    mHost->mScrollX + (mHost->mRight - mHost->mLeft),
                    mHost->mBottomSelectionDividerBottom - mHost->mSelectionDividerHeight);
            *info = nodeInfo;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
        case VIRTUAL_VIEW_ID_INCREMENT: {
            AutoPtr<IAccessibilityNodeInfo> nodeInfo = CreateAccessibilityNodeInfoForVirtualButton(VIRTUAL_VIEW_ID_INCREMENT, GetVirtualIncrementButtonText(),
                mHost->mScrollX, mHost->mBottomSelectionDividerBottom - mHost->mSelectionDividerHeight,
                mHost->mScrollX + (mHost->mRight - mHost->mLeft), mHost->mScrollY + (mHost->mBottom - mHost->mTop));
            *info = nodeInfo;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
    }
    AccessibilityNodeProvider::CreateAccessibilityNodeInfo(virtualViewId, info);
    return NOERROR;
}

ECode NumberPicker::AccessibilityNodeProviderImpl::FindAccessibilityNodeInfosByText(
    /* [in] */ const String& searched,
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    if (TextUtils::IsEmpty(searched)) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->GetEmptyList(list);
    }

    String searchedLowerCase = searched.ToLowerCase();
    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    switch (virtualViewId) {
        case IView::NO_ID:
            FindAccessibilityNodeInfosByTextInChild(searchedLowerCase, VIRTUAL_VIEW_ID_DECREMENT, result);
            FindAccessibilityNodeInfosByTextInChild(searchedLowerCase, VIRTUAL_VIEW_ID_INPUT, result);
            FindAccessibilityNodeInfosByTextInChild(searchedLowerCase, VIRTUAL_VIEW_ID_INCREMENT, result);
            *list = result;
            REFCOUNT_ADD(*list);
            return NOERROR;
        case VIRTUAL_VIEW_ID_DECREMENT:
        case VIRTUAL_VIEW_ID_INCREMENT:
        case VIRTUAL_VIEW_ID_INPUT:
            FindAccessibilityNodeInfosByTextInChild(searchedLowerCase, virtualViewId, result);
            *list = result;
            REFCOUNT_ADD(*list);
            return NOERROR;
    }
    AccessibilityNodeProvider::FindAccessibilityNodeInfosByText(searched, virtualViewId, list);
    return NOERROR;
}

ECode NumberPicker::AccessibilityNodeProviderImpl::PerformAction(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    switch(virtualViewId) {
        case IView::NO_ID:
            switch (action) {
                case IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS: {
                    if (mAccessibilityFocusedView != virtualViewId) {
                        mAccessibilityFocusedView = virtualViewId;
                        Boolean result;
                        mHost->RequestAccessibilityFocus(&result);
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
                case IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS: {
                    if (mAccessibilityFocusedView == virtualViewId) {
                        mAccessibilityFocusedView = UNDEFINED;
                        mHost->ClearAccessibilityFocus();
                        *res = TRUE;
                        return NOERROR;
                    }
                    *res = FALSE;
                    return NOERROR;
                }
                case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
                    Boolean result;
                    mHost->IsEnabled(&result);
                    Boolean result2;
                    mHost->GetWrapSelectorWheel(&result2);
                    Int32 value;
                    mHost->GetValue(&value);
                    Int32 minValue;
                    mHost->GetMinValue(&minValue);
                    if (result && (result2 || value < minValue)) {
                        mHost->ChangeValueByOne(TRUE);
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
                case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
                    Boolean result;
                    mHost->IsEnabled(&result);
                    Boolean result2;
                    mHost->GetWrapSelectorWheel(&result2);
                    Int32 value;
                    mHost->GetValue(&value);
                    Int32 minValue;
                    mHost->GetMinValue(&minValue);
                    if (result && (result2 || value > minValue)) {
                        mHost->ChangeValueByOne(TRUE);
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
            }
            break;
        case VIRTUAL_VIEW_ID_INPUT:
            switch (action) {
                case IAccessibilityNodeInfo::ACTION_FOCUS: {
                    Boolean isFocused = FALSE;
                    IView::Probe(mHost->mInputText)->IsFocused(&isFocused);
                    Boolean result;
                    mHost->IsEnabled(&result);
                    if (result && !isFocused) {
                        return IView::Probe(mHost->mInputText)->RequestFocus(res);
                    }
                }
                    break;
                case IAccessibilityNodeInfo::ACTION_CLEAR_FOCUS: {
                    Boolean isFocused = FALSE;
                    IView::Probe(mHost->mInputText)->IsFocused(&isFocused);
                    Boolean result;
                    mHost->IsEnabled(&result);
                    if (result && isFocused) {
                        IView::Probe(mHost->mInputText)->ClearFocus();
                        *res = TRUE;
                        return NOERROR;
                    }
                    *res = FALSE;
                    return NOERROR;
                }
                case IAccessibilityNodeInfo::ACTION_CLICK: {
                    Boolean result;
                    if (mHost->IsEnabled(&result), result) {
                        mHost->PerformClick(&result);
                        *res = TRUE;
                        return NOERROR;
                    }
                    *res = FALSE;
                    return NOERROR;
                }
                case IAccessibilityNodeInfo::ACTION_LONG_CLICK: {
                    Boolean result;
                    if (mHost->IsEnabled(&result), result) {
                        mHost->PerformLongClick(&result);
                        *res = TRUE;
                        return NOERROR;
                    }
                    *res = FALSE;
                    return NOERROR;
                }
                case IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS: {
                    if (mAccessibilityFocusedView != virtualViewId) {
                        mAccessibilityFocusedView = virtualViewId;
                        SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
                        IView::Probe(mHost->mInputText)->Invalidate();
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
                case IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS: {
                    if (mAccessibilityFocusedView == virtualViewId) {
                        mAccessibilityFocusedView = UNDEFINED;
                        SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED);
                        IView::Probe(mHost->mInputText)->Invalidate();
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
                default:
                    IView::Probe(mHost->mInputText)->PerformAccessibilityAction(action, arguments, res);
                    return NOERROR;
            }
                *res = FALSE;
                return NOERROR;
        case VIRTUAL_VIEW_ID_INCREMENT:
            switch (action) {
                case IAccessibilityNodeInfo::ACTION_CLICK: {
                    Boolean result;
                    if (mHost->IsEnabled(&result), result) {
                        mHost->ChangeValueByOne(TRUE);
                        SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_CLICKED);
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
                case IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS: {
                    if (mAccessibilityFocusedView != virtualViewId) {
                        mAccessibilityFocusedView = virtualViewId;
                        SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
                        mHost->Invalidate(0, mHost->mBottomSelectionDividerBottom, mHost->mRight, mHost->mBottom);
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
                case IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS: {
                    if (mAccessibilityFocusedView == virtualViewId) {
                        mAccessibilityFocusedView = UNDEFINED;
                        SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED);
                        mHost->Invalidate(0, mHost->mBottomSelectionDividerBottom, mHost->mRight, mHost->mBottom);
                        *res = TRUE;
                        return NOERROR;
                    }
                }
                    *res = FALSE;
                    return NOERROR;
            }
                *res = FALSE;
                return NOERROR;

            case VIRTUAL_VIEW_ID_DECREMENT:
                switch (action) {
                    case IAccessibilityNodeInfo::ACTION_CLICK: {
                        Boolean result;
                        if (mHost->IsEnabled(&result), result) {
                            Boolean increment = (virtualViewId == VIRTUAL_VIEW_ID_INCREMENT);
                            mHost->ChangeValueByOne(increment);
                            SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_CLICKED);
                            *res = TRUE;
                            return NOERROR;
                        }
                    }
                        *res = FALSE;
                        return NOERROR;
                    case IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS: {
                        if (mAccessibilityFocusedView != virtualViewId) {
                            mAccessibilityFocusedView = virtualViewId;
                            SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED);
                            mHost->Invalidate(0, 0, mHost->mRight, mHost->mTopSelectionDividerTop);
                            *res = TRUE;
                            return NOERROR;
                        }
                    }
                        *res = FALSE;
                        return NOERROR;
                    case IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS: {
                        if (mAccessibilityFocusedView == virtualViewId) {
                            mAccessibilityFocusedView = UNDEFINED;
                            SendAccessibilityEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED);
                            mHost->Invalidate(0, 0, mHost->mRight, mHost->mTopSelectionDividerTop);
                            *res = TRUE;
                            return NOERROR;
                        }
                    }
                        *res = FALSE;
                        return NOERROR;
                }
                    *res = FALSE;
                    return NOERROR;
    }
    AccessibilityNodeProvider::PerformAction(virtualViewId, action, arguments, res);
    return NOERROR;
}

void NumberPicker::AccessibilityNodeProviderImpl::SendAccessibilityEventForVirtualView(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 eventType)
{
    switch (virtualViewId) {
        case VIRTUAL_VIEW_ID_DECREMENT: {
            if (HasVirtualDecrementButton()) {
                SendAccessibilityEventForVirtualButton(virtualViewId, eventType, GetVirtualDecrementButtonText());
            }
            break;
        }

        case VIRTUAL_VIEW_ID_INPUT: {
            SendAccessibilityEventForVirtualText(eventType);
            break;
        }

        case VIRTUAL_VIEW_ID_INCREMENT: {
            if (HasVirtualIncrementButton()) {
                SendAccessibilityEventForVirtualButton(virtualViewId, eventType, GetVirtualIncrementButtonText());
            }
            break;
        }
    }
}

void NumberPicker::AccessibilityNodeProviderImpl::SendAccessibilityEventForVirtualText(
    /* [in] */ Int32 eventType)
{
    AutoPtr<IAccessibilityManager> am;
    CAccessibilityManager::GetInstance(mHost->mContext, (IAccessibilityManager**)&am);
    Boolean isEnabled = FALSE;
    am->IsEnabled(&isEnabled);
    if (isEnabled) {
        AutoPtr<IAccessibilityEvent> event;
        CAccessibilityEvent::Obtain(eventType, (IAccessibilityEvent**)&event);
        IView::Probe(mHost->mInputText)->OnInitializeAccessibilityEvent(event);
        IView::Probe(mHost->mInputText)->OnPopulateAccessibilityEvent(event);
        IAccessibilityRecord::Probe(event)->SetSource((IView*)mHost, VIRTUAL_VIEW_ID_INPUT);
        Boolean res;
        mHost->RequestSendAccessibilityEvent((IView*)mHost, event, &res);
    }
}

void NumberPicker::AccessibilityNodeProviderImpl::SendAccessibilityEventForVirtualButton(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 eventType,
    /* [in] */ const String& text)
{
    AutoPtr<IAccessibilityManager> am;
    CAccessibilityManager::GetInstance(mHost->mContext, (IAccessibilityManager**)&am);
    Boolean isEnabled = FALSE;
    am->IsEnabled(&isEnabled);
    if (isEnabled) {
        AutoPtr<IAccessibilityEvent> event;
        CAccessibilityEvent::Obtain(eventType, (IAccessibilityEvent**)&event);

        AutoPtr<IAccessibilityRecord> eventProbe = IAccessibilityRecord::Probe(event);
        eventProbe->SetClassName(CoreUtils::Convert("Button"));
        String packageName;
        mHost->mContext->GetPackageName(&packageName);
        event->SetPackageName(CoreUtils::Convert(packageName));
        AutoPtr<IList> list;
        eventProbe->GetText((IList**)&list);
        list->Add(CoreUtils::Convert(text));
        Boolean result;
        mHost->IsEnabled(&result);
        eventProbe->SetEnabled(result);
        eventProbe->SetSource((IView*)mHost, virtualViewId);
        Boolean res;
        mHost->RequestSendAccessibilityEvent((IView*)mHost, event, &res);
    }
}

void NumberPicker::AccessibilityNodeProviderImpl::FindAccessibilityNodeInfosByTextInChild(
    /* [in] */ const String& searchedLowerCase,
    /* [in] */ Int32 virtualViewId,
    /* [in] */ IList* outResult)
{
    switch (virtualViewId) {
        case VIRTUAL_VIEW_ID_DECREMENT: {
            String text = GetVirtualDecrementButtonText();
            text = text.ToLowerCase();
            if (text.IsEmpty() && text.Contains(searchedLowerCase)) {
                AutoPtr<IAccessibilityNodeInfo> info;
                CreateAccessibilityNodeInfo(VIRTUAL_VIEW_ID_DECREMENT, (IAccessibilityNodeInfo**)&info);
                outResult->Add(info);
            }
            return;
        }
        case VIRTUAL_VIEW_ID_INPUT: {
            AutoPtr<ICharSequence> text;
            ITextView::Probe(mHost->mInputText)->GetText((ICharSequence**)&text);
            String textStr = String(NULL);
            text->ToString(&textStr);
            textStr = textStr.ToLowerCase();
            AutoPtr<IAccessibilityNodeInfo> info;
            CreateAccessibilityNodeInfo(VIRTUAL_VIEW_ID_INPUT, (IAccessibilityNodeInfo**)&info);
            if (!TextUtils::IsEmpty(text) && textStr.Contains(searchedLowerCase)) {
                outResult->Add(info);
            }

            AutoPtr<ICharSequence> contentDesc;
            ITextView::Probe(mHost->mInputText)->GetText((ICharSequence**)&contentDesc);
            String contStr = String(NULL);
            text->ToString(&contStr);
            contStr = contStr.ToLowerCase();
            if (!TextUtils::IsEmpty(text) && contStr.Contains(searchedLowerCase)) {
                outResult->Add(info);
            }
            break;
        }
        case VIRTUAL_VIEW_ID_INCREMENT: {
            String text = GetVirtualIncrementButtonText();
            text = text.ToLowerCase();
            if (!text.IsEmpty() && text.Contains(searchedLowerCase)) {
                AutoPtr<IAccessibilityNodeInfo> info;
                CreateAccessibilityNodeInfo(VIRTUAL_VIEW_ID_INCREMENT, (IAccessibilityNodeInfo**)&info);
                outResult->Add(info);
            }
            return;
         }
    }
}

AutoPtr<IAccessibilityNodeInfo> NumberPicker::AccessibilityNodeProviderImpl::CreateAccessibiltyNodeInfoForInputText(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IAccessibilityNodeInfo> info;
    IView::Probe(mHost->mInputText)->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&info);
    info->SetSource((IView*)mHost, VIRTUAL_VIEW_ID_INPUT);
    if (mAccessibilityFocusedView != VIRTUAL_VIEW_ID_INPUT) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS);
    }
    if (mAccessibilityFocusedView == VIRTUAL_VIEW_ID_INPUT) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS);
    }

    AutoPtr<IRect> boundsInParent = mTempRect;
    boundsInParent->Set(left, top, right, bottom);
    info->SetVisibleToUser(mHost->IsVisibleToUser(boundsInParent));
    info->SetBoundsInParent(boundsInParent);
    AutoPtr<IRect> boundsInScreen = boundsInParent;
    AutoPtr<ArrayOf<Int32> > locationOnScreen = mTempArray;
    mHost->GetLocationOnScreen(locationOnScreen);
    boundsInScreen->Offset((*locationOnScreen)[0], (*locationOnScreen)[1]);
    info->SetBoundsInScreen(boundsInScreen);

    return info;
}

AutoPtr<IAccessibilityNodeInfo> NumberPicker::AccessibilityNodeProviderImpl::CreateAccessibilityNodeInfoForVirtualButton(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ const String& text,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IAccessibilityNodeInfo> info;
    CAccessibilityNodeInfo::Obtain((IAccessibilityNodeInfo**)&info);
    info->SetClassName(CoreUtils::Convert("Button"));
    String packageName;
    mHost->mContext->GetPackageName(&packageName);
    info->SetPackageName(CoreUtils::Convert(packageName));
    info->SetSource((IView*)mHost, virtualViewId);
    info->SetParent((IView*)mHost);
    info->SetText(CoreUtils::Convert(text));
    info->SetClickable(TRUE);
    info->SetLongClickable(TRUE);
    Boolean result;
    mHost->IsEnabled(&result);
    info->SetEnabled(result);

    AutoPtr<IRect> boundsInParent = mTempRect;
    boundsInParent->Set(mHost->mLeft, mHost->mTop, mHost->mRight, mHost->mBottom);
    info->SetVisibleToUser(mHost->IsVisibleToUser(boundsInParent));
    info->SetBoundsInParent(boundsInParent);

    AutoPtr<IRect> boundsInScreen = boundsInParent;
    AutoPtr<ArrayOf<Int32> > locationOnScreen = mTempArray;

    mHost->GetLocationOnScreen(locationOnScreen);
    boundsInScreen->Offset((*locationOnScreen)[0], (*locationOnScreen)[1]);
    info->SetBoundsInScreen(boundsInScreen);

    if (mAccessibilityFocusedView != virtualViewId) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS);
    }
    if (mAccessibilityFocusedView == virtualViewId) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS);
    }
    if (mHost->IsEnabled(&result), result) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_CLICK);
    }

    return info;
}

AutoPtr<IAccessibilityNodeInfo> NumberPicker::AccessibilityNodeProviderImpl::CreateAccessibilityNodeInfoForNumberPicker(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IAccessibilityNodeInfo> info;
    CAccessibilityNodeInfo::Obtain((IAccessibilityNodeInfo**)&info);
    info->SetClassName(CoreUtils::Convert(NUMBERPICKER_NAME));

    String packageName;
    mHost->mContext->GetPackageName(&packageName);
    info->SetPackageName(CoreUtils::Convert(packageName));
    info->SetSource((IView*)mHost);

    if (HasVirtualDecrementButton()) {
        info->AddChild((IView*)mHost, VIRTUAL_VIEW_ID_DECREMENT);
    }
    info->AddChild((IView*)mHost, VIRTUAL_VIEW_ID_INPUT);
    if (HasVirtualIncrementButton()) {
        info->AddChild((IView*)mHost, VIRTUAL_VIEW_ID_INCREMENT);
    }

    AutoPtr<IViewParent> parent;
    mHost->GetParentForAccessibility((IViewParent**)&parent);
    info->SetParent(IView::Probe(parent));
    Boolean result;
    mHost->IsEnabled(&result);
    info->SetEnabled(result);
    info->SetScrollable(TRUE);

    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ICompatibilityInfo> cominfo;
    resources->GetCompatibilityInfo((ICompatibilityInfo**)&cominfo);
    AutoPtr<CCompatibilityInfo> cInof = (CCompatibilityInfo*)cominfo.Get();
    Float applicationScale = cInof->mApplicationScale;
    AutoPtr<IRect> boundsInParent = mTempRect;
    boundsInParent->Set(mHost->mLeft, mHost->mTop, mHost->mRight, mHost->mBottom);
    boundsInParent->Scale(applicationScale);
    info->SetBoundsInParent(boundsInParent);
    info->SetVisibleToUser(mHost->IsVisibleToUser());

    AutoPtr<IRect> boundsInScreen = boundsInParent;
    AutoPtr<ArrayOf<Int32> > locationOnScreen = mTempArray;
    mHost->GetLocationOnScreen(locationOnScreen);
    boundsInScreen->Offset((*locationOnScreen)[0], (*locationOnScreen)[1]);
    boundsInScreen->Scale(applicationScale);
    info->SetBoundsInScreen(boundsInScreen);

    if (mAccessibilityFocusedView != IView::NO_ID) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS);
    }
    if (mAccessibilityFocusedView == IView::NO_ID) {
        info->AddAction(IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS);
    }
    if (mHost->IsEnabled(&result), result) {
        Boolean res;
        mHost->GetWrapSelectorWheel(&res);
        Int32 value;
        mHost->GetValue(&value);
        Int32 minValue;
        mHost->GetMinValue(&minValue);
        if (res || value < minValue) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
        }
        if (res || value > minValue) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
        }
    }
    return info;
}

Boolean NumberPicker::AccessibilityNodeProviderImpl::HasVirtualDecrementButton()
{
    Boolean res;
    mHost->GetWrapSelectorWheel(&res);
    Int32 value;
    mHost->GetValue(&value);
    Int32 minValue;
    mHost->GetMinValue(&minValue);
    return res || value > minValue;
}

Boolean NumberPicker::AccessibilityNodeProviderImpl::HasVirtualIncrementButton()
{
    Boolean res;
    mHost->GetWrapSelectorWheel(&res);
    Int32 value;
    mHost->GetValue(&value);
    Int32 maxValue;
    mHost->GetMaxValue(&maxValue);
    return res || value < maxValue;
}

String NumberPicker::AccessibilityNodeProviderImpl::GetVirtualDecrementButtonText()
{
    Int32 value = mHost->mValue - 1;
    if (mHost->mWrapSelectorWheel) {
        value = mHost->GetWrappedSelectorIndex(value);
    }
    if (value >= mHost->mMinValue) {
        return (mHost->mDisplayedValues == NULL) ? mHost->FormatNumber(value)
            : (*mHost->mDisplayedValues)[value - mHost->mMinValue];
    }
    return String(NULL);
}

String NumberPicker::AccessibilityNodeProviderImpl::GetVirtualIncrementButtonText()
{
    Int32 value = mHost->mValue + 1;
    if (mHost->mWrapSelectorWheel) {
        value = mHost->GetWrappedSelectorIndex(value);
    }
    if (value <= mHost->mMaxValue) {
        return (mHost->mDisplayedValues == NULL) ? mHost->FormatNumber(value)
            : (*mHost->mDisplayedValues)[value - mHost->mMinValue];
    }
    return String(NULL);
}

String NumberPicker::AccessibilityNodeProviderImpl::FormatNumberWithLocale(
    /* [in] */ Int32 value)
{
    String result("");
    result.AppendFormat("%d", value);
    return result;
}

//==============================================================================================
//          NumberPicker::TwoDigitFormatter
//==============================================================================================

CAR_INTERFACE_IMPL(NumberPicker::TwoDigitFormatter, Object, INumberPickerFormatter);

NumberPicker::TwoDigitFormatter::TwoDigitFormatter()
{
    mBuilder = new StringBuilder();
    mArgs = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    Init(locale);
}

ECode NumberPicker::TwoDigitFormatter::Init(
    /* [in] */ ILocale* locale)
{
    mFmt = CreateFormatter(locale);
    mZeroDigit = GetZeroDigit(locale);
    return NOERROR;
}

ECode NumberPicker::TwoDigitFormatter::Format(
    /* [in] */ Int32 value,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> currentLocale;
    helper->GetDefault((ILocale**)&currentLocale);
    if (mZeroDigit != GetZeroDigit(currentLocale)) {
        Init(currentLocale);
    }

    AutoPtr<IInteger32> integer;
    CInteger32::New(value, (IInteger32**)&integer);

    mArgs->Set(0, integer);
    Int32 length = 0;
    mBuilder->GetLength(&length);
    mBuilder->Delete(0, length);

    mFmt->Format(String("%02d"), mArgs);
    mFmt->ToString(str);
    return NOERROR;
}

Char32 NumberPicker::TwoDigitFormatter::GetZeroDigit(
    /* [in] */ ILocale* locale)
{
    AutoPtr<ILocaleDataHelper> helper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
    AutoPtr<ILocaleData> data;
    helper->Get(locale, (ILocaleData**)&data);
    Char32 zerodigit = 0;
    data->GetZeroDigit(&zerodigit);
    return zerodigit;
}

AutoPtr<IFormatter> NumberPicker::TwoDigitFormatter::CreateFormatter(
    /* [in] */ ILocale* locale)
{
    AutoPtr<IFormatter> formatter;
    CFormatter::New((IAppendable*)mBuilder.Get(), locale, (IFormatter**)&formatter);
    return formatter;
}

//==============================================================================================
//          NumberPicker::NumberPickerOnClickListener
//==============================================================================================

CAR_INTERFACE_IMPL(NumberPicker::NumberPickerOnClickListener, Object, IViewOnClickListener);

NumberPicker::NumberPickerOnClickListener::NumberPickerOnClickListener(
    /* [in] */ NumberPicker* host)
    : mHost(host)
{
}

ECode NumberPicker::NumberPickerOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->HideSoftInput();
    IView::Probe(mHost->mInputText)->ClearFocus();
    Int32 id = 0;
    v->GetId(&id);
    if (id == R::id::increment) {
        mHost->ChangeValueByOne(TRUE);
    }
    else {
        mHost->ChangeValueByOne(FALSE);
    }
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::NumberPickerOnLongCliskListener
//==============================================================================================

CAR_INTERFACE_IMPL(NumberPicker::NumberPickerOnLongCliskListener, Object, IViewOnLongClickListener);

NumberPicker::NumberPickerOnLongCliskListener::NumberPickerOnLongCliskListener(
    /* [in] */ NumberPicker* host)
    : mHost(host)
{
}

ECode NumberPicker::NumberPickerOnLongCliskListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->HideSoftInput();
    IView::Probe(mHost->mInputText)->ClearFocus();
    Int32 id = 0;
    v->GetId(&id);
    if (id == R::id::increment) {
        mHost->PostChangeCurrentByOneFromLongPress(TRUE, 0);
    }
    else {
        mHost->PostChangeCurrentByOneFromLongPress(FALSE, 0);
    }
    *result = FALSE;
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::NumberPickerOnFocusChangeListener
//==============================================================================================

CAR_INTERFACE_IMPL(NumberPicker::NumberPickerOnFocusChangeListener, Object, IViewOnFocusChangeListener);

NumberPicker::NumberPickerOnFocusChangeListener::NumberPickerOnFocusChangeListener(
    /* [in] */ NumberPicker* host)
    : mHost(host)
{
}

ECode NumberPicker::NumberPickerOnFocusChangeListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (hasFocus) {
        mHost->mInputText->SelectAll();
    }
    else {
        mHost->mInputText->SetSelection(0, 0);
        mHost->ValidateInputTextView(v);
    }
    return NOERROR;
}

//==============================================================================================
//          NumberPicker::InputTextFilter
//==============================================================================================

CAR_INTERFACE_IMPL(NumberPicker, LinearLayout, INumberPicker);

NumberPicker::NumberPicker()
    : mSelectionDividersDistance(0)
    , mMinHeight(0)
    , mMaxHeight(0)
    , mMinWidth(0)
    , mMaxWidth(0)
    , mComputeMaxWidth(FALSE)
    , mTextSize(0)
    , mSelectorTextGapHeight(0)
    , mMinValue(0)
    , mMaxValue(0)
    , mValue(0)
    , mLongPressUpdateInterval(DEFAULT_LONG_PRESS_UPDATE_INTERVAL)
    , mSelectorElementHeight(0)
    , mInitialScrollOffset(Elastos::Core::Math::INT32_MIN_VALUE)
    , mCurrentScrollOffset(0)
    , mPreviousScrollerY(0)
    , mLastDownEventY(0.f)
    , mLastDownEventTime(0LL)
    , mLastDownOrMoveEventY(0.f)
    , mTouchSlop(0)
    , mMinimumFlingVelocity(0)
    , mMaximumFlingVelocity(0)
    , mWrapSelectorWheel(FALSE)
    , mSolidColor(0)
    , mHasSelectorWheel(FALSE)
    , mSelectionDividerHeight(0)
    , mScrollState(INumberPickerOnScrollListener::SCROLL_STATE_IDLE)
    , mIgnoreMoveEvents(FALSE)
    , mPerformClickOnTap(FALSE)
    , mTopSelectionDividerTop(0)
    , mBottomSelectionDividerBottom(0)
    , mLastHoveredChildVirtualViewId(0)
    , mIncrementVirtualButtonPressed(FALSE)
    , mDecrementVirtualButtonPressed(FALSE)
    , mLastHandledDownDpadKeyCode(-1)
    , mHideWheelUntilFocused(FALSE)
{
    CSparseArray::New((ISparseArray**)&mSelectorIndexToStringCache);
    mSelectorIndices = ArrayOf<Int32>::Alloc(SELECTOR_WHEEL_ITEM_COUNT);
}

NumberPicker::~NumberPicker()
{}

ECode NumberPicker::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode NumberPicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::numberPickerStyle);
}

ECode NumberPicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode NumberPicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::NumberPicker),
            ArraySize(R::styleable::NumberPicker));
    AutoPtr<ITypedArray> attributesArray;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&attributesArray));

    Int32 layoutResID = 0;
    attributesArray->GetResourceId(R::styleable::NumberPicker_internalLayout, DEFAULT_LAYOUT_RESOURCE_ID, &layoutResID);

    mHasSelectorWheel = (layoutResID != DEFAULT_LAYOUT_RESOURCE_ID);

    attributesArray->GetBoolean(
            R::styleable::NumberPicker_hideWheelUntilFocused, FALSE, &mHideWheelUntilFocused);

    attributesArray->GetColor(R::styleable::NumberPicker_solidColor, 0, &mSolidColor);

    attributesArray->GetDrawable(R::styleable::NumberPicker_selectionDivider, (IDrawable**)&mSelectionDivider);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> display;
    resources->GetDisplayMetrics((IDisplayMetrics**)&display);
    Int32 defSelectionDividerHeight = (Int32)CTypedValue::ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP, UNSCALED_DEFAULT_SELECTION_DIVIDER_HEIGHT, display);
    attributesArray->GetDimensionPixelSize(R::styleable::NumberPicker_selectionDividerHeight, defSelectionDividerHeight, &mSelectionDividerHeight);
    Int32 defSelectionDividerDistance = (Int32)CTypedValue::ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP, UNSCALED_DEFAULT_SELECTION_DIVIDERS_DISTANCE, display);
    attributesArray->GetDimensionPixelSize(R::styleable::NumberPicker_selectionDividersDistance, defSelectionDividerDistance, &mSelectionDividersDistance);

    attributesArray->GetDimensionPixelSize(R::styleable::NumberPicker_internalMinHeight, SIZE_UNSPECIFIED, &mMinHeight);
    attributesArray->GetDimensionPixelSize(R::styleable::NumberPicker_internalMaxHeight, SIZE_UNSPECIFIED, &mMaxHeight);

    if (mMinHeight != SIZE_UNSPECIFIED && mMaxHeight != SIZE_UNSPECIFIED && mMinHeight > mMaxHeight) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    attributesArray->GetDimensionPixelSize(R::styleable::NumberPicker_internalMinWidth, SIZE_UNSPECIFIED, &mMinWidth);
    attributesArray->GetDimensionPixelSize(R::styleable::NumberPicker_internalMaxWidth, SIZE_UNSPECIFIED, &mMaxWidth);

    if (mMinWidth != SIZE_UNSPECIFIED && mMaxWidth != SIZE_UNSPECIFIED && mMinWidth > mMaxWidth) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mComputeMaxWidth = (mMaxWidth == SIZE_UNSPECIFIED);

    attributesArray->GetDrawable(R::styleable::NumberPicker_virtualButtonPressedDrawable, (IDrawable**)&mVirtualButtonPressedDrawable);

    attributesArray->Recycle();

    mPressedStateHelper = new PressedStateHelper(this);

    SetWillNotDraw(!mHasSelectorWheel);

    AutoPtr<IContext> con;
    GetContext((IContext**)&con);
    AutoPtr<IInterface> i;
    con->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&i);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(i);
    AutoPtr<IView> v;
    inflater->Inflate(layoutResID, this, TRUE, (IView**)&v);

    AutoPtr<IViewOnClickListener> onClickListener = new NumberPickerOnClickListener(this);
    AutoPtr<IViewOnLongClickListener> onLongClickListener = new NumberPickerOnLongCliskListener(this);

    if (!mHasSelectorWheel) {
        AutoPtr<IView> view;
        FindViewById(R::id::increment, (IView**)&view);
        mInCrementButton = IImageButton::Probe(view);
        view->SetOnClickListener(onClickListener);
        view->SetOnLongClickListener(onLongClickListener);
    }
    else {
        mInCrementButton = NULL;
    }

    if (!mHasSelectorWheel) {
        AutoPtr<IView> view;
        FindViewById(R::id::decrement, (IView**)&view);
        mDecrementButton = IImageButton::Probe(view);
        view->SetOnClickListener(onClickListener);
        view->SetOnLongClickListener(onLongClickListener);
    }
    else {
        mDecrementButton = NULL;
    }

    AutoPtr<IView> view;
    FindViewById(R::id::numberpicker_input, (IView**)&view);
    mInputText = IEditText::Probe(view);
    AutoPtr<IViewOnFocusChangeListener> listener = new NumberPickerOnFocusChangeListener(this);
    IView::Probe(mInputText)->SetOnFocusChangeListener(listener);

    using Elastos::Droid::Text::IInputFilter;

    AutoPtr<ArrayOf<IInputFilter*> > array = ArrayOf<IInputFilter*>::Alloc(1);
    AutoPtr<InputTextFilter> inputFilter = new InputTextFilter(this);
    array->Set(0, (IInputFilter*)inputFilter.Get());
    AutoPtr<ITextView> mInputTextProbe = ITextView::Probe(mInputText);
    mInputTextProbe->SetFilters(array);

    mInputTextProbe->SetRawInputType(IInputType::TYPE_CLASS_NUMBER);
    mInputTextProbe->SetImeOptions(IEditorInfo::IME_ACTION_DONE);

    AutoPtr<IViewConfiguration> configuration = CViewConfiguration::Get(context);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledMinimumFlingVelocity(&mMinimumFlingVelocity);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumFlingVelocity);
    mMaximumFlingVelocity = mMaximumFlingVelocity / SELECTOR_MAX_FLING_VELOCITY_ADJUSTMENT;

    Float size = 0;
    mInputTextProbe->GetTextSize(&size);
    mTextSize = (Int32)size;
    AutoPtr<IPaint> paint;
    FAIL_RETURN(CPaint::New((IPaint**)&paint));
    paint->SetAntiAlias(TRUE);
    paint->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);
    paint->SetTextSize(mTextSize);
    AutoPtr<ITypeface> face;
    mInputTextProbe->GetTypeface((ITypeface**)&face);
    paint->SetTypeface(face);

    AutoPtr<IColorStateList> colors;
    mInputTextProbe->GetTextColors((IColorStateList**)&colors);
    Int32 color = 0;
    colors->GetColorForState(ENABLED_STATE_SET, IColor::WHITE, &color);
    paint->SetColor(color);
    mSelectorWheelPaint = paint;

    CScroller::New(context, NULL, TRUE, (IScroller**)&mFlingScroller);
    CScroller::New(context, (IScroller**)&mAdjustScroller);

    UpdateInputTextView();
    Int32 accessibility;
    GetImportantForAccessibility(&accessibility);
    if (accessibility == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }

    return NOERROR;
}

ECode NumberPicker::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    Boolean result;
    if (!mHasSelectorWheel || (IsEnabled(&result), !result)) {
        return NOERROR;
    }
    Int32 action = 0;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            RemoveAllCallbacks();
            IView::Probe(mInputText)->SetVisibility(IView::INVISIBLE);
            event->GetY(&mLastDownEventY);
            event->GetY(&mLastDownOrMoveEventY);
            IInputEvent::Probe(event)->GetEventTime(&mLastDownEventTime);
            mIgnoreMoveEvents = FALSE;
            mPerformClickOnTap = FALSE;

            if (mLastDownEventY < mTopSelectionDividerTop) {
                if (mScrollState == INumberPickerOnScrollListener::SCROLL_STATE_IDLE) {
                    mPressedStateHelper->ButtonPressDelayed(PressedStateHelper::BUTTON_DECREMENT);
                }
            }
            else if (mLastDownEventY > mBottomSelectionDividerBottom) {
                if (mScrollState == INumberPickerOnScrollListener::SCROLL_STATE_IDLE) {
                    mPressedStateHelper->ButtonPressDelayed(PressedStateHelper::BUTTON_INCREMENT);
                }
            }

            AutoPtr<IViewParent> parent;
            GetParent((IViewParent**)&parent);
            parent->RequestDisallowInterceptTouchEvent(TRUE);
            Boolean isFlingFinished = FALSE;
            mFlingScroller->IsFinished(&isFlingFinished);
            Boolean isAdjustFinished = FALSE;
            mAdjustScroller->IsFinished(&isAdjustFinished);
            if (!isFlingFinished) {
                mFlingScroller->ForceFinished(TRUE);
                mAdjustScroller->ForceFinished(TRUE);
                OnScrollStateChange(INumberPickerOnScrollListener::SCROLL_STATE_IDLE);
            }
            else if (!isAdjustFinished) {
                mFlingScroller->ForceFinished(TRUE);
                mAdjustScroller->ForceFinished(TRUE);
            }
            else if (mLastDownEventY < mTopSelectionDividerTop) {
                HideSoftInput();
                PostChangeCurrentByOneFromLongPress(FALSE, CViewConfiguration::GetLongPressTimeout());
            }
            else if (mLastDownEventY > mBottomSelectionDividerBottom) {
                HideSoftInput();
                PostChangeCurrentByOneFromLongPress(TRUE, CViewConfiguration::GetLongPressTimeout());
            }
            else {
                mPerformClickOnTap = TRUE;
                PostBeginSoftInputOnLongPressCommand();
            }
            *res = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode NumberPicker::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    Boolean result;
    if ((IsEnabled(&result), result) || !mHasSelectorWheel) {
        return NOERROR;
    }
    if (mVelocityTracker == NULL) {
        mVelocityTracker = VelocityTracker::Obtain();
    }
    mVelocityTracker->AddMovement(event);
    Int32 action = 0;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_MOVE: {
            if (mIgnoreMoveEvents) {
                break;
            }
            Float currentMoveY = 0;
            event->GetY(&currentMoveY);
            if (mScrollState != INumberPickerOnScrollListener::SCROLL_STATE_TOUCH_SCROLL) {
                Int32 deltaDownY = (Int32)Elastos::Core::Math::Abs(currentMoveY - mLastDownEventY);
                if (deltaDownY > mTouchSlop) {
                    RemoveAllCallbacks();
                    OnScrollStateChange(INumberPickerOnScrollListener::SCROLL_STATE_TOUCH_SCROLL);
                }
            }
            else {
                Int32 deltaMoveY = (Int32)(currentMoveY - mLastDownOrMoveEventY);
                ScrollBy(0, deltaMoveY);
                Invalidate();
            }
            mLastDownOrMoveEventY = currentMoveY;
        }
        break;

        case IMotionEvent::ACTION_UP: {
            RemoveBeginSoftInputCommand();
            RemoveChangeCurrentByOneFromLongPress();
            mPressedStateHelper->Cancel();
            AutoPtr<VelocityTracker> velocityTracker = mVelocityTracker;
            velocityTracker->ComputeCurrentVelocity(1000, (Float)mMaximumFlingVelocity);
            Float y;
            velocityTracker->GetYVelocity(&y);
            Int32 initialVelocity = (Int32)y;
            if (Elastos::Core::Math::Abs(initialVelocity) > mMinimumFlingVelocity) {
                Fling(initialVelocity);
                OnScrollStateChange(INumberPickerOnScrollListener::SCROLL_STATE_FLING);
            }
            else {
                Float fy = 0;
                event->GetY(&fy);
                Int32 eventY = (Int32)fy;
                Int32 deltaMoveY = (Int32)(Elastos::Core::Math::Abs(eventY - mLastDownEventY));
                Int64 deltaTime = 0;
                IInputEvent::Probe(event)->GetEventTime(&deltaTime);
                deltaTime -= mLastDownEventTime;
                if (deltaMoveY <= mTouchSlop && deltaTime < CViewConfiguration::GetTapTimeout()) {
                    if (mPerformClickOnTap) {
                        mPerformClickOnTap = FALSE;
                        PerformClick(&result);
                    }
                    else {
                        Int32 selectorIndexOffset = (eventY / mSelectorElementHeight) - SELECTOR_MIDDLE_ITEM_INDEX;
                        if (selectorIndexOffset > 0) {
                            ChangeValueByOne(TRUE);
                            mPressedStateHelper->ButtonTapped(PressedStateHelper::BUTTON_INCREMENT);
                        }
                        else if (selectorIndexOffset < 0) {
                            ChangeValueByOne(FALSE);
                            mPressedStateHelper->ButtonTapped(PressedStateHelper::BUTTON_DECREMENT);
                        }
                    }
                }
                else {
                    EnsureScrollWheelAdjusted();
                }
                OnScrollStateChange(INumberPickerOnScrollListener::SCROLL_STATE_IDLE);
            }
            mVelocityTracker->Recycle();
            mVelocityTracker = NULL;
        }
        break;
    }
    *res = TRUE;
    return NOERROR;
}

ECode NumberPicker::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 action = 0;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            RemoveAllCallbacks();
            break;
    }
    return LinearLayout::DispatchTouchEvent(event, res);
}

ECode NumberPicker::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 keyCode = 0;
    event->GetKeyCode(&keyCode);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_ENTER:
            RemoveAllCallbacks();
            break;
        case IKeyEvent::KEYCODE_DPAD_DOWN:
        case IKeyEvent::KEYCODE_DPAD_UP: {
            if (!mHasSelectorWheel) {
                break;
            }
            Int32 action;
            event->GetAction(&action);
            switch (action) {
                case IKeyEvent::ACTION_DOWN: {
                    Int32 value, maxValue, minValue;
                    GetValue(&value);
                    GetMaxValue(&maxValue);
                    GetMinValue(&minValue);
                    if (mWrapSelectorWheel || ((keyCode == IKeyEvent::KEYCODE_DPAD_DOWN)
                            ? value < maxValue : value > minValue)) {
                        Boolean result;
                        RequestFocus(&result);
                        mLastHandledDownDpadKeyCode = keyCode;
                        RemoveAllCallbacks();
                        Boolean isFinished;
                        mFlingScroller->IsFinished(&isFinished);
                        if (isFinished) {
                            ChangeValueByOne(keyCode == IKeyEvent::KEYCODE_DPAD_DOWN);
                        }
                        *res = TRUE;
                        return NOERROR;
                    }
                    break;
                }
                case IKeyEvent::ACTION_UP:
                    if (mLastHandledDownDpadKeyCode == keyCode) {
                        mLastHandledDownDpadKeyCode = -1;
                        *res = TRUE;
                        return NOERROR;
                    }
                    break;
            }
        }
    }
    return LinearLayout::DispatchKeyEvent(event, res);
}

ECode NumberPicker::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 action = 0;
    event->GetActionMasked(&action);
    switch (action) {
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            RemoveAllCallbacks();
            break;
    }
    return LinearLayout::DispatchTrackballEvent(event, res);
}

Boolean NumberPicker::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    if (!mHasSelectorWheel) {
        return LinearLayout::DispatchHoverEvent(event);
    }
    AutoPtr<IAccessibilityManager> am;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&am);
    Boolean isEnabled = FALSE;
    am->IsEnabled(&isEnabled);
    if (isEnabled) {
        Float fy = 0;
        event->GetY(&fy);
        Int32 eventY = (Int32)fy;
        Int32 hoveredVirtualViewId = 0;
        if (eventY < mTopSelectionDividerTop) {
            hoveredVirtualViewId = AccessibilityNodeProviderImpl::VIRTUAL_VIEW_ID_DECREMENT;
        }
        else if (eventY > mBottomSelectionDividerBottom) {
            hoveredVirtualViewId = AccessibilityNodeProviderImpl::VIRTUAL_VIEW_ID_INCREMENT;
        }
        else {
            hoveredVirtualViewId = AccessibilityNodeProviderImpl::VIRTUAL_VIEW_ID_INPUT;
        }

        Int32 action = 0;
        event->GetActionMasked(&action);
        AutoPtr<IAccessibilityNodeProvider> provider;
        GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        AutoPtr<AccessibilityNodeProviderImpl> providerImpl = (AccessibilityNodeProviderImpl*)provider.Get();
        Boolean res = FALSE;
        switch (action) {
            case IMotionEvent::ACTION_HOVER_ENTER:
                providerImpl->SendAccessibilityEventForVirtualView(hoveredVirtualViewId, IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER);
                mLastHoveredChildVirtualViewId = hoveredVirtualViewId;
                providerImpl->PerformAction(hoveredVirtualViewId, IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS, NULL, &res);
                break;
            case IMotionEvent::ACTION_HOVER_MOVE:
                if (mLastHoveredChildVirtualViewId != hoveredVirtualViewId &&
                    mLastHoveredChildVirtualViewId != IView::NO_ID) {
                    providerImpl->SendAccessibilityEventForVirtualView(mLastHoveredChildVirtualViewId, IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT);
                    providerImpl->SendAccessibilityEventForVirtualView(hoveredVirtualViewId, IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER);
                    mLastHoveredChildVirtualViewId = hoveredVirtualViewId;
                    providerImpl->PerformAction(hoveredVirtualViewId, IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS, NULL, &res);
                }
                break;
            case IMotionEvent::ACTION_HOVER_EXIT:
                providerImpl->SendAccessibilityEventForVirtualView(hoveredVirtualViewId, IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT);
                mLastHoveredChildVirtualViewId = IView::NO_ID;
                break;
        }
    }
    return FALSE;
}

ECode NumberPicker::ComputeScroll()
{
    AutoPtr<IScroller> scroller = mFlingScroller;
    Boolean flingFinished = FALSE;
    scroller->IsFinished(&flingFinished);
    if (flingFinished) {
        scroller = mAdjustScroller;
        Boolean adjustFinished = FALSE;
        scroller->IsFinished(&adjustFinished);
        if (adjustFinished) {
            return NOERROR;
        }
    }
    Boolean res = FALSE;
    scroller->ComputeScrollOffset(&res);
    Int32 currentScrollerY = 0;
    scroller->GetCurrY(&currentScrollerY);
    if (mPreviousScrollerY == 0) {
        scroller->GetStartY(&mPreviousScrollerY);
    }
    ScrollBy(0, currentScrollerY - mPreviousScrollerY);
    mPreviousScrollerY = currentScrollerY;
    Boolean isFinished = FALSE;
    scroller->IsFinished(&isFinished);
    if (isFinished) {
        OnScrollerFinished(scroller);
    }
    else {
        Invalidate();
    }

    return NOERROR;
}

ECode NumberPicker::SetEnabled(
    /* [in] */ Boolean enabled)
{
    LinearLayout::SetEnabled(enabled);
    if (!mHasSelectorWheel) {
        IView::Probe(mInCrementButton)->SetEnabled(enabled);
    }
    if (!mHasSelectorWheel) {
        IView::Probe(mDecrementButton)->SetEnabled(enabled);
    }
    IView::Probe(mInputText)->SetEnabled(enabled);
    return NOERROR;
}

ECode NumberPicker::ScrollBy(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<ArrayOf<Int32> > selectorIndices = mSelectorIndices;
    if (!mWrapSelectorWheel && y > 0
        && (*selectorIndices)[SELECTOR_MIDDLE_ITEM_INDEX] <= mMinValue) {
        mCurrentScrollOffset = mInitialScrollOffset;
        return NOERROR;
    }

    if (!mWrapSelectorWheel && y < 0
        && (*selectorIndices)[SELECTOR_MIDDLE_ITEM_INDEX] >= mMaxValue) {
        mCurrentScrollOffset = mInitialScrollOffset;
        return NOERROR;
    }

    mCurrentScrollOffset += y;
    while (mCurrentScrollOffset - mInitialScrollOffset > mSelectorTextGapHeight) {
        mCurrentScrollOffset -= mSelectorElementHeight;
        DecrementSelectorIndices(selectorIndices);
        SetValueInternal((*selectorIndices)[SELECTOR_MIDDLE_ITEM_INDEX], TRUE);
        if (!mWrapSelectorWheel && (*selectorIndices)[SELECTOR_MIDDLE_ITEM_INDEX] <= mMinValue) {
            mCurrentScrollOffset = mInitialScrollOffset;
        }
    }

    while (mCurrentScrollOffset - mInitialScrollOffset < -mSelectorTextGapHeight) {
        mCurrentScrollOffset += mSelectorElementHeight;
        IncrementSelectorIndices(selectorIndices);
        SetValueInternal((*selectorIndices)[SELECTOR_MIDDLE_ITEM_INDEX], TRUE);
        if (!mWrapSelectorWheel && (*selectorIndices)[SELECTOR_MIDDLE_ITEM_INDEX] >= mMaxValue) {
            mCurrentScrollOffset = mInitialScrollOffset;
        }
    }
    return NOERROR;
}

Int32 NumberPicker::ComputeVerticalScrollOffset()
{
    return mCurrentScrollOffset;
}

Int32 NumberPicker::ComputeVerticalScrollRange()
{
    return (mMaxValue - mMinValue + 1) * mSelectorElementHeight;
}

Int32 NumberPicker::ComputeVerticalScrollExtent()
{
    Int32 height;
    GetHeight(&height);
    return height;
}

ECode NumberPicker::GetSolidColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mSolidColor;
    return NOERROR;
}

ECode NumberPicker::SetOnValueChangedListener(
    /* [in] */ INumberPickerOnValueChangeListener* onValueChangedListener)
{
    mOnValueChangeListener = onValueChangedListener;
    return NOERROR;
}

ECode NumberPicker::SetOnScrollListener(
    /* [in] */ INumberPickerOnScrollListener* onScrollListener)
{
    mOnScrollListener = onScrollListener;
    return NOERROR;
}

ECode NumberPicker::SetFormatter(
    /* [in] */ INumberPickerFormatter* formatter)
{
    if (formatter == mFormatter) {
        return NOERROR;
    }
    mFormatter = formatter;
    InitializeSelectorWheelIndices();
    UpdateInputTextView();
    return NOERROR;
}

ECode NumberPicker::SetValue(
    /* [in] */ Int32 value)
{
    SetValueInternal(value, FALSE);
    return NOERROR;
}

ECode NumberPicker::PerformClick(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean result;
    if (!mHasSelectorWheel) {
        return LinearLayout::PerformClick(res);
    }
    else if (LinearLayout::PerformClick(&result), !result) {
        ShowSoftInput();
    }
    *res = TRUE;
    return NOERROR;
}

ECode NumberPicker::PerformLongClick(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean result;
    if (!mHasSelectorWheel) {
        return LinearLayout::PerformLongClick(res);
    }
    else if (LinearLayout::PerformLongClick(&result), !result) {
        ShowSoftInput();
        mIgnoreMoveEvents = TRUE;
    }
    *res = TRUE;
    return NOERROR;
}

ECode NumberPicker::GetWrapSelectorWheel(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mWrapSelectorWheel;
    return NOERROR;
}

ECode NumberPicker::SetWrapSelectorWheel(
    /* [in] */ Boolean wrapSelectorWheel)
{
    Boolean wrappingAllowed = (mMaxValue - mMinValue) >= (*mSelectorIndices).GetLength();
    if ((!wrapSelectorWheel || wrappingAllowed) && wrapSelectorWheel != mWrapSelectorWheel) {
        mWrapSelectorWheel = wrapSelectorWheel;
    }
    return NOERROR;
}

ECode NumberPicker::SetOnLongPressUpdateInterval(
    /* [in] */ Int64 intervalMillis)
{
    mLongPressUpdateInterval = intervalMillis;
    return NOERROR;
}

ECode NumberPicker::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode NumberPicker::GetMinValue(
    /* [out] */ Int32* minValue)
{
    VALIDATE_NOT_NULL(minValue);
    *minValue = mMinValue;
    return NOERROR;
}

ECode NumberPicker::SetMinValue(
    /* [in] */ Int32 minValue)
{
    if (mMinValue == minValue) {
        return NOERROR;
    }
    if (minValue < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMinValue = minValue;
    if (mMinValue > mValue) {
        mValue = mMinValue;
    }

    Boolean wrapSelectorWheel = mMaxValue - mMinValue > (*mSelectorIndices).GetLength();
    SetWrapSelectorWheel(wrapSelectorWheel);
    InitializeSelectorWheelIndices();
    UpdateInputTextView();
    TryComputeMaxWidth();
    Invalidate();
    return NOERROR;
}

ECode NumberPicker::GetMaxValue(
    /* [out] */ Int32* maxValue)
{
    VALIDATE_NOT_NULL(maxValue);
    *maxValue = mMaxValue;
    return NOERROR;
}

ECode NumberPicker::SetMaxValue(
    /* [in] */ Int32 maxValue)
{
    if (mMaxValue == maxValue) {
        return NOERROR;
    }
    if (maxValue < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMaxValue = maxValue;
    if (mMaxValue < mValue) {
        mValue = mMaxValue;
    }

    Boolean wrapSelectorWheel = mMaxValue - mMinValue > (*mSelectorIndices).GetLength();
    SetWrapSelectorWheel(wrapSelectorWheel);
    InitializeSelectorWheelIndices();
    UpdateInputTextView();
    TryComputeMaxWidth();
    Invalidate();
    return NOERROR;
}

ECode NumberPicker::GetDisplayedValues(
    /* [out, callee] */ ArrayOf<String>** displayedValues)
{
    VALIDATE_NOT_NULL(displayedValues);
    *displayedValues = mDisplayedValues;
    REFCOUNT_ADD(*displayedValues);
    return NOERROR;
}

ECode NumberPicker::SetDisplayedValues(
    /* [in] */ ArrayOf<String>* displayedValues)
{
    if (mDisplayedValues.Get() == displayedValues) {
        return NOERROR;
    }
    mDisplayedValues = displayedValues;
    if (mDisplayedValues) {
        ITextView::Probe(mInputText)->SetRawInputType(IInputType::TYPE_CLASS_TEXT
            | IInputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS);
    }
    else {
        ITextView::Probe(mInputText)->SetRawInputType(IInputType::TYPE_CLASS_NUMBER);
    }
    UpdateInputTextView();
    InitializeSelectorWheelIndices();
    TryComputeMaxWidth();
    return NOERROR;
}

ECode NumberPicker::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    LinearLayout::OnInitializeAccessibilityEvent(event);

    AutoPtr<IAccessibilityRecord> record = IAccessibilityRecord::Probe(event);
    record->SetClassName(CoreUtils::Convert(NUMBERPICKER_NAME));
    record->SetScrollable(TRUE);
    record->SetScrollY((mMinValue + mValue) * mSelectorElementHeight);
    record->SetMaxScrollY((mMaxValue - mMinValue) * mSelectorElementHeight);
    return NOERROR;
}

ECode NumberPicker::GetAccessibilityNodeProvider(
    /* [out] */ IAccessibilityNodeProvider** provider)
{
    VALIDATE_NOT_NULL(provider);

    if (!mHasSelectorWheel) {
        return LinearLayout::GetAccessibilityNodeProvider(provider);
    }
    if (mAccessibilityNodeProvider == NULL) {
        mAccessibilityNodeProvider = new AccessibilityNodeProviderImpl(this);
    }
    *provider = mAccessibilityNodeProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode NumberPicker::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 letf,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (!mHasSelectorWheel) {
        LinearLayout::OnLayout(changed, letf, top, right, bottom);
        return NOERROR;
    }

    Int32 msrdWdth;
    GetMeasuredWidth(&msrdWdth);
    Int32 msrdHght;
    GetMeasuredHeight(&msrdHght);

    AutoPtr<IView> view = IView::Probe(mInputText);
    Int32 inptTxtMsrdWdth = 0;
    view->GetMeasuredWidth(&inptTxtMsrdWdth);
    Int32 inptTxtMsrdHght = 0;
    view->GetMeasuredHeight(&inptTxtMsrdHght);

    Int32 inptTxtLeft = (msrdWdth - inptTxtMsrdWdth) / 2;
    Int32 inptTxtTop = (msrdHght - inptTxtMsrdHght) / 2;
    Int32 inptTxtRight = inptTxtLeft + inptTxtMsrdWdth;
    Int32 inptTxtBottom = inptTxtTop + inptTxtMsrdHght;

    view->Layout(inptTxtLeft, inptTxtTop, inptTxtRight, inptTxtBottom);

    if (changed) {
        InitializeSelectorWheel();
        InitializeFadingEdges();
        Int32 height;
        GetHeight(&height);
        mTopSelectionDividerTop = (height - mSelectionDividersDistance) / 2 - mSelectionDividerHeight;

        mBottomSelectionDividerBottom = mTopSelectionDividerTop + 2 * mSelectionDividerHeight + mSelectionDividersDistance;
    }
    return NOERROR;
}

void NumberPicker::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (!mHasSelectorWheel) {
        LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
        return;
    }
    Int32 newWihthMeasureSpec = MakeMeasureSpec(widthMeasureSpec, mMaxWidth);
    Int32 newHeightMeasureSpec = MakeMeasureSpec(heightMeasureSpec, mMaxHeight);
    LinearLayout::OnMeasure(newWihthMeasureSpec, newHeightMeasureSpec);
    Int32 width, height;
    GetMeasuredWidth(&width);
    GetMeasuredHeight(&height);
    Int32 widthSize = ResolveSizeAndStateRespectingMinSize(mMinWidth, width, widthMeasureSpec);
    Int32 heightSize = ResolveSizeAndStateRespectingMinSize(mMinHeight, height, heightMeasureSpec);
    SetMeasuredDimension(widthSize, heightSize);
}

Float NumberPicker::GetTopFadingEdgeStrength()
{
    return TOP_AND_BOTTOM_FADING_EDGE_STRENGTH;
}

Float NumberPicker::GetBottomFadingEdgeStrength()
{
    return TOP_AND_BOTTOM_FADING_EDGE_STRENGTH;
}

ECode NumberPicker::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();
    RemoveAllCallbacks();
    return NOERROR;
}

void NumberPicker::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (!mHasSelectorWheel) {
        LinearLayout::OnDraw(canvas);
        return;
    }

    Boolean focus;
    const Boolean showSelectorWheel = mHideWheelUntilFocused ? (HasFocus(&focus), focus) : TRUE;
    Float x = (mRight - mLeft) / 2;
    Float y = mCurrentScrollOffset;

    // draw the virtual buttons pressed state if needed
    if (showSelectorWheel && mVirtualButtonPressedDrawable && mScrollState == INumberPickerOnScrollListener::SCROLL_STATE_IDLE) {
        Boolean res = FALSE;
        if (mDecrementVirtualButtonPressed) {
            mVirtualButtonPressedDrawable->SetState(PRESSED_STATE_SET, &res);
            mVirtualButtonPressedDrawable->SetBounds(0, 0, mRight, mTopSelectionDividerTop);
            mVirtualButtonPressedDrawable->Draw(canvas);
        }
        if (mIncrementVirtualButtonPressed) {
            mVirtualButtonPressedDrawable->SetState(PRESSED_STATE_SET, &res);
            mVirtualButtonPressedDrawable->SetBounds(0, mBottomSelectionDividerBottom, mRight, mBottom);
            mVirtualButtonPressedDrawable->Draw(canvas);
        }
    }

    // draw the selector wheel
    AutoPtr<ArrayOf<Int32> > selectorIndices = mSelectorIndices;
    for(Int32 i = 0; i < selectorIndices->GetLength(); i++) {
        Int32 selectorIndex = (*selectorIndices)[i];
        AutoPtr<IInterface> obj;
        mSelectorIndexToStringCache->Get(selectorIndex, (IInterface**)&obj);
        AutoPtr<ICharSequence> seq = ICharSequence::Probe(obj);
        String scrollSelectorValue;
        seq->ToString(&scrollSelectorValue);

        // Do not draw the middle item if input is visible since the input
        // is shown only if the wheel is static and it covers the middle
        // item. Otherwise, if the user starts editing the text via the
        // IME he may see a dimmed version of the old value intermixed
        // with the new one.
        Int32 visible = 0;
        IView::Probe(mInputText)->GetVisibility(&visible);
        if ((showSelectorWheel && i != SELECTOR_MIDDLE_ITEM_INDEX) ||
                (i == SELECTOR_MIDDLE_ITEM_INDEX && visible != VISIBLE)) {
            canvas->DrawText(scrollSelectorValue, x, y, mSelectorWheelPaint);
        }
        y += mSelectorElementHeight;
    }

    // draw the selection dividers
    if (showSelectorWheel && mSelectionDivider != NULL) {
        // draw the top divider
        Int32 topOfTopDivider = mTopSelectionDividerTop;
        Int32 bottomOfTopDivider = topOfTopDivider + mSelectionDividerHeight;
        mSelectionDivider->SetBounds(0, topOfTopDivider, mRight, bottomOfTopDivider);
        mSelectionDivider->Draw(canvas);

        // draw the bottom divider
        Int32 bottomOfBottomDivider = mBottomSelectionDividerBottom;
        Int32 topOfBottomDivider = bottomOfBottomDivider - mSelectionDividerHeight;
        mSelectionDivider->SetBounds(0, topOfBottomDivider, mRight, bottomOfBottomDivider);
        mSelectionDivider->Draw(canvas);
    }
}

Boolean NumberPicker::MoveToFinalScrollerPosition(
    /* [in] */ IScroller* scroller)
{
    scroller->ForceFinished(TRUE);
    Int32 finalY = 0;
    scroller->GetFinalY(&finalY);
    Int32 currY = 0;
    scroller->GetCurrY(&currY);

    Int32 amountToScroll = finalY - currY;
    Int32 futureScrollOffset = (mCurrentScrollOffset + amountToScroll) % mSelectorElementHeight;
    Int32 overshootAdjustment = mInitialScrollOffset - futureScrollOffset;

    if (overshootAdjustment != 0) {
        if (Elastos::Core::Math::Abs(overshootAdjustment) > mSelectorElementHeight / 2) {
            if (overshootAdjustment > 0) {
                overshootAdjustment -= mSelectorElementHeight;
            }
            else {
                overshootAdjustment += mSelectorElementHeight;
            }
        }
        amountToScroll += overshootAdjustment;
        ScrollBy(0, amountToScroll);
        return TRUE;
    }
    return FALSE;
}

void NumberPicker::ShowSoftInput()
{
    AutoPtr<IInputMethodManager> inputMethodManager = CInputMethodManager::PeekInstance();
    if (inputMethodManager != NULL) {
        if (mHasSelectorWheel) {
            IView::Probe(mInputText)->SetVisibility(IView::VISIBLE);
        }
        Boolean res = FALSE;
        IView::Probe(mInputText)->RequestFocus(&res);
        inputMethodManager->ShowSoftInput(IView::Probe(mInputText), 0, &res);
    }
}

void NumberPicker::HideSoftInput()
{
    AutoPtr<IInputMethodManager> inputMethodManager = CInputMethodManager::PeekInstance();
    if (inputMethodManager == NULL) {
        return;
    }
    Boolean isActive = FALSE;
    inputMethodManager->IsActive(IView::Probe(mInputText), &isActive);
    if (inputMethodManager && isActive) {
        Boolean res = FALSE;
        AutoPtr<IBinder> binder;
        GetWindowToken((IBinder**)&binder);
        inputMethodManager->HideSoftInputFromWindow(binder, 0, &res);
        if (mHasSelectorWheel) {
            IView::Probe(mInputText)->SetVisibility(IView::INVISIBLE);
        }
    }
}

void NumberPicker::TryComputeMaxWidth()
{
    if (!mComputeMaxWidth) {
        return;
    }
    Int32 maxTextWidth = 0;
    if (mDisplayedValues == NULL) {
        Float maxDigitWidth = 0;
        for (Int32 i = 0; i <= 9; i++) {
            Float digitWidth = 0;
            mSelectorWheelPaint->MeasureText(mAccessibilityNodeProvider->FormatNumberWithLocale(i), &digitWidth);
            if (digitWidth > maxDigitWidth) {
                maxDigitWidth = digitWidth;
            }
        }
        Int32 numberOfDigits = 0;
        Int32 current = mMaxValue;
        while (current > 0) {
            numberOfDigits++;
            current /= 10;
        }
        maxTextWidth = (Int32)(numberOfDigits * maxDigitWidth);
    }
    else {
        Int32 valueCount = mDisplayedValues->GetLength();
        for (Int32 i = 0; i < valueCount; i++) {
            Float textWidth = 0;
            mSelectorWheelPaint->MeasureText((*mDisplayedValues)[i], &textWidth);
            if (textWidth > maxTextWidth) {
                maxTextWidth = (Int32)textWidth;
            }
        }
    }
    Int32 paddingLeft = 0;
    IView::Probe(mInputText)->GetPaddingLeft(&paddingLeft);
    Int32 paddingRight = 0;
    IView::Probe(mInputText)->GetPaddingRight(&paddingRight);
    maxTextWidth += paddingLeft + paddingRight;
    if (mMaxWidth != maxTextWidth) {
        if (maxTextWidth > mMinWidth){
            mMaxWidth = maxTextWidth;
        }
        else {
            mMaxWidth = mMinWidth;
     }
    Invalidate();
    }
}

Int32 NumberPicker::MakeMeasureSpec(
    /* [in] */ Int32 measureSpec,
    /* [in] */ Int32 maxSize)
{
    if (maxSize == SIZE_UNSPECIFIED) {
        return measureSpec;
    }
    Int32 size = View::MeasureSpec::GetSize(measureSpec);
    Int32 mode = View::MeasureSpec::GetMode(measureSpec);
    switch (mode) {
        case View::MeasureSpec::EXACTLY:
            return measureSpec;
        case View::MeasureSpec::AT_MOST:
            return View::MeasureSpec::MakeMeasureSpec(Elastos::Core::Math::Min(size, maxSize), View::MeasureSpec::EXACTLY);
        case View::MeasureSpec::UNSPECIFIED:
            return View::MeasureSpec::MakeMeasureSpec(maxSize, View::MeasureSpec::EXACTLY);
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

Int32 NumberPicker::ResolveSizeAndStateRespectingMinSize(
    /* [in] */ Int32 minSize,
    /* [in] */ Int32 measuredSize,
    /* [in] */ Int32 measureSpec)
{
    if (minSize != SIZE_UNSPECIFIED) {
        Int32 desiredWidth = Elastos::Core::Math::Max(minSize, measuredSize);
        return ResolveSizeAndState(desiredWidth, measureSpec, 0);
    }
    else {
        return measuredSize;
    }
}

void NumberPicker::InitializeSelectorWheelIndices()
{
    mSelectorIndexToStringCache->Clear();
    AutoPtr<ArrayOf<Int32> > selectorIndices = mSelectorIndices;
    Int32 current;
    GetValue(&current);
    for (Int32 i = 0; i < (*mSelectorIndices).GetLength(); i++) {
        Int32 selectorIndex = current + (i - SELECTOR_MIDDLE_ITEM_INDEX);
        if (mWrapSelectorWheel) {
            selectorIndex = GetWrappedSelectorIndex(selectorIndex);
        }
        (*selectorIndices)[i] = selectorIndex;
        EnsureCachedScrollSelectorValue((*selectorIndices)[i]);
    }
}

void NumberPicker::SetValueInternal(
    /* [in] */ Int32 current,
    /* [in] */ Boolean notifyChange)
{
    if (mValue == current) {
        return;
    }
    if (mWrapSelectorWheel) {
        current = GetWrappedSelectorIndex(current);
    }
    else {
        current = Elastos::Core::Math::Max(current, mMinValue);
        current = Elastos::Core::Math::Min(current, mMaxValue);
    }
    Int32 previous = mValue;
    mValue = current;
    UpdateInputTextView();
    if (notifyChange) {
        NotifyChange(previous, current);
    }
    InitializeSelectorWheelIndices();
    Invalidate();
}

void NumberPicker::ChangeValueByOne(
    /* [in] */ Boolean increment)
{
    if (mHasSelectorWheel) {
        IView::Probe(mInputText)->SetVisibility(IView::INVISIBLE);
        if (!MoveToFinalScrollerPosition(mFlingScroller)) {
            MoveToFinalScrollerPosition(mAdjustScroller);
        }
        mPreviousScrollerY = 0;
        if (increment) {
            mFlingScroller->StartScroll(0, 0, 0, -mSelectorElementHeight, SNAP_SCROLL_DURATION);
        }
        else {
            mFlingScroller->StartScroll(0, 0, 0, mSelectorElementHeight, SNAP_SCROLL_DURATION);
        }
        Invalidate();
    }
    else {
        if (increment) {
            SetValueInternal(mValue + 1, TRUE);
        }
        else {
            SetValueInternal(mValue - 1, TRUE);
        }
    }
}

void NumberPicker::InitializeSelectorWheel()
{
    InitializeSelectorWheelIndices();
    AutoPtr<ArrayOf<Int32> > selectorIndices = mSelectorIndices;
    Int32 totalTextHeight = (selectorIndices->GetLength()) * mTextSize;
    Float totalTextGapHeight = (mBottom - mTop) - totalTextHeight;
    Float TextGapCount = (*selectorIndices).GetLength();
    mSelectorTextGapHeight = (Int32)(totalTextGapHeight / TextGapCount + 0.5f);
    mSelectorElementHeight = mTextSize + mSelectorTextGapHeight;
    Int32 baseLine = 0;
    IView::Probe(mInputText)->GetBaseline(&baseLine);
    Int32 top = 0;
    IView::Probe(mInputText)->GetTop(&top);
    Int32 editTextTextPosition = baseLine + top;
    mInitialScrollOffset = editTextTextPosition - (mSelectorElementHeight * SELECTOR_MIDDLE_ITEM_INDEX);
    mCurrentScrollOffset = mInitialScrollOffset;
    UpdateInputTextView();
}

void NumberPicker::InitializeFadingEdges()
{
    SetVerticalFadingEdgeEnabled(TRUE);
    SetFadingEdgeLength((mBottom - mTop - mTextSize) / 2);
}

void NumberPicker::OnScrollerFinished(
    /* [in] */ IScroller* scroller)
{
    if (scroller == mFlingScroller) {
        if (!EnsureScrollWheelAdjusted()) {
            UpdateInputTextView();
        }
        OnScrollStateChange(INumberPickerOnScrollListener::SCROLL_STATE_IDLE);
    }
    else {
        if (mScrollState != INumberPickerOnScrollListener::SCROLL_STATE_TOUCH_SCROLL) {
            UpdateInputTextView();
        }
    }
}

void NumberPicker::OnScrollStateChange(
    /* [in] */ Int32 scrollState)
{
    if (mScrollState == scrollState) {
        return;
    }
    mScrollState = scrollState;
    if (mOnScrollListener) {
        mOnScrollListener->OnScrollStateChange(this, scrollState);
    }
}

void NumberPicker::Fling(
    /* [in] */ Int32 velocityY)
{
    mPreviousScrollerY = 0;
    if (velocityY > 0) {
        mFlingScroller->Fling(0, 0, 0, velocityY, 0, 0, 0, Elastos::Core::Math::INT32_MAX_VALUE);
    }
    else {
        mFlingScroller->Fling(0, Elastos::Core::Math::INT32_MAX_VALUE, 0, velocityY, 0, 0, 0, Elastos::Core::Math::INT32_MAX_VALUE);
    }
    Invalidate();
}

Int32 NumberPicker::GetWrappedSelectorIndex(
    /* [in] */ Int32 selectorIndex)
{
    if (selectorIndex > mMaxValue) {
        return mMinValue + (selectorIndex - mMaxValue) % (mMaxValue - mMinValue) - 1;
    }
    else if (selectorIndex < mMinValue) {
        return mMaxValue - (mMinValue - selectorIndex) % (mMaxValue - mMinValue) + 1;
    }
    return selectorIndex;
}

void NumberPicker::IncrementSelectorIndices(
    /* [in] */ ArrayOf<Int32>* selectorIndices)
{
    for (Int32 i = 0; i < selectorIndices->GetLength() - 1; i++) {
        (*selectorIndices)[i] = (*selectorIndices)[i + 1];
    }
    Int32 nextScrollSelectorIndex = (*selectorIndices)[selectorIndices->GetLength() - 2] + 1;
    if (mWrapSelectorWheel && nextScrollSelectorIndex > mMaxValue) {
        nextScrollSelectorIndex = mMinValue;
    }
    (*selectorIndices)[selectorIndices->GetLength() - 1] = nextScrollSelectorIndex;
    EnsureCachedScrollSelectorValue(nextScrollSelectorIndex);
}

void NumberPicker::DecrementSelectorIndices(
    /* [in] */ ArrayOf<Int32>* selectorIndices)
{
    for (Int32 i = selectorIndices->GetLength() - 1; i > 0; i--) {
        (*selectorIndices)[i] = (*selectorIndices)[i - 1];
    }
    Int32 nextScrollSelectorIndex = (*selectorIndices)[1] - 1;
    if (mWrapSelectorWheel && nextScrollSelectorIndex < mMinValue) {
        nextScrollSelectorIndex = mMaxValue;
    }
    (*selectorIndices)[0] = nextScrollSelectorIndex;
    EnsureCachedScrollSelectorValue(nextScrollSelectorIndex);
}

void NumberPicker::EnsureCachedScrollSelectorValue(
    /* [in] */ Int32 selectorIndex)
{
    AutoPtr<ISparseArray> cache = mSelectorIndexToStringCache;
    AutoPtr<IInterface> obj;
    cache->Get(selectorIndex, (IInterface**)&obj);
    AutoPtr<ICharSequence> seq = ICharSequence::Probe(obj);
    String scrollSelectorValue;
    seq->ToString(&scrollSelectorValue);

    if (scrollSelectorValue) {
        return;
    }
    if (selectorIndex < mMinValue || selectorIndex > mMaxValue) {
        scrollSelectorValue = String("");
    }
    else {
        if (mDisplayedValues) {
            Int32 displayedValuesIndex = selectorIndex - mMinValue;
            scrollSelectorValue = (*mDisplayedValues)[displayedValuesIndex];
        }
        else {
            scrollSelectorValue = FormatNumber(selectorIndex);
        }
    }
    cache->Put(selectorIndex, CoreUtils::Convert(scrollSelectorValue));
}

String NumberPicker::FormatNumber(
    /* [in] */ Int32 value)
{
    if (mFormatter) {
        String str = String(NULL);
        mFormatter->Format(value, &str);
        return str;
    }
    else {
        return mAccessibilityNodeProvider->FormatNumberWithLocale(value);
    }
}

void NumberPicker::ValidateInputTextView(
    /* [in] */ IView* v)
{
    AutoPtr<ICharSequence> charSeq;
    (ITextView::Probe(v))->GetText((ICharSequence**)&charSeq);
    String str = String(NULL);
    charSeq->ToString(&str);
    if (str.IsEmpty()) {
        UpdateInputTextView();
    }
    else {
        Int32 current = GetSelectedPos(str);
        SetValueInternal(current, TRUE);
    }
}

Boolean NumberPicker::UpdateInputTextView()
{
    String text = String(NULL);
    if (!mDisplayedValues) {
        text = FormatNumber(mValue);
    }
    else {
        text = (*mDisplayedValues)[mValue - mMinValue];
    }
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(text);
    AutoPtr<ICharSequence> itext;
    ITextView::Probe(mInputText)->GetText((ICharSequence**)&itext);
    String inputText = String(NULL);
    itext->ToString(&inputText);
    if (!TextUtils::IsEmpty(csq) && !text.Equals(inputText)) {
        ITextView::Probe(mInputText)->SetText(csq);
        return TRUE;
    }
    return FALSE;
}

void NumberPicker::NotifyChange(
    /* [in] */ Int32 previous,
    /* [in] */ Int32 current)
{
    if (mOnValueChangeListener) {
        mOnValueChangeListener->OnValueChange(this, previous, mValue);
    }
}

void NumberPicker::PostChangeCurrentByOneFromLongPress(
    /* [in] */ Boolean increment,
    /* [in] */ Int64 delayMillis)
{
    Boolean res;
    if (!mChangeCurrentByOneFromLongPressCommand) {
        mChangeCurrentByOneFromLongPressCommand = new ChangeCurrentByOneFromLongPressCommand(this);
    }
    else {
        RemoveCallbacks(mChangeCurrentByOneFromLongPressCommand, &res);
    }
    mChangeCurrentByOneFromLongPressCommand->SetStep(increment);
    PostDelayed(mChangeCurrentByOneFromLongPressCommand, delayMillis, &res);
}

void NumberPicker::RemoveChangeCurrentByOneFromLongPress()
{
    if (mChangeCurrentByOneFromLongPressCommand) {
        Boolean res;
        RemoveCallbacks(mChangeCurrentByOneFromLongPressCommand, &res);
    }
}

void NumberPicker::PostBeginSoftInputOnLongPressCommand()
{
    Boolean res;
    if (!mBeginSoftInputOnLongPressCommand) {
        mBeginSoftInputOnLongPressCommand = new BeginSoftInputOnLongPressCommand(this);
    }
    else {
        RemoveCallbacks(mBeginSoftInputOnLongPressCommand, &res);
    }
    PostDelayed(mBeginSoftInputOnLongPressCommand, CViewConfiguration::GetLongPressTimeout(), &res);
}

void NumberPicker::RemoveBeginSoftInputCommand()
{
    if (mBeginSoftInputOnLongPressCommand) {
        Boolean res;
        RemoveCallbacks(mBeginSoftInputOnLongPressCommand, &res);
    }
}

void NumberPicker::RemoveAllCallbacks()
{
    Boolean res;
    if (mChangeCurrentByOneFromLongPressCommand) {
        RemoveCallbacks(mChangeCurrentByOneFromLongPressCommand, &res);
    }
    if (mSetSelectionCommand) {
        RemoveCallbacks(mSetSelectionCommand, &res);
    }
    if (mBeginSoftInputOnLongPressCommand) {
        RemoveCallbacks(mBeginSoftInputOnLongPressCommand, &res);
    }

    mPressedStateHelper->Cancel();
}

Int32 NumberPicker::GetSelectedPos(
    /* [in] */ const String& value)
{
    if (!mDisplayedValues) {
        //try{
            return StringUtils::ParseInt32(value);
        //} catch{

        //}
    }
    else {
        String str;
        for (Int32 i = 0; i < (*mDisplayedValues).GetLength(); i++) {
            str = (*mDisplayedValues)[i];
            if (str.StartWithIgnoreCase(value)) {
                return mMinValue + i;
            }
        }
        //try{
            return StringUtils::ParseInt32(value);
        //} catch{

        //}
    }
    return mMinValue;
}

void NumberPicker::PostSetSelectionCommand(
    /* [in] */ Int32 selectionStart,
    /* [in] */ Int64 selectionEnd)
{
    Boolean res;
    if (!mSetSelectionCommand) {
        mSetSelectionCommand = new SetSelectionCommand(this);
    }
    else {
        RemoveCallbacks(mSetSelectionCommand, &res);
    }
    mSetSelectionCommand->mSelectionStart = selectionStart;
    mSetSelectionCommand->mSelectionEnd = selectionEnd;
    Post(mSetSelectionCommand, &res);
}

Boolean NumberPicker::EnsureScrollWheelAdjusted()
{
    Int32 deltaY = mInitialScrollOffset - mCurrentScrollOffset;
    if (deltaY != 0) {
        mPreviousScrollerY = 0;
        if (Elastos::Core::Math::Abs(deltaY) > mSelectorElementHeight / 2) {
            deltaY += (deltaY > 0) ? -mSelectorElementHeight : mSelectorElementHeight;
        }
        mAdjustScroller->StartScroll(0, 0, 0, deltaY, SELECTOR_ADJUSTMENT_DURATION_MILLIS);
        Invalidate();
        return TRUE;
    }
    return FALSE;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
