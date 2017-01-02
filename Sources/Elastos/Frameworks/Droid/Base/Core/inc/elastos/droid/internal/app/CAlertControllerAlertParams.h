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

#ifndef __ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLERALERTPARAMS_H__
#define __ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLERALERTPARAMS_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_Internal_App_CAlertControllerAlertParams.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include "elastos/droid/widget/CursorAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnKeyListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnMultiChoiceClickListener;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Droid::Widget::CursorAdapter;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Internal::App::IAlertControllerAlertParamsOnPrepareListViewListener;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {


CarClass(CAlertControllerAlertParams)
    , public Object
    , public IAlertControllerAlertParams
{
private:
    class RecycleArrayAdapter : public ArrayAdapter
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 resource,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ ArrayOf<IInterface*>* objects,
            /* [in] */ CAlertControllerAlertParams* host,
            /* [in] */ IListView* listView);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        AutoPtr<CAlertControllerAlertParams> mAlertParams;
        AutoPtr<IListView> mListView;
    };

    class RecycleCursorAdapter : public CursorAdapter
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ ICursor* c,
            /* [in] */ Boolean autoRequery,
            /* [in] */ CAlertControllerAlertParams* host,
            /* [in] */ IListView* listView,
            /* [in] */ IAlertController* dialog);

        CARAPI NewView(
            /* [in] */ IContext* context,
            /* [in] */ ICursor* cursor,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI BindView(
            /* [in] */ IView* view,
            /* [in] */ IContext* context,
            /* [in] */ ICursor* cursor);

    private:
        CARAPI_(void) InitAdapter();

    private:
        Int32 mLabelIndex;
        Int32 mIsCheckedIndex;
        AutoPtr<CAlertControllerAlertParams> mAlertParams;
        AutoPtr<IListView> mListView;
        IAlertController* mAlertController; // mAlertController hold this's reference
    };

    class AdapterListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        AdapterListener(
            /* [in] */ CAlertControllerAlertParams* params,
            /* [in] */ IAlertController* dialog,
            /* [in] */ IListView* listView,
            /* [in] */ Boolean isClick);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        AutoPtr<CAlertControllerAlertParams> mAlertParams;
        AutoPtr<IAlertController> mAlertController;
        AutoPtr<IListView> mListView;
        Boolean mIsClick;
    };

    class CheckedItemAdapter : public ArrayAdapter
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 resource,
            /* [in] */ Int32 textViewResourceId,
            /* [in] */ ArrayOf<ICharSequence*>* objects);

        // @Override
        CARAPI HasStableIds(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* result);
    };

