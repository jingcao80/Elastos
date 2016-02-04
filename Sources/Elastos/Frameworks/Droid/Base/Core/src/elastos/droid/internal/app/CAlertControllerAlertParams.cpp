
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/app/CAlertControllerAlertParams.h"
#include "elastos/droid/internal/app/CAlertController.h"
#include "elastos/droid/widget/CSimpleCursorAdapter.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::View::EIID_ILayoutInflater;
using Elastos::Droid::Widget::CSimpleCursorAdapter;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::ICheckedTextView;
using Elastos::Droid::Widget::ISimpleCursorAdapter;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

//==============================================================================
//                  CAlertControllerAlertParams::RecycleArrayAdapter
//==============================================================================

ECode CAlertControllerAlertParams::RecycleArrayAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ ArrayOf<IInterface*>* objects,
    /* [in] */ CAlertControllerAlertParams* host,
    /* [in] */ IListView* listView)
{
    FAIL_RETURN(ArrayAdapter::constructor(context, resource, textViewResourceId, objects))
    mAlertParams = host;
    mListView = listView;
    return NOERROR;
}

ECode CAlertControllerAlertParams::RecycleArrayAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    ArrayAdapter::GetView(position, convertView, parent, view);

    if (mAlertParams->mCheckedItems != NULL) {
        Boolean isItemChecked = (*mAlertParams->mCheckedItems)[position];
        if (isItemChecked) {
            IAbsListView::Probe(mListView)->SetItemChecked(position, TRUE);
        }
    }

    return NOERROR;
}

//==============================================================================
//                  CAlertControllerAlertParams::RecycleCursorAdapter
//==============================================================================

ECode CAlertControllerAlertParams::RecycleCursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* c,
    /* [in] */ Boolean autoRequery,
    /* [in] */ CAlertControllerAlertParams* host,
    /* [in] */ IListView* listView,
    /* [in] */ IAlertController* dialog)
{
    FAIL_RETURN(CursorAdapter::constructor(context, c, autoRequery))
    mAlertParams = host;
    mListView = listView;
    mAlertController = dialog;
    InitAdapter();
    return NOERROR;
}

ECode CAlertControllerAlertParams::RecycleCursorAdapter::BindView(
    /* [in] */ IView* view,
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IView> tempView;
    view->FindViewById(R::id::text1, (IView**)&tempView);
    AutoPtr<ICheckedTextView> text = ICheckedTextView::Probe(tempView);
    String str;
    cursor->GetString(mLabelIndex, &str);
    AutoPtr<ICharSequence> csq;
    CString::New(str, (ICharSequence**)&csq);
    ITextView::Probe(text)->SetText(csq);
    Int32 position, checked;
    cursor->GetPosition(&position);
    cursor->GetInt32(mIsCheckedIndex, &checked);
    IAbsListView::Probe(mListView)->SetItemChecked(position, checked == 1);
    return NOERROR;
}

ECode CAlertControllerAlertParams::RecycleCursorAdapter::NewView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    Int32 layout = ((CAlertController*)mAlertController)->mMultiChoiceItemLayout;
    return mAlertParams->mInflater->Inflate(layout, parent, FALSE, view);
}

void CAlertControllerAlertParams::RecycleCursorAdapter::InitAdapter()
{
    AutoPtr<ICursor> cursor;
    GetCursor((ICursor**)&cursor);
    cursor->GetColumnIndexOrThrow(mAlertParams->mLabelColumn, &mLabelIndex);
    cursor->GetColumnIndexOrThrow(mAlertParams->mIsCheckedColumn, &mIsCheckedIndex);
}

//==============================================================================
//                  CAlertControllerAlertParams::AdapterListener
//==============================================================================
CAR_INTERFACE_IMPL(CAlertControllerAlertParams::AdapterListener, Object, IAdapterViewOnItemClickListener)

CAlertControllerAlertParams::AdapterListener::AdapterListener(
    /* [in] */ CAlertControllerAlertParams* params,
    /* [in] */ IAlertController* dialog,
    /* [in] */ IListView* listView,
    /* [in] */ Boolean isClick)
    : mAlertParams(params)
    , mAlertController(dialog)
    , mListView(listView)
    , mIsClick(isClick)
{
}

