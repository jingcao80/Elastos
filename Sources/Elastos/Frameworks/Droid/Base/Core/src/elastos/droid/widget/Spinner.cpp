
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/Spinner.h"
#include "elastos/droid/widget/CSpinnerSavedState.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

//==========================================================================
//                       Spinner::DropDownAdapter
//==========================================================================
CAR_INTERFACE_IMPL_3(Spinner::DropDownAdapter, Object, IListAdapter, IAdapter, ISpinnerAdapter);
Spinner::DropDownAdapter::DropDownAdapter(
    /* [in] */ ISpinnerAdapter* adapter)
    : mAdapter(adapter)
{
    AutoPtr<IListAdapter> la = IListAdapter::Probe(adapter);
    if (la) {
        mListAdapter = la;
    }
}

ECode Spinner::DropDownAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 0;

    if (mAdapter) {
        IAdapter::Probe(mAdapter)->GetCount(count);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    *item = NULL;

    if (mAdapter) {
        IAdapter::Probe(mAdapter)->GetItem(position, item);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = -1;

    if (mAdapter) {
        IAdapter::Probe(mAdapter)->GetItemId(position, itemId);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return GetDropDownView(position, convertView, parent, view);
}

ECode Spinner::DropDownAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;

    if (mAdapter) {
        mAdapter->GetDropDownView(position, convertView, parent, view);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = FALSE;

    if (mAdapter) {
        IAdapter::Probe(mAdapter)->HasStableIds(hasStableIds);
    }

    return NOERROR;
}

ECode Spinner::DropDownAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    if (mAdapter) {
        IAdapter::Probe(mAdapter)->RegisterDataSetObserver(observer);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    if (mAdapter) {
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(observer);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;

    if (mListAdapter) {
        return mListAdapter->AreAllItemsEnabled(enabled);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;

    if (mListAdapter) {
        return mListAdapter->IsEnabled(position, enabled);
    }
    return NOERROR;
}

ECode Spinner::DropDownAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);
    *viewType = 0;
    return NOERROR;
}

ECode Spinner::DropDownAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 1;
    return NOERROR;
}

ECode Spinner::DropDownAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    Int32 count = 0;
    GetCount(&count);
    *isEmpty = count == 0;
    return NOERROR;
}

//==========================================================================
//                       Spinner::DialogPopup
//==========================================================================
CAR_INTERFACE_IMPL_2(Spinner::DialogPopup, Object, ISpinnerPopup, IDialogInterfaceOnClickListener);
Spinner::DialogPopup::DialogPopup(
    /* [in] */ Spinner* host)
    : mHost(host)
{
}

ECode Spinner::DialogPopup::Dismiss()
{
    if (mPopup != NULL) {
        IDialogInterface::Probe(mPopup)->Dismiss();
        mPopup = NULL;
    }
    return NOERROR;
}

ECode Spinner::DialogPopup::IsShowing(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    if (mPopup) {
        return IDialog::Probe(mPopup)->IsShowing(res);
    }
    return NOERROR;
}

ECode Spinner::DialogPopup::SetAdapter(
    /* [in] */ IListAdapter* adapter)
{
    mListAdapter = adapter;
    return NOERROR;
}

ECode Spinner::DialogPopup::SetPromptText(
    /* [in] */ ICharSequence* hintText)
{
    mPrompt = hintText;
    return NOERROR;
}

ECode Spinner::DialogPopup::GetHintText(
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    *csq = mPrompt;
    REFCOUNT_ADD(*csq);
    return NOERROR;
}

ECode Spinner::DialogPopup::Show(
    /* [in] */ Int32 textDirection,
    /* [in] */ Int32 textAlignment)
{
    if (mListAdapter == NULL) {
        return NOERROR;
    }

    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(context, (IAlertDialogBuilder**)&builder);
    if (mPrompt) {
        builder->SetTitle(mPrompt);
    }

    Int32 position = 0;
    mHost->GetSelectedItemPosition(&position);
    builder->SetSingleChoiceItems(mListAdapter, position, this);
    builder->Create((IAlertDialog**)&mPopup);
    AutoPtr<IListView> listView;
    mPopup->GetListView((IListView**)&listView);
    IView::Probe(listView)->SetTextDirection(textDirection);
    IView::Probe(listView)->SetTextAlignment(textAlignment);
    IDialog::Probe(mPopup)->Show();
    return NOERROR;
}

