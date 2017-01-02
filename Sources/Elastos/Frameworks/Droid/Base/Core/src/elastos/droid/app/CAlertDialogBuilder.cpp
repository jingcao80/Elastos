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

#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/app/CAlertDialog.h"
#include "elastos/droid/internal/app/CAlertControllerAlertParams.h"
#include "elastos/droid/view/CContextThemeWrapper.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::Internal::App::CAlertControllerAlertParams;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CAlertDialogBuilder, Object, IAlertDialogBuilder)

CAR_OBJECT_IMPL(CAlertDialogBuilder)

CAlertDialogBuilder::CAlertDialogBuilder()
    : mTheme(0)
{
}

ECode CAlertDialogBuilder::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, AlertDialog::ResolveDialogTheme(context, 0));
}

ECode CAlertDialogBuilder::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    AutoPtr<IContextThemeWrapper> wrapper;
    CContextThemeWrapper::New(context, AlertDialog::ResolveDialogTheme(context, theme),
        (IContextThemeWrapper**)&wrapper);
    CAlertControllerAlertParams::New(IContext::Probe(wrapper), (IAlertControllerAlertParams**)&mP);
    assert(mP != NULL);
    mTheme = theme;

    return NOERROR;
}

ECode CAlertDialogBuilder::GetContext(
    /* [out] */ IContext** context)
{
    return mP->GetContext(context);
}

ECode CAlertDialogBuilder::SetTitle(
    /* [in] */ Int32 titleId)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> text;
    context->GetText(titleId, (ICharSequence**)&text);
    return mP->SetTitle(text);
}

ECode CAlertDialogBuilder::SetTitle(
    /* [in] */ ICharSequence* title)
{
    return mP->SetTitle(title);
}

ECode CAlertDialogBuilder::SetCustomTitle(
    /* [in] */ IView* customTitleView)
{
    return mP->SetCustomTitleView(customTitleView);
}

ECode CAlertDialogBuilder::SetMessage(
    /* [in] */ Int32 messageId)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> message;
    context->GetText(messageId, (ICharSequence**)&message);
    return mP->SetMessage(message);
}

ECode CAlertDialogBuilder::SetMessage(
    /* [in] */ ICharSequence* message)
{
    return mP->SetMessage(message);
}

ECode CAlertDialogBuilder::SetIcon(
    /* [in] */ Int32 iconId)
{
    return mP->SetIconId(iconId);
}

ECode CAlertDialogBuilder::SetIcon(
    /* [in] */ IDrawable* icon)
{
    return mP->SetIcon(icon);
}

ECode CAlertDialogBuilder::SetIconAttribute(
    /* [in] */ Int32 attrId)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    Int32 resourceId = Dialog::GetResourceId(context, attrId);
    return mP->SetIconAttrId(resourceId);
}

ECode CAlertDialogBuilder::SetPositiveButton(
    /* [in] */ Int32 textId,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> text;
    context->GetText(textId, (ICharSequence**)&text);

    mP->SetPositiveButtonText(text);
    return mP->SetPositiveButtonListener(listener);
}

ECode CAlertDialogBuilder::SetPositiveButton(
    /* [in] */ ICharSequence* text,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mP->SetPositiveButtonText(text);
    return mP->SetPositiveButtonListener(listener);
}

ECode CAlertDialogBuilder::SetNegativeButton(
    /* [in] */ Int32 textId,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> text;
    context->GetText(textId, (ICharSequence**)&text);

    mP->SetNegativeButtonText(text);
    return mP->SetNegativeButtonListener(listener);
}

ECode CAlertDialogBuilder::SetNegativeButton(
    /* [in] */ ICharSequence* text,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mP->SetNegativeButtonText(text);
    return mP->SetNegativeButtonListener(listener);
}

ECode CAlertDialogBuilder::SetNeutralButton(
    /* [in] */ Int32 textId,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> text;
    context->GetText(textId, (ICharSequence**)&text);

    mP->SetNeutralButtonText(text);
    return mP->SetNeutralButtonListener(listener);
}

ECode CAlertDialogBuilder::SetNeutralButton(
    /* [in] */ ICharSequence* text,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mP->SetNeutralButtonText(text);
    return mP->SetNeutralButtonListener(listener);
}

ECode CAlertDialogBuilder::SetCancelable(
    /* [in] */ Boolean cancelable)
{
    return mP->SetCancelable(cancelable);
}

ECode CAlertDialogBuilder::SetOnCancelListener(
    /* [in] */ IDialogInterfaceOnCancelListener* onCancelListener)
{
    return mP->SetOnCancelListener(onCancelListener);
}