ECode CAlertControllerAlertParams::AdapterListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    CAlertController* dialog = (CAlertController*)mAlertController.Get();
    if (mIsClick) {
        mAlertParams->mOnClickListener->OnClick(dialog->mDialogInterface, position);
        if (!mAlertParams->mIsSingleChoice) {
            dialog->mDialogInterface->Dismiss();
        }
    }
    else {
        Boolean checked = FALSE;
        IAbsListView::Probe(mListView)->IsItemChecked(position, &checked);

        if (mAlertParams->mCheckedItems != NULL) {
            (*mAlertParams->mCheckedItems)[position] = checked;
        }

        mAlertParams->mOnCheckboxClickListener->OnClick(
            dialog->mDialogInterface, position, checked);
    }
    return NOERROR;
}

//==============================================================================
//                  CAlertControllerAlertParams::CheckedItemAdapter
//==============================================================================

ECode CAlertControllerAlertParams::CheckedItemAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource,
    /* [in] */ Int32 textViewResourceId,
    /* [in] */ ArrayOf<ICharSequence*>* objects)
{
    return ArrayAdapter::constructor(context, resource, textViewResourceId, (ArrayOf<IInterface*>*)objects);
}

// @Override
ECode CAlertControllerAlertParams::CheckedItemAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

// @Override
ECode CAlertControllerAlertParams::CheckedItemAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = position;
    return NOERROR;
}

//==============================================================================
//                  CAlertControllerAlertParams
//==============================================================================

CAR_INTERFACE_IMPL(CAlertControllerAlertParams, Object, IAlertControllerAlertParams)

CAR_OBJECT_IMPL(CAlertControllerAlertParams)

CAlertControllerAlertParams::CAlertControllerAlertParams()
    : mIconId(0)
    , mIconAttrId(0)
    , mCancelable(FALSE)
    , mViewLayoutResId(0)
    , mViewSpacingLeft(0)
    , mViewSpacingTop(0)
    , mViewSpacingRight(0)
    , mViewSpacingBottom(0)
    , mViewSpacingSpecified(FALSE)
    , mIsMultiChoice(FALSE)
    , mIsSingleChoice(FALSE)
    , mCheckedItem(-1)
    , mForceInverseBackground(FALSE)
    , mRecycleOnMeasure(TRUE)
{}