ECode Spinner::DialogPopup::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->SetSelection(which);
    if (mHost->mOnItemClickListener) {
        Int64 id = 0;
        IAdapter::Probe(mListAdapter)->GetItemId(which, &id);
        Boolean result = FALSE;
        mHost->PerformItemClick(NULL, which, id, &result);
    }
    Dismiss();
    return NOERROR;
}

ECode Spinner::DialogPopup::SetBackgroundDrawable(
    /* [in] */ IDrawable* bg)
{
    Logger::E(TAG, "Cannot set popup background for MODE_DIALOG, ignoring");
    return NOERROR;
}

ECode Spinner::DialogPopup::SetVerticalOffset(
    /* [in] */ Int32 px)
{
    Logger::E(TAG, "Cannot set vertical offset for MODE_DIALOG, ignoring");
    return NOERROR;
}

ECode Spinner::DialogPopup::SetHorizontalOffset(
    /* [in] */ Int32 px)
{
    Logger::E(TAG, "Cannot set horizontal offset for MODE_DIALOG, ignoring");
    return NOERROR;
}

ECode Spinner::DialogPopup::GetBackground(
    /* [out] */ IDrawable** d)
{
    VALIDATE_NOT_NULL(d);
    *d = NULL;
    return NOERROR;
}

ECode Spinner::DialogPopup::GetVerticalOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = 0;
    return NOERROR;
}

ECode Spinner::DialogPopup::GetHorizontalOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = 0;
    return NOERROR;
}

//==========================================================================
//                       Spinner::ItemClickListener
//==========================================================================
CAR_INTERFACE_IMPL(Spinner::ItemClickListener, Object, IAdapterViewOnItemClickListener);
Spinner::ItemClickListener::ItemClickListener(
    /* [in] */ Spinner* spinnerHost,
    /* [in] */ DropdownPopup* popupHost)
    : mSpinnerHost(spinnerHost)
    , mPopupHost(popupHost)
{
}

ECode Spinner::ItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    mSpinnerHost->SetSelection(position);
    if (mSpinnerHost->mOnItemClickListener != NULL) {
        Int64 id = 0;
        IAdapter::Probe(mPopupHost->mAdapter)->GetItemId(position, &id);
        Boolean tmp = FALSE;
        mSpinnerHost->PerformItemClick(view, position, id, &tmp);
    }
    mPopupHost->Dismiss();
    return NOERROR;
}

//==========================================================================
//                       Spinner::GlobalLayoutListener
//==========================================================================
CAR_INTERFACE_IMPL(Spinner::GlobalLayoutListener, Object, IOnGlobalLayoutListener);
Spinner::GlobalLayoutListener::GlobalLayoutListener(
    /* [in] */ Spinner* spinnerHost,
    /* [in] */ DropdownPopup* popupHost)
    : mSpinnerHost(spinnerHost)
    , mPopupHost(popupHost)
{
}

ECode Spinner::GlobalLayoutListener::OnGlobalLayout()
{
    if (!mSpinnerHost->IsVisibleToUser()) {
        mPopupHost->Dismiss();
    }
    else {
        mPopupHost->ComputeContentWidth();

        // Use super.show here to update; we don't want to move the selected
        // position or adjust other things that would be reset otherwise.
        // DropdownPopup.super.show();
        mPopupHost->ListPopupWindow::Show();
    }

    return NOERROR;
}

//==========================================================================
//                       Spinner::GlobalLayoutListener2
//==========================================================================
CAR_INTERFACE_IMPL(Spinner::GlobalLayoutListener2, Object, IOnGlobalLayoutListener);
Spinner::GlobalLayoutListener2::GlobalLayoutListener2(
    /* [in] */ Spinner* host)
    : mHost(host)
{}

ECode Spinner::GlobalLayoutListener2::OnGlobalLayout()
{
    Boolean showing = FALSE;
    if (mHost->mPopup->IsShowing(&showing), !showing) {
        Int32 dir = 0, align = 0;
        mHost->GetTextDirection(&dir);
        mHost->GetTextAlignment(&align);
        mHost->mPopup->Show(dir, align);
    }
    AutoPtr<IViewTreeObserver> vto;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
    if (vto != NULL) {
        vto->RemoveOnGlobalLayoutListener(this);
    }
    return NOERROR;
}