public:
    CAlertControllerAlertParams();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Apply(
        /* [in] */ IAlertController* dialog);

    CARAPI SetContext(
        /* [in] */ IContext* cxt);

    CARAPI GetContext(
        /* [out] */ IContext** cxt);

    CARAPI SetInflater(
        /* [in] */ ILayoutInflater* inflater);

    CARAPI GetInflater(
        /* [out] */ ILayoutInflater** inflater);

    CARAPI SetIconId(
        /* [in] */ Int32 iconId);

    CARAPI GetIconId(
        /* [out] */ Int32* iconId);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI GetIcon(
        /* [out] */ IDrawable** icon);

    CARAPI SetIconAttrId(
        /* [in] */ Int32 id);

    CARAPI GetIconAttrId(
        /* [out] */ Int32* id);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI SetCustomTitleView(
        /* [in] */ IView* view);

    CARAPI GetCustomTitleView(
        /* [out] */ IView** view);

    CARAPI SetMessage(
        /* [in] */ ICharSequence* message);

    CARAPI GetMessage(
        /* [out] */ ICharSequence** message);

    CARAPI SetPositiveButtonText(
        /* [in] */ ICharSequence* text);

    CARAPI GetPositiveButtonText(
        /* [out] */ ICharSequence** text);

    CARAPI SetPositiveButtonListener(
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI GetPositiveButtonListener(
        /* [out] */ IDialogInterfaceOnClickListener** listener);

    CARAPI SetNegativeButtonText(
        /* [in] */ ICharSequence* text);

    CARAPI GetNegativeButtonText(
        /* [out] */ ICharSequence** text);

    CARAPI SetNegativeButtonListener(
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI GetNegativeButtonListener(
        /* [out] */ IDialogInterfaceOnClickListener** listener);

    CARAPI SetNeutralButtonText(
        /* [in] */ ICharSequence* text);

    CARAPI GetNeutralButtonText(
        /* [out] */ ICharSequence** text);

    CARAPI SetNeutralButtonListener(
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI GetNeutralButtonListener(
        /* [out] */ IDialogInterfaceOnClickListener** listener);

    CARAPI SetCancelable(
        /* [in] */ Boolean cancelable);

    CARAPI GetCancelable(
        /* [out] */ Boolean* cancelable);

    CARAPI SetOnCancelListener(
        /* [in] */ IDialogInterfaceOnCancelListener* listener);

    CARAPI GetOnCancelListener(
        /* [out] */ IDialogInterfaceOnCancelListener** listener);

    CARAPI SetOnDismissListener(
        /* [in] */ IDialogInterfaceOnDismissListener* listener);

    CARAPI GetOnDismissListener(
        /* [out] */ IDialogInterfaceOnDismissListener** listener);

    CARAPI SetOnKeyListener(
        /* [in] */ IDialogInterfaceOnKeyListener* listener);

    CARAPI GetOnKeyListener(
        /* [out] */ IDialogInterfaceOnKeyListener** listener);

    CARAPI SetItems(
        /* [in] */ ArrayOf<ICharSequence*>* items);

    CARAPI GetItems(
        /* [out, callee] */ ArrayOf<ICharSequence*>** items);

    CARAPI SetAdapter(
        /* [in] */ IListAdapter* adapter);

    CARAPI GetAdapter(
        /* [out] */ IListAdapter** adapter);

    CARAPI SetOnClickListener(
        /* [in] */ IDialogInterfaceOnClickListener* listener);

    CARAPI GetOnClickListener(
        /* [out] */ IDialogInterfaceOnClickListener** listener);

    CARAPI SetViewLayoutResId(
        /* [in] */ Int32 viewLayoutResId);

    CARAPI GetViewLayoutResId(
        /* [out] */ Int32* viewLayoutResId);

    CARAPI SetView(
        /* [in] */ IView* view);

    CARAPI GetView(
        /* [out] */ IView** view);

    CARAPI SetViewSpacingLeft(
        /* [in] */ Int32 left);

    CARAPI GetViewSpacingLeft(
        /* [out] */ Int32* left);

    CARAPI SetViewSpacingTop(
        /* [in] */ Int32 top);

    CARAPI GetViewSpacingTop(
        /* [out] */ Int32* top);

    CARAPI SetViewSpacingRight(
        /* [in] */ Int32 right);

    CARAPI GetViewSpacingRight(
        /* [out] */ Int32* right);

    CARAPI SetViewSpacingBottom(
        /* [in] */ Int32 bottom);

    CARAPI GetViewSpacingBottom(
        /* [out] */ Int32* bottom);

    CARAPI SetViewSpacingSpecified(
        /* [in] */ Boolean value);

    CARAPI GetViewSpacingSpecified(
        /* [out] */ Boolean* value);

    CARAPI SetCheckedItems(
        /* [in] */ ArrayOf<Boolean>* items);

    CARAPI GetCheckedItems(
        /* [out, callee] */ ArrayOf<Boolean>** items);

    CARAPI SetIsMultiChoice(
        /* [in] */ Boolean isMutiChoice);

    CARAPI GetIsMultiChoice(
        /* [out] */ Boolean* isMutiChoice);

    CARAPI SetIsSingleChoice(
        /* [in] */ Boolean isSingleChoice);

    CARAPI GetIsSingleChoice(
        /* [out] */ Boolean* isSingleChoice);

    CARAPI SetCheckedItem(
        /* [in] */ Int32 checkedItem);

    CARAPI GetCheckedItem(
        /* [out] */ Int32* checkedItem);

    CARAPI SetOnCheckboxClickListener(
        /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener);

    CARAPI GetOnCheckboxClickListener(
        /* [out] */ IDialogInterfaceOnMultiChoiceClickListener** listener);

    CARAPI SetCursor(
        /* [in] */ ICursor* cursor);

    CARAPI GetCursor(
        /* [out] */ ICursor** cursor);

    CARAPI SetLabelColumn(
        /* [in] */ const String& labelColumn);

    CARAPI GetLabelColumn(
        /* [out] */ String* labelColumn);

    CARAPI SetIsCheckedColumn(
        /* [in] */ const String& column);

    CARAPI GetIsCheckedColumn(
        /* [out] */ String* column);

    CARAPI SetForceInverseBackground(
        /* [in] */ Boolean value);

    CARAPI GetForceInverseBackground(
        /* [out] */ Boolean* value);

    CARAPI SetOnItemSelectedListener(
        /* [in] */ IAdapterViewOnItemSelectedListener* listener);

    CARAPI GetOnItemSelectedListener(
        /* [out] */ IAdapterViewOnItemSelectedListener** listener);

    CARAPI SetOnPrepareListViewListener(
        /* [in] */ IAlertControllerAlertParamsOnPrepareListViewListener* listener);

    CARAPI GetOnPrepareListViewListener(
        /* [out] */ IAlertControllerAlertParamsOnPrepareListViewListener** listener);

    CARAPI SetRecycleOnMeasure(
        /* [in] */ Boolean recycleOnMeasure);

    CARAPI GetRecycleOnMeasure(
        /* [out] */ Boolean* recycleOnMeasure);

private:
    CARAPI_(void) CreateListView(
        /* [in] */ IAlertController* dialog);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ILayoutInflater> mInflater;

    Int32 mIconId;
    AutoPtr<IDrawable> mIcon;
    Int32 mIconAttrId;
    AutoPtr<ICharSequence> mTitle;
    AutoPtr<IView> mCustomTitleView;
    AutoPtr<ICharSequence> mMessage;
    AutoPtr<ICharSequence> mPositiveButtonText;
    AutoPtr<IDialogInterfaceOnClickListener> mPositiveButtonListener;
    AutoPtr<ICharSequence> mNegativeButtonText;
    AutoPtr<IDialogInterfaceOnClickListener> mNegativeButtonListener;
    AutoPtr<ICharSequence> mNeutralButtonText;
    AutoPtr<IDialogInterfaceOnClickListener> mNeutralButtonListener;
    Boolean mCancelable;
    AutoPtr<IDialogInterfaceOnCancelListener> mOnCancelListener;
    AutoPtr<IDialogInterfaceOnDismissListener> mOnDismissListener;
    AutoPtr<IDialogInterfaceOnKeyListener> mOnKeyListener;
    AutoPtr<ArrayOf<ICharSequence*> > mItems;
    AutoPtr<IListAdapter> mAdapter;
    AutoPtr<IDialogInterfaceOnClickListener> mOnClickListener;
    Int32 mViewLayoutResId;
    AutoPtr<IView> mView;
    Int32 mViewSpacingLeft;
    Int32 mViewSpacingTop;
    Int32 mViewSpacingRight;
    Int32 mViewSpacingBottom;
    Boolean mViewSpacingSpecified;
    AutoPtr<ArrayOf<Boolean> > mCheckedItems;
    Boolean mIsMultiChoice;
    Boolean mIsSingleChoice;
    Int32 mCheckedItem;
    AutoPtr<IDialogInterfaceOnMultiChoiceClickListener> mOnCheckboxClickListener;
    AutoPtr<ICursor> mCursor;
    String mLabelColumn;
    String mIsCheckedColumn;
    Boolean mForceInverseBackground;
    AutoPtr<IAdapterViewOnItemSelectedListener> mOnItemSelectedListener;
    AutoPtr<IAlertControllerAlertParamsOnPrepareListViewListener> mOnPrepareListViewListener;
    Boolean mRecycleOnMeasure;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_CALERTCONTROLLERALERTPARAMS_H__