ECode CAlertControllerAlertParams::SetContext(
    /* [in] */ IContext* cxt)
{
    mContext = cxt;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetContext(
    /* [out] */ IContext** cxt)
{
    VALIDATE_NOT_NULL(cxt);
    *cxt = mContext;
    REFCOUNT_ADD(*cxt);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetInflater(
    /* [in] */ ILayoutInflater* inflater)
{
    mInflater = inflater;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    *inflater = mInflater;
    REFCOUNT_ADD(*inflater);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetIconId(
    /* [in] */ Int32 iconId)
{
    mIconId = iconId;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetIconId(
    /* [out] */ Int32* iconId)
{
    VALIDATE_NOT_NULL(iconId);
    *iconId = mIconId;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    REFCOUNT_ADD(*icon);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetIconAttrId(
    /* [in] */ Int32 id)
{
    mIconAttrId = id;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetIconAttrId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mIconAttrId;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    *title = mTitle;
    REFCOUNT_ADD(*title);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetCustomTitleView(
    /* [in] */ IView* view)
{
    mCustomTitleView = view;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetCustomTitleView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mCustomTitleView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetMessage(
    /* [in] */ ICharSequence* message)
{
    mMessage = message;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetMessage(
    /* [out] */ ICharSequence** message)
{
    VALIDATE_NOT_NULL(message);
    *message = mMessage;
    REFCOUNT_ADD(*message);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetPositiveButtonText(
    /* [in] */ ICharSequence* text)
{
    mPositiveButtonText = text;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetPositiveButtonText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mPositiveButtonText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetPositiveButtonListener(
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mPositiveButtonListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetPositiveButtonListener(
    /* [out] */ IDialogInterfaceOnClickListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mPositiveButtonListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetNegativeButtonText(
    /* [in] */ ICharSequence* text)
{
    mNegativeButtonText = text;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetNegativeButtonText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mNegativeButtonText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetNegativeButtonListener(
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mNegativeButtonListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetNegativeButtonListener(
    /* [out] */ IDialogInterfaceOnClickListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mNegativeButtonListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetNeutralButtonText(
    /* [in] */ ICharSequence* text)
{
    mNeutralButtonText = text;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetNeutralButtonText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mNeutralButtonText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetNeutralButtonListener(
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mNeutralButtonListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetNeutralButtonListener(
    /* [out] */ IDialogInterfaceOnClickListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mNeutralButtonListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetCancelable(
    /* [in] */ Boolean cancelable)
{
    mCancelable = cancelable;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetCancelable(
    /* [out] */ Boolean* cancelable)
{
    VALIDATE_NOT_NULL(cancelable);
    *cancelable = mCancelable;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetOnCancelListener(
    /* [in] */ IDialogInterfaceOnCancelListener* listener)
{
    mOnCancelListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetOnCancelListener(
    /* [out] */ IDialogInterfaceOnCancelListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnCancelListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetOnDismissListener(
    /* [in] */ IDialogInterfaceOnDismissListener* listener)
{
    mOnDismissListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetOnDismissListener(
    /* [out] */ IDialogInterfaceOnDismissListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnDismissListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetOnKeyListener(
    /* [in] */ IDialogInterfaceOnKeyListener* listener)
{
    mOnKeyListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetOnKeyListener(
    /* [out] */ IDialogInterfaceOnKeyListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnKeyListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetItems(
    /* [in] */ ArrayOf<ICharSequence*>* items)
{
    mItems = items;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetItems(
    /* [out, callee] */ ArrayOf<ICharSequence*>** items)
{
    VALIDATE_NOT_NULL(items);
    *items = mItems;
    REFCOUNT_ADD(*items);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetAdapter(
    /* [in] */ IListAdapter* adapter)
{
    mAdapter = adapter;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetAdapter(
    /* [out] */ IListAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    *adapter = mAdapter;
    REFCOUNT_ADD(*adapter);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetOnClickListener(
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mOnClickListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetOnClickListener(
    /* [out] */ IDialogInterfaceOnClickListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnClickListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetViewLayoutResId(
    /* [in] */ Int32 viewLayoutResId)
{
    mViewLayoutResId = viewLayoutResId;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetViewLayoutResId(
    /* [out] */ Int32* viewLayoutResId)
{
    VALIDATE_NOT_NULL(viewLayoutResId);
    *viewLayoutResId = mViewLayoutResId;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetView(
    /* [in] */ IView* view)
{
    mView = view;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetViewSpacingLeft(
    /* [in] */ Int32 left)
{
    mViewSpacingLeft = left;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetViewSpacingLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);
    *left = mViewSpacingLeft;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetViewSpacingTop(
    /* [in] */ Int32 top)
{
    mViewSpacingTop = top;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetViewSpacingTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    *top = mViewSpacingTop;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetViewSpacingRight(
    /* [in] */ Int32 right)
{
    mViewSpacingRight = right;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetViewSpacingRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right);
    *right = mViewSpacingRight;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetViewSpacingBottom(
    /* [in] */ Int32 bottom)
{
    mViewSpacingBottom = bottom;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetViewSpacingBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = mViewSpacingBottom;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetViewSpacingSpecified(
    /* [in] */ Boolean value)
{
    mViewSpacingSpecified = value;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetViewSpacingSpecified(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mViewSpacingSpecified;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetCheckedItems(
    /* [in] */ ArrayOf<Boolean>* items)
{
    mCheckedItems = items;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetCheckedItems(
    /* [out, callee] */ ArrayOf<Boolean>** items)
{
    VALIDATE_NOT_NULL(items);
    *items = mCheckedItems;
    REFCOUNT_ADD(*items);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetIsMultiChoice(
    /* [in] */ Boolean isMutiChoice)
{
    mIsMultiChoice = isMutiChoice;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetIsMultiChoice(
    /* [out] */ Boolean* isMutiChoice)
{
    VALIDATE_NOT_NULL(isMutiChoice);
    *isMutiChoice = mIsMultiChoice;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetIsSingleChoice(
    /* [in] */ Boolean isSingleChoice)
{
    mIsSingleChoice = isSingleChoice;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetIsSingleChoice(
    /* [out] */ Boolean* isSingleChoice)
{
    VALIDATE_NOT_NULL(isSingleChoice);
    *isSingleChoice = mIsSingleChoice;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetCheckedItem(
    /* [in] */ Int32 checkedItem)
{
    mCheckedItem = checkedItem;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetCheckedItem(
    /* [out] */ Int32* checkedItem)
{
    VALIDATE_NOT_NULL(checkedItem);
    *checkedItem = mCheckedItem;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetOnCheckboxClickListener(
    /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener)
{
    mOnCheckboxClickListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetOnCheckboxClickListener(
    /* [out] */ IDialogInterfaceOnMultiChoiceClickListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnCheckboxClickListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetCursor(
    /* [in] */ ICursor* cursor)
{
    mCursor = cursor;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetCursor(
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor);
    *cursor = mCursor;
    REFCOUNT_ADD(*cursor);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetLabelColumn(
    /* [in] */ const String& labelColumn)
{
    mLabelColumn = labelColumn;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetLabelColumn(
    /* [out] */ String* labelColumn)
{
    VALIDATE_NOT_NULL(labelColumn);
    *labelColumn = mLabelColumn;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetIsCheckedColumn(
    /* [in] */ const String& column)
{
    mIsCheckedColumn = column;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetIsCheckedColumn(
    /* [out] */ String* column)
{
    VALIDATE_NOT_NULL(column);
    *column = mIsCheckedColumn;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetForceInverseBackground(
    /* [in] */ Boolean value)
{
    mForceInverseBackground = value;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetForceInverseBackground(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mForceInverseBackground;
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetOnItemSelectedListener(
    /* [in] */ IAdapterViewOnItemSelectedListener* listener)
{
    mOnItemSelectedListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetOnItemSelectedListener(
    /* [out] */ IAdapterViewOnItemSelectedListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnItemSelectedListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetOnPrepareListViewListener(
    /* [in] */ IAlertControllerAlertParamsOnPrepareListViewListener* listener)
{
    mOnPrepareListViewListener = listener;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetOnPrepareListViewListener(
    /* [out] */ IAlertControllerAlertParamsOnPrepareListViewListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnPrepareListViewListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode CAlertControllerAlertParams::SetRecycleOnMeasure(
    /* [in] */ Boolean recycleOnMeasure)
{
    mRecycleOnMeasure = recycleOnMeasure;
    return NOERROR;
}

ECode CAlertControllerAlertParams::GetRecycleOnMeasure(
    /* [out] */ Boolean* recycleOnMeasure)
{
    VALIDATE_NOT_NULL(recycleOnMeasure);
    *recycleOnMeasure = mRecycleOnMeasure;
    return NOERROR;
}

ECode CAlertControllerAlertParams::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mCancelable = TRUE;
    AutoPtr<IInterface> temp;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&temp);
    mInflater = ILayoutInflater::Probe(temp.Get());
    return NOERROR;
}

ECode CAlertControllerAlertParams::Apply(
    /* [in] */ IAlertController* dialog)
{
    if (mCustomTitleView != NULL) {
        dialog->SetCustomTitle(mCustomTitleView);
    }
    else {
        if (mTitle != NULL) {
            dialog->SetTitle(mTitle);
        }
        if (mIcon != NULL) {
            dialog->SetIcon(mIcon);
        }
        if (mIconId >= 0) {
            dialog->SetIcon(mIconId);
        }
        if (mIconAttrId > 0) {
            Int32 resId;
            dialog->GetIconAttributeResId(mIconAttrId, &resId);
            dialog->SetIcon(resId);
        }
    }
    if (mMessage != NULL) {
        dialog->SetMessage(mMessage);
    }

    if (mPositiveButtonText != NULL) {
        dialog->SetButton(IDialogInterface::BUTTON_POSITIVE, mPositiveButtonText,
                mPositiveButtonListener, NULL);
    }
    if (mNegativeButtonText != NULL) {
        dialog->SetButton(IDialogInterface::BUTTON_NEGATIVE, mNegativeButtonText,
                mNegativeButtonListener, NULL);
    }
    if (mNeutralButtonText != NULL) {
        dialog->SetButton(IDialogInterface::BUTTON_NEUTRAL, mNeutralButtonText,
                mNeutralButtonListener, NULL);
    }
    if (mForceInverseBackground) {
        dialog->SetInverseBackgroundForced(TRUE);
    }

    // For a list, the client can either supply an array of items or an
    // adapter or a cursor
    if ((mItems != NULL) || (mCursor != NULL) || (mAdapter != NULL)) {
        CreateListView(dialog);
    }

    if (mView != NULL) {
        if (mViewSpacingSpecified) {
            dialog->SetView(mView, mViewSpacingLeft, mViewSpacingTop, mViewSpacingRight,
                    mViewSpacingBottom);
        } else {
            dialog->SetView(mView);
        }
    }
    else if (mViewLayoutResId != 0) {
        dialog->SetView(mViewLayoutResId);
    }

    /*
    dialog.setCancelable(mCancelable);
    dialog.setOnCancelListener(mOnCancelListener);
    if (mOnKeyListener != null) {
        dialog.setOnKeyListener(mOnKeyListener);
    }
    */
    return NOERROR;
}

void CAlertControllerAlertParams::CreateListView(
    /* [in] */ IAlertController* _dialog)
{
    CAlertController* dialog = (CAlertController*)_dialog;

    AutoPtr<IView> tempView;
    mInflater->Inflate(dialog->mListLayout, NULL, (IView**)&tempView);
    AutoPtr<IListView> listView = IListView::Probe(tempView.Get());//IRecycleListView
    assert(listView != NULL);

    AutoPtr<IListAdapter> adapter;
    if (mIsMultiChoice) {
        if (mCursor == NULL) {
            AutoPtr<RecycleArrayAdapter> _adapter = new RecycleArrayAdapter;
            _adapter->constructor(mContext, dialog->mMultiChoiceItemLayout,
                R::id::text1, (ArrayOf<IInterface*>*)mItems.Get(), this, listView);
            adapter = _adapter;
        }
        else {
            AutoPtr<RecycleCursorAdapter> _adapter = new RecycleCursorAdapter;
            _adapter->constructor(mContext, mCursor, FALSE, this, listView, dialog);
            adapter = _adapter;
        }
    }
    else {
        Int32 layout = mIsSingleChoice ? dialog->mSingleChoiceItemLayout : dialog->mListItemLayout;
        if (mCursor == NULL) {
            if (mAdapter != NULL) {
                adapter = mAdapter;
            }
            else {
                AutoPtr<CheckedItemAdapter> _adapter = new CheckedItemAdapter();
                _adapter->constructor(mContext, layout, R::id::text1, mItems);
                adapter = _adapter;
            }

        }
        else {
            AutoPtr<ArrayOf<String> > sParams = ArrayOf<String>::Alloc(1);
            AutoPtr<ArrayOf<Int32> > iParams = ArrayOf<Int32>::Alloc(1);
            (*sParams)[0] = mLabelColumn;
            (*iParams)[0] = R::id::text1;
            CSimpleCursorAdapter::New(mContext, layout, mCursor, sParams, iParams, (ISimpleCursorAdapter**)&adapter);
        }
    }

    if (mOnPrepareListViewListener != NULL) {
        mOnPrepareListViewListener->OnPrepareListView(listView);
    }

    /* Don't directly set the adapter on the ListView as we might
     * want to add a footer to the ListView later.
     */
    dialog->mAdapter = adapter;
    dialog->mCheckedItem = mCheckedItem;

    if (mOnClickListener != NULL) {
        AutoPtr<IAdapterViewOnItemClickListener> l = new AdapterListener(
            this, dialog, listView, TRUE);
        IAdapterView::Probe(listView)->SetOnItemClickListener(l);
    }
    else if (mOnCheckboxClickListener != NULL) {
        AutoPtr<IAdapterViewOnItemClickListener> l = new AdapterListener(
            this, dialog, listView, FALSE);
        IAdapterView::Probe(listView)->SetOnItemClickListener(l);
    }

    // Attach a given OnItemSelectedListener to the ListView
    if (mOnItemSelectedListener != NULL) {
        IAdapterView::Probe(listView)->SetOnItemSelectedListener(mOnItemSelectedListener);
    }

    if (mIsSingleChoice) {
        IAbsListView::Probe(listView)->SetChoiceMode(IAbsListView::CHOICE_MODE_SINGLE);
    }
    else if (mIsMultiChoice) {
        IAbsListView::Probe(listView)->SetChoiceMode(IAbsListView::CHOICE_MODE_MULTIPLE);
    }

    IRecycleListView::Probe(listView)->SetRecycleOnMeasure(mRecycleOnMeasure);
    dialog->mListView = listView;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