//==========================================================================
//                       Spinner::DismissListener
//==========================================================================
CAR_INTERFACE_IMPL(Spinner::DismissListener, Object, IPopupWindowOnDismissListener);
Spinner::DismissListener::DismissListener(
    /* [in] */ Spinner* spinnerHost,
    /* [in] */ GlobalLayoutListener* layoutListener)
    : mSpinnerHost(spinnerHost)
    , mLayoutListener(layoutListener)
{
}

ECode Spinner::DismissListener::OnDismiss()
{
    AutoPtr<IViewTreeObserver> vto;
    mSpinnerHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
    if (vto) {
        vto->RemoveGlobalOnLayoutListener(mLayoutListener);
    }
    return NOERROR;
}

//==========================================================================
//                       Spinner::DropdownPopup
//==========================================================================
CAR_INTERFACE_IMPL_2(Spinner::DropdownPopup, ListPopupWindow, IDropdownPopup, ISpinnerPopup);
Spinner::DropdownPopup::DropdownPopup(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [in] */ Spinner* host)
    : mHost(host)
{
    ListPopupWindow::constructor(context, attrs, defStyleAttr, defStyleRes);
    SetAnchorView((IView*)(mHost->Probe(EIID_IView)));
    SetModal(TRUE);
    SetPromptPosition(IListPopupWindow::POSITION_PROMPT_ABOVE);
    AutoPtr<IAdapterViewOnItemClickListener> listener = new ItemClickListener(mHost, this);
    SetOnItemClickListener(listener);
}

ECode Spinner::DropdownPopup::SetAdapter(
    /* [in] */ IListAdapter* adapter)
{
    ListPopupWindow::SetAdapter(adapter);
    mAdapter = adapter;
    return NOERROR;
}

ECode Spinner::DropdownPopup::GetHintText(
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    *csq = mHintText;
    REFCOUNT_ADD(*csq);
    return NOERROR;
}

ECode Spinner::DropdownPopup::SetPromptText(
    /* [in] */ ICharSequence* hintText)
{
    // Hint text is ignored for dropdowns, but maintain it here.
    mHintText = hintText;
    return NOERROR;
}

void Spinner::DropdownPopup::ComputeContentWidth()
{
    AutoPtr<IDrawable> background;
    mHost->GetBackground((IDrawable**)&background);
    Int32 hOffset = 0;
    Boolean isLayoutRtl = FALSE;
    if (background != NULL) {
        Boolean isPadding = FALSE;
        background->GetPadding(mHost->mTempRect, &isPadding);
        mHost->IsLayoutRtl(&isLayoutRtl);
        if (isLayoutRtl) {
            mHost->mTempRect->GetRight(&hOffset);
        }
        else {
            mHost->mTempRect->GetLeft(&hOffset);
            hOffset = -hOffset;
        }
    }
    else {
        mHost->mTempRect->SetLeft(0);
        mHost->mTempRect->SetRight(0);
    }

    Int32 spinnerPaddingLeft = 0;
    mHost->GetPaddingLeft(&spinnerPaddingLeft);
    Int32 spinnerPaddingRight = 0;
    mHost->GetPaddingRight(&spinnerPaddingRight);
    Int32 spinnerWidth = 0;
    mHost->GetWidth(&spinnerWidth);

    if (mHost->mDropDownWidth == IListPopupWindow::WRAP_CONTENT) {
        AutoPtr<IDrawable> drawable;
        mHost->GetBackground((IDrawable**)&drawable);
        Int32 contentWidth = mHost->MeasureContentWidth(
                ISpinnerAdapter::Probe(mAdapter), drawable);

        AutoPtr<IResources> res;
        mHost->mContext->GetResources((IResources**)&res);
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        Int32 widthPixels = 0;
        dm->GetWidthPixels(&widthPixels);
        Int32 left = 0, right = 0;
        mHost->mTempRect->GetLeft(&left);
        mHost->mTempRect->GetRight(&right);
        Int32 contentWidthLimit = widthPixels - left - right;
        if (contentWidth > contentWidthLimit) {
            contentWidth = contentWidthLimit;
        }
        SetContentWidth(Elastos::Core::Math::Max(
               contentWidth, spinnerWidth - spinnerPaddingLeft - spinnerPaddingRight));
    }
    else if (mHost->mDropDownWidth == IListPopupWindow::MATCH_PARENT) {
        SetContentWidth(spinnerWidth - spinnerPaddingLeft - spinnerPaddingRight);
    }
    else {
        SetContentWidth(mHost->mDropDownWidth);
    }

    if (mHost->IsLayoutRtl(&isLayoutRtl), isLayoutRtl) {
        Int32 width = 0;
        GetWidth(&width);
        hOffset += spinnerWidth - spinnerPaddingRight - width;
    }
    else {
        hOffset += spinnerPaddingLeft;
    }
    ListPopupWindow::SetHorizontalOffset(hOffset);
}