ECode CAlertDialogBuilder::SetOnDismissListener(
    /* [in] */ IDialogInterfaceOnDismissListener* onDismissListener)
{
    return mP->SetOnDismissListener(onDismissListener);
}

ECode CAlertDialogBuilder::SetOnKeyListener(
    /* [in] */ IDialogInterfaceOnKeyListener* onKeyListener)
{
    return mP->SetOnKeyListener(onKeyListener);
}

ECode CAlertDialogBuilder::SetItems(
    /* [in] */ Int32 itemsId,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ArrayOf<ICharSequence*> > items;
    resources->GetTextArray(itemsId, (ArrayOf<ICharSequence*>**)&items);
    mP->SetItems(items);
    return mP->SetOnClickListener(listener);
}

ECode CAlertDialogBuilder::SetItems(
    /* [in] */ ArrayOf<ICharSequence*>* _items,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    if (_items) {
        AutoPtr<ArrayOf<ICharSequence*> > items = _items->Clone();
        mP->SetItems(items);
    }
    else {
        mP->SetItems(NULL);
    }
    return mP->SetOnClickListener(listener);
}

ECode CAlertDialogBuilder::SetAdapter(
    /* [in] */ IListAdapter* adapter,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mP->SetAdapter(adapter);
    return mP->SetOnClickListener(listener);
}

ECode CAlertDialogBuilder::SetCursor(
    /* [in] */ ICursor* cursor,
    /* [in] */ IDialogInterfaceOnClickListener* listener,
    /* [in] */ const String& labelColumn)
{
    mP->SetCursor(cursor);
    mP->SetLabelColumn(labelColumn);
    return mP->SetOnClickListener(listener);
}

ECode CAlertDialogBuilder::SetMultiChoiceItems(
    /* [in] */ Int32 itemsId,
    /* [in] */ ArrayOf<Boolean>* _checkedItems,
    /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ArrayOf<ICharSequence*> > items;
    resources->GetTextArray(itemsId, (ArrayOf<ICharSequence*>**)&items);
    mP->SetItems(items);

    mP->SetOnCheckboxClickListener(listener);

    if (_checkedItems) {
        AutoPtr<ArrayOf<Boolean> > checkedItems = _checkedItems->Clone();
        mP->SetCheckedItems(checkedItems);
    }
    else {
        mP->SetCheckedItems(NULL);
    }

    return mP->SetIsMultiChoice(TRUE);
}

ECode CAlertDialogBuilder::SetMultiChoiceItems(
    /* [in] */ ArrayOf<ICharSequence*>* _items,
    /* [in] */ ArrayOf<Boolean>* _checkedItems,
    /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener)
{
    if (_items) {
        AutoPtr<ArrayOf<ICharSequence*> > items = _items->Clone();
        mP->SetItems(items);
    }
    else {
        mP->SetItems(NULL);
    }

    if (_checkedItems) {
        AutoPtr<ArrayOf<Boolean> > checkedItems = _checkedItems->Clone();
        mP->SetCheckedItems(checkedItems);
    }
    else {
        mP->SetCheckedItems(NULL);
    }

    mP->SetOnCheckboxClickListener(listener);
    return mP->SetIsMultiChoice(TRUE);
}