ECode Spinner::DropdownPopup::Show(
    /* [in] */ Int32 textDirection,
    /* [in] */ Int32 textAlignment)
{
    Boolean wasShowing = FALSE;
    IsShowing(&wasShowing);

    ComputeContentWidth();

    SetInputMethodMode(IListPopupWindow::INPUT_METHOD_NOT_NEEDED);
    ListPopupWindow::Show();
    AutoPtr<IListView> listView;
    GetListView((IListView**) &listView);
    IAbsListView::Probe(listView)->SetChoiceMode(IAbsListView::CHOICE_MODE_SINGLE);
    IView::Probe(listView)->SetTextDirection(textDirection);
    IView::Probe(listView)->SetTextAlignment(textAlignment);
    Int32 position = 0;
    mHost->GetSelectedItemPosition(&position);
    SetSelection(position);

    if (wasShowing) {
        // Skip setting up the layout/dismiss listener below. If we were previously
        // showing it will still stick around.
        return NOERROR;
    }

    // Make sure we hide if our anchor goes away.
    // TODO: This might be appropriate to push all the way down to PopupWindow,
    // but it may have other side effects to investigate first. (Text editing handles, etc.)
    AutoPtr<IViewTreeObserver> vto;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
    if (vto != NULL) {
        AutoPtr<GlobalLayoutListener> layoutListener = new GlobalLayoutListener(mHost, this);
        vto->AddOnGlobalLayoutListener(layoutListener);
        AutoPtr<IPopupWindowOnDismissListener> dismiss = new DismissListener(mHost, layoutListener);
        SetOnDismissListener(dismiss);
    }
    return NOERROR;
}

ECode Spinner::DropdownPopup::Dismiss()
{
    return ListPopupWindow::Dismiss();
}

ECode Spinner::DropdownPopup::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ListPopupWindow::IsShowing(result);
}

ECode Spinner::DropdownPopup::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    return ListPopupWindow::SetBackgroundDrawable(d);
}

ECode Spinner::DropdownPopup::SetVerticalOffset(
    /* [in] */ Int32 offset)
{
    return ListPopupWindow::SetVerticalOffset(offset);
}

ECode Spinner::DropdownPopup::SetHorizontalOffset(
    /* [in] */ Int32 offset)
{
    return ListPopupWindow::SetHorizontalOffset(offset);
}

ECode Spinner::DropdownPopup::GetBackground(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result);
    return ListPopupWindow::GetBackground(result);
}

ECode Spinner::DropdownPopup::GetVerticalOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return ListPopupWindow::GetVerticalOffset(result);
}

ECode Spinner::DropdownPopup::GetHorizontalOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return ListPopupWindow::GetHorizontalOffset(result);
}

//==========================================================================
//                       Spinner::SpinnerForwardingListener
//==========================================================================
Spinner::SpinnerForwardingListener::SpinnerForwardingListener(
    /* [in] */ DropdownPopup* popup,
    /* [in] */ Spinner* host)
    : mData(popup)
    , mHost(host)
{
    ForwardingListener::constructor((IView*)host->Probe(EIID_IView));
}

ECode Spinner::SpinnerForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** window)
{
    VALIDATE_NOT_NULL(window);
    *window = (IListPopupWindow*)mData->Probe(EIID_IListPopupWindow);
    REFCOUNT_ADD(*window);
    return NOERROR;
}

ECode Spinner::SpinnerForwardingListener::OnForwardingStarted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean showing = FALSE;
    if (mHost->mPopup->IsShowing(&showing), !showing) {
        Int32 dir = 0, align = 0;
        mHost->GetTextDirection(&dir);
        mHost->GetTextAlignment(&align);
        mHost->mPopup->Show(dir, align);
    }
    *result = TRUE;
    return NOERROR;
}

//==========================================================================
//                       Spinner::SavedState
//==========================================================================
CAR_INTERFACE_IMPL(Spinner::SavedState, AbsSpinner::SavedState, ISpinnerSavedState);
Spinner::SavedState::SavedState()
    : mShowDropdown(FALSE)
{}

ECode Spinner::SavedState::constructor()
{
    return NOERROR;
}

ECode Spinner::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return AbsSpinner::SavedState::constructor(superState);
}

ECode Spinner::SavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    AbsSpinner::SavedState::WriteToParcel(out);
    out->WriteBoolean(mShowDropdown);
    return NOERROR;
}

ECode Spinner::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AbsSpinner::SavedState::ReadFromParcel(source);
    source->ReadBoolean(&mShowDropdown);
    return NOERROR;
}

const Int32 Spinner::MAX_ITEMS_MEASURED = 15;
const Int32 Spinner::MODE_DIALOG = 0;
const Int32 Spinner::MODE_DROPDOWN = 1;
const Int32 Spinner::MODE_THEME = -1;
const String Spinner::TAG("Spinner");

CAR_INTERFACE_IMPL(Spinner, AbsSpinner, ISpinner);
Spinner::Spinner()
    : mDropDownWidth(0)
    , mGravity(0)
    , mDisableChildrenWhenDisabled(FALSE)
{
    CRect::New((IRect**)&mTempRect);
}

ECode Spinner::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, (IAttributeSet*)NULL);
}

ECode Spinner::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 mode)
{
    return constructor(context, NULL, R::attr::spinnerStyle, mode);
}

ECode Spinner::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::spinnerStyle);
}

ECode Spinner::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0, MODE_THEME);
}

ECode Spinner::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 mode)
{
    return constructor(context, attrs, defStyleAttr, 0, mode);
}

ECode Spinner::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [in] */ Int32 mode)
{
    AbsSpinner::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::Spinner),
            ArraySize(R::styleable::Spinner));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    if (mode == MODE_THEME) {
        a->GetInt32(R::styleable::Spinner_spinnerMode, MODE_DIALOG, &mode);
    }

    switch (mode) {
        case MODE_DIALOG: {
            mPopup = new DialogPopup(this);
            break;
        }

        case MODE_DROPDOWN: {
            AutoPtr<DropdownPopup> popup = new DropdownPopup(context, attrs, defStyleAttr, defStyleRes, this);
            a->GetLayoutDimension(R::styleable::Spinner_dropDownWidth, IViewGroupLayoutParams::WRAP_CONTENT, &mDropDownWidth);
            AutoPtr<IDrawable> d;
            a->GetDrawable(R::styleable::Spinner_popupBackground, (IDrawable**)&d);
            popup->SetBackgroundDrawable(d);
            mPopup = popup;
            mForwardingListener = new SpinnerForwardingListener(popup, this);
            break;
        }
    }

    a->GetInt32(R::styleable::Spinner_gravity, IGravity::CENTER, &mGravity);
    String prompt;
    a->GetString(R::styleable::Spinner_prompt, &prompt);
    AutoPtr<ICharSequence> cprompt;
    CString::New(prompt, (ICharSequence**)&cprompt);
    mPopup->SetPromptText(cprompt);
    a->GetBoolean(R::styleable::Spinner_disableChildrenWhenDisabled, FALSE, &mDisableChildrenWhenDisabled);

    a->Recycle();

    if (mTempAdapter) {
        mPopup->SetAdapter(mTempAdapter);
        mTempAdapter = NULL;
    }

    return NOERROR;
}

ECode Spinner::SetPopupBackgroundDrawable(
    /* [in] */ IDrawable* background)
{
    if (!IDropdownPopup::Probe(mPopup)) {
        return NOERROR;
    }
    mPopup->SetBackgroundDrawable(background);
    return NOERROR;
}

ECode Spinner::SetPopupBackgroundResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(resId, (IDrawable**)&drawable);
    SetPopupBackgroundDrawable(drawable);
    return NOERROR;
}

ECode Spinner::GetPopupBackground(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return mPopup->GetBackground(drawable);
}

ECode Spinner::SetDropDownVerticalOffset(
    /* [in] */ Int32 pixels)
{
    return mPopup->SetVerticalOffset(pixels);
}

ECode Spinner::GetDropDownVerticalOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    return mPopup->GetVerticalOffset(offset);
}

ECode Spinner::SetDropDownHorizontalOffset(
    /* [in] */ Int32 pixels)
{
    return mPopup->SetHorizontalOffset(pixels);
}

ECode Spinner::GetDropDownHorizontalOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    return mPopup->GetHorizontalOffset(offset);
}

ECode Spinner::SetDropDownWidth(
    /* [in] */ Int32 pixels)
{
    if (!IDropdownPopup::Probe(mPopup)) {
        return NOERROR;
    }
    mDropDownWidth = pixels;
    return NOERROR;
}