ECode CAlertDialogBuilder::SetMultiChoiceItems(
    /* [in] */ ICursor* cursor,
    /* [in] */ const String& isCheckedColumn,
    /* [in] */ const String& labelColumn,
    /* [in] */ IDialogInterfaceOnMultiChoiceClickListener* listener)
{
    mP->SetCursor(cursor);
    mP->SetOnCheckboxClickListener(listener);
    mP->SetIsCheckedColumn(isCheckedColumn);
    mP->SetLabelColumn(labelColumn);
    mP->SetIsMultiChoice(TRUE);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetSingleChoiceItems(
    /* [in] */ Int32 itemsId,
    /* [in] */ Int32 checkedItem,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<ArrayOf<ICharSequence*> > items;
    resources->GetTextArray(itemsId, (ArrayOf<ICharSequence*>**)&items);
    mP->SetItems(items);

    mP->SetOnClickListener(listener);
    mP->SetCheckedItem(checkedItem);
    mP->SetIsSingleChoice(TRUE);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetSingleChoiceItems(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 checkedItem,
    /* [in] */ const String& labelColumn,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mP->SetCursor(cursor);
    mP->SetOnClickListener(listener);
    mP->SetCheckedItem(checkedItem);
    mP->SetLabelColumn(labelColumn);
    mP->SetIsSingleChoice(TRUE);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetSingleChoiceItems(
    /* [in] */ ArrayOf<ICharSequence*>* _items,
    /* [in] */ Int32 checkedItem,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    if (_items) {
        AutoPtr<ArrayOf<ICharSequence*> > items = _items->Clone();
        mP->SetItems(items);
    }
    else {
        mP->SetItems(NULL);
    }
    mP->SetOnClickListener(listener);
    mP->SetCheckedItem(checkedItem);
    mP->SetIsSingleChoice(TRUE);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetSingleChoiceItems(
    /* [in] */ IListAdapter* adapter,
    /* [in] */ Int32 checkedItem,
    /* [in] */ IDialogInterfaceOnClickListener* listener)
{
    mP->SetAdapter(adapter);
    mP->SetOnClickListener(listener);
    mP->SetCheckedItem(checkedItem);
    mP->SetIsSingleChoice(TRUE);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetOnItemSelectedListener(
    /* [in] */ IAdapterViewOnItemSelectedListener* listener)
{
    mP->SetOnItemSelectedListener(listener);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetView(
    /* [in] */ Int32 layoutResId)
{
    mP->SetView(NULL);
    mP->SetViewLayoutResId(layoutResId);
    mP->SetViewSpacingSpecified(FALSE);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetView(
    /* [in] */ IView* view)
{
    mP->SetView(view);
    mP->SetViewLayoutResId(0);
    mP->SetViewSpacingSpecified(FALSE);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetView(
    /* [in] */ IView* view,
    /* [in] */ Int32 viewSpacingLeft,
    /* [in] */ Int32 viewSpacingTop,
    /* [in] */ Int32 viewSpacingRight,
    /* [in] */ Int32 viewSpacingBottom)
{
    mP->SetView(view);
    mP->SetViewLayoutResId(0);
    mP->SetViewSpacingSpecified(TRUE);
    mP->SetViewSpacingLeft(viewSpacingLeft);
    mP->SetViewSpacingTop(viewSpacingTop);
    mP->SetViewSpacingRight(viewSpacingRight);
    mP->SetViewSpacingBottom(viewSpacingBottom);
    return NOERROR;
}

ECode CAlertDialogBuilder::SetInverseBackgroundForced(
    /* [in] */ Boolean useInverseBackground)
{
    return mP->SetForceInverseBackground(useInverseBackground);
}

ECode CAlertDialogBuilder::SetRecycleOnMeasureEnabled(
    /* [in] */ Boolean enabled)
{
    return mP->SetRecycleOnMeasure(enabled);
}

ECode CAlertDialogBuilder::Create(
    /* [out] */ IAlertDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);

    AutoPtr<IContext> context;
    mP->GetContext((IContext**)&context);
    AutoPtr<CAlertDialog> cdialog;
    CAlertDialog::NewByFriend(context, mTheme, (CAlertDialog**)&cdialog);
    AutoPtr<IAlertController> alert;
    cdialog->GetAlertController((IAlertController**)&alert);
    mP->Apply(alert);

    Boolean cancelable;
    mP->GetCancelable(&cancelable);
    cdialog->SetCancelable(cancelable);
    if (cancelable) {
        cdialog->SetCanceledOnTouchOutside(TRUE);
    }

    AutoPtr<IDialogInterfaceOnCancelListener> onCancleListener;
    mP->GetOnCancelListener((IDialogInterfaceOnCancelListener**)&onCancleListener);

    AutoPtr<IDialogInterfaceOnDismissListener> onDismissListener;
    mP->GetOnDismissListener((IDialogInterfaceOnDismissListener**)&onDismissListener);

    AutoPtr<IDialogInterfaceOnKeyListener> onKeyListener;
    mP->GetOnKeyListener((IDialogInterfaceOnKeyListener**)&onKeyListener);

    cdialog->SetOnCancelListener(onCancleListener);
    cdialog->SetOnDismissListener(onDismissListener);
    if (onKeyListener != NULL) {
        cdialog->SetOnKeyListener(onKeyListener);
    }

    *dialog = cdialog.Get();
    REFCOUNT_ADD(*dialog);

    return NOERROR;
}

ECode CAlertDialogBuilder::Show(
    /* [out] */ IAlertDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    AutoPtr<IAlertDialog> ad;
    FAIL_RETURN(Create((IAlertDialog**)&ad));
    IDialog::Probe(ad)->Show();

    *dialog = ad;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