ECode Spinner::GetDropDownWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mDropDownWidth;
    return NOERROR;
}

ECode Spinner::SetEnabled(
    /* [in] */ Boolean enabled)
{
    AbsSpinner::SetEnabled(enabled);
    if (mDisableChildrenWhenDisabled) {
        Int32 count = 0;
        GetChildCount(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            child->SetEnabled(enabled);
        }
    }
    return NOERROR;
}

ECode Spinner::SetGravity(
    /* [in] */ Int32 gravity)
{
    if (mGravity != gravity) {
        if ((gravity & IGravity::HORIZONTAL_GRAVITY_MASK) == 0) {
            gravity |= IGravity::START;
        }
        mGravity = gravity;
        RequestLayout();
    }
    return NOERROR;
}

ECode Spinner::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mGravity;
    return NOERROR;
}

ECode Spinner::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    AutoPtr<ISpinnerAdapter> sa = ISpinnerAdapter::Probe(adapter);
    if (sa == NULL) {
        return NOERROR;
    }

    AbsSpinner::SetAdapter(IAdapter::Probe(sa));
    mRecycler->Clear();

    Int32 targetSdkVersion = 0, count = 0;
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);
    info->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion >= Build::VERSION_CODES::LOLLIPOP
            && adapter != NULL && (adapter->GetViewTypeCount(&count), count) != 1) {
        // throw new IllegalArgumentException("Spinner adapter view type count must be 1");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mPopup) {
        AutoPtr<IListAdapter> ad = new DropDownAdapter(sa);
        mPopup->SetAdapter(ad);
    }
    else {
        mTempAdapter = new DropDownAdapter(sa);
    }
    return NOERROR;
}

ECode Spinner::GetBaseline(
    /* [out] */ Int32* baseline)
{
    VALIDATE_NOT_NULL(baseline);
    *baseline = -1;

    AutoPtr<IView> child;
    Int32 count = 0, cc = 0;
    IAdapter::Probe(mAdapter)->GetCount(&count);
    if ((GetChildCount(&cc), cc) > 0) {
        GetChildAt(0, (IView**)&child);
    }
    else if (mAdapter && count > 0) {
        child = MakeView(0, FALSE);
        mRecycler->Put(0, child);
    }

    if (child) {
        Int32 childBaseline = 0;
        child->GetBaseline(&childBaseline);
        Int32 top = 0;
        child->GetTop(&top);
        if (childBaseline >= 0) {
            *baseline = top + childBaseline;
        }
    }

    return NOERROR;
}

ECode Spinner::SetOnItemClickListener(
    /* [in] */ IAdapterViewOnItemClickListener* l)
{
    //throw new RuntimeException("setOnItemClickListener cannot be used with a spinner.");
    return E_RUNTIME_EXCEPTION;
}

ECode Spinner::SetOnItemClickListenerInt(
    /* [in] */ IAdapterViewOnItemClickListener* l)
{
    return AbsSpinner::SetOnItemClickListener(l);
}

ECode Spinner::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mForwardingListener.Get() != NULL && (mForwardingListener->OnTouch(this, event, result), *result)) {
        *result = TRUE;
        return NOERROR;
    }

    return AbsSpinner::OnTouchEvent(event, result);
}

ECode Spinner::PerformClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean handled = FALSE;
    AbsSpinner::PerformClick(&handled);
    if (!handled) {
        handled = TRUE;
        Boolean show = FALSE;
        mPopup->IsShowing(&show);
        if (!show) {
            Int32 dir = 0, align = 0;
            GetTextDirection(&dir);
            GetTextAlignment(&align);
            mPopup->Show(dir, align);
        }
    }
    *result = handled;
    return NOERROR;
}

ECode Spinner::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    SetSelection(which);
    dialog->Dismiss();
    return NOERROR;
}

ECode Spinner::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AbsSpinner::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CSpinner"), (ICharSequence**)&seq);
    return IAccessibilityRecord::Probe(event)->SetClassName(seq);
}

ECode Spinner::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsSpinner::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CSpinner"), (ICharSequence**)&seq);
    info->SetClassName(seq);

    if (mAdapter != NULL) {
        info->SetCanOpenPopup(TRUE);
    }
    return NOERROR;
}

ECode Spinner::SetPrompt(
    /* [in] */ ICharSequence* prompt)
{
    return mPopup->SetPromptText(prompt);
}

ECode Spinner::SetPromptId(
    /* [in] */ Int32 promptId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ICharSequence> csq;
    context->GetText(promptId, (ICharSequence**)&csq);
    SetPrompt(csq);
    return NOERROR;
}

ECode Spinner::GetPrompt(
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq);
    return mPopup->GetHintText(seq);
}

Int32 Spinner::MeasureContentWidth(
    /* [in] */ ISpinnerAdapter* adapter,
    /* [in] */ IDrawable* background)
{
    if (!adapter) {
        return 0;
    }
    Int32 width = 0;
    AutoPtr<IView> itemView;
    Int32 itemType = 0;
    Int32 widthMeasureSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);
    Int32 heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(0, View::MeasureSpec::UNSPECIFIED);

    Int32 pos = 0;
    GetSelectedItemPosition(&pos);
    Int32 start = Elastos::Core::Math::Max(0, pos);
    Int32 adc = 0;
    IAdapter::Probe(adapter)->GetCount(&adc);
    Int32 end = Elastos::Core::Math::Min(adc, start + MAX_ITEMS_MEASURED);
    Int32 count = end - start;
    start = Elastos::Core::Math::Max(0, start - (MAX_ITEMS_MEASURED - count));

    for (Int32 i = start; i < end; i++) {
        Int32 positionType = 0;
        IAdapter::Probe(adapter)->GetItemViewType(i, &positionType);
        if (positionType != itemType) {
            itemType = positionType;
            itemView = NULL;
        }
        IAdapter::Probe(adapter)->GetView(i, itemView, this, (IView**)&itemView);

        AutoPtr<IViewGroupLayoutParams> lp;
        itemView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        if (!lp) {
            AutoPtr<IViewGroupLayoutParams> params;
            CViewGroupLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT,
                (IViewGroupLayoutParams**)&params);
            itemView->SetLayoutParams(params);
        }
        itemView->Measure(widthMeasureSpec, heightMeasureSpec);
        Int32  mw = 0;
        itemView->GetMeasuredWidth(&mw);
        width = Elastos::Core::Math::Max(width, mw);
    }

    if (background) {
        Boolean res;
        background->GetPadding(mTempRect, &res);
        AutoPtr<CRect> ct = (CRect*)mTempRect.Get();
        width += ct->mLeft + ct->mRight;
    }
    return width;
}

AutoPtr<IParcelable> Spinner::OnSaveInstanceState()
{
    AutoPtr<IParcelable> ss;
    CSpinnerSavedState::New(AbsSpinner::OnSaveInstanceState(), (IParcelable**)&ss);
    Boolean tmp = FALSE;
    ((CSpinnerSavedState*)ss.Get())->mShowDropdown = mPopup != NULL && (mPopup->IsShowing(&tmp), tmp);
    return ss;
}

void Spinner::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<SavedState> ss = (SavedState*) state;

    AutoPtr<IParcelable> s;
    ss->GetSuperState((IParcelable**)&s);
    AbsSpinner::OnRestoreInstanceState(s);

    if (ss->mShowDropdown) {
        AutoPtr<IViewTreeObserver> vto;
        GetViewTreeObserver((IViewTreeObserver**)&vto);
        if (vto != NULL) {
            AutoPtr<IOnGlobalLayoutListener> listener = new GlobalLayoutListener2(this);
            vto->AddOnGlobalLayoutListener(listener);
        }
    }
}

ECode Spinner::OnDetachedFromWindow()
{
    AbsSpinner::OnDetachedFromWindow();

    Boolean show = FALSE;
    mPopup->IsShowing(&show);
    if (mPopup && show) {
        mPopup->Dismiss();
    }
    return NOERROR;
}

void Spinner::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AbsSpinner::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    if (mPopup && View::MeasureSpec::GetMode(widthMeasureSpec) == View::MeasureSpec::AT_MOST) {
        Int32 measuredWidth = 0, measuredHeight = 0;
        GetMeasuredWidth(&measuredWidth);
        GetMeasuredHeight(&measuredHeight);
        AutoPtr<IAdapter> a;
        GetAdapter((IAdapter**)&a);
        AutoPtr<ISpinnerAdapter> adapter = ISpinnerAdapter::Probe(a);

        AutoPtr<IDrawable> drawable;
        GetBackground((IDrawable**)&drawable);
        SetMeasuredDimension(Elastos::Core::Math::Min(Elastos::Core::Math::Max(
            measuredWidth, MeasureContentWidth(adapter, drawable)),
            MeasureSpec::GetSize(widthMeasureSpec)), measuredHeight);
    }
}

ECode Spinner::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    AbsSpinner::OnLayout(changed, l, t, r, b);
    mInLayout = TRUE;
    Layout(0, FALSE);
    mInLayout = FALSE;
    return NOERROR;
}

void Spinner::Layout(
    /* [in] */ Int32 delta,
    /* [in] */ Boolean animate)
{
    Int32 childrenLeft = mSpinnerPadding->mLeft;
    Int32 childrenWidth = mRight - mLeft - mSpinnerPadding->mLeft - mSpinnerPadding->mRight;

    if (mDataChanged) {
        HandleDataChanged();
    }

    if (mItemCount == 0) {
        ResetList();
        return;
    }
     if (mNextSelectedPosition >= 0) {
        SetSelectedPositionInt(mNextSelectedPosition);
    }

    RecycleAllViews();
    RemoveAllViewsInLayout();
    mFirstPosition = mSelectedPosition;

    if (mAdapter != NULL) {
        AutoPtr<IView> sel = MakeView(mSelectedPosition, TRUE);
        Int32 width = 0;
        sel->GetMeasuredWidth(&width);
        Int32 selectedOffset = childrenLeft;
        Int32 layoutDirection = 0;
        GetLayoutDirection(&layoutDirection);
        Int32 absoluteGravity = Gravity::GetAbsoluteGravity(mGravity, layoutDirection);
        switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
            case IGravity::CENTER_HORIZONTAL:
                selectedOffset = childrenLeft + (childrenWidth / 2) - (width / 2);
                break;
            case IGravity::RIGHT:
                selectedOffset = childrenLeft + childrenWidth - width;
                break;
        }

        sel->OffsetLeftAndRight(selectedOffset);
    }

    // Flush any cached views that did not get reused above
    mRecycler->Clear();
    Invalidate();
    CheckSelectionChanged();
    mDataChanged = FALSE;
    mNeedSync = FALSE;
    SetNextSelectedPositionInt(mSelectedPosition);
}

AutoPtr<IView> Spinner::MakeView(
    /* [in] */ Int32 position,
    /* [in] */ Boolean addChild)
{
    AutoPtr<IView> child;
    if (!mDataChanged) {
        child = mRecycler->Get(position);
        if (child) {
            SetUpChild(child, addChild);
            return child;
        }
    }
    child = NULL;
    // Nothing found in the recycler -- ask the adapter for a view
    IAdapter::Probe(mAdapter)->GetView(position, NULL, this, (IView**)&child);

    // Position the view
    SetUpChild(child, addChild);
    return child;
}

void Spinner::SetUpChild(
    /* [in] */ IView* child,
    /* [in] */ Boolean addChild)
{
    assert(child != NULL);

    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    if (!lp) {
        GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&lp);
    }

    if (addChild) {
        AddViewInLayout(child, 0, lp);
    }

    Boolean value = FALSE;
    child->SetSelected((HasFocus(&value), value));
    if (mDisableChildrenWhenDisabled) {
        child->SetEnabled((IsEnabled(&value), value));
    }
    Int32 h = 0;
    lp->GetHeight(&h);
    Int32 childHeightSpec = ViewGroup::GetChildMeasureSpec(mHeightMeasureSpec,
            mSpinnerPadding->mTop + mSpinnerPadding->mBottom, h);
    Int32 w = 0;
    lp->GetWidth(&w);
    Int32 childWidthSpec = ViewGroup::GetChildMeasureSpec(mWidthMeasureSpec,
            mSpinnerPadding->mLeft + mSpinnerPadding->mRight, w);

    child->Measure(childWidthSpec, childHeightSpec);

    Int32 measuredHeight = 0, sh = 0;
    child->GetMeasuredHeight(&measuredHeight);
    Int32 childLeft = 0, childRight = 0;

    GetMeasuredHeight(&sh);
    Int32 childTop = mSpinnerPadding->mTop + ((sh - mSpinnerPadding->mBottom -
        mSpinnerPadding->mTop - measuredHeight) / 2);
    Int32 childBottom = childTop + measuredHeight;
    Int32 width = 0;
    child->GetMeasuredWidth(&width);
    childLeft = 0;
    childRight = childLeft + width;

    child->Layout(childLeft, childTop, childRight, childBottom);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
