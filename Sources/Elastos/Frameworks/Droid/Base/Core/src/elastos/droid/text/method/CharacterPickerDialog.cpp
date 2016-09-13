#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/method/CharacterPickerDialog.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/view/LayoutInflater.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::App::EIID_IDialog;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::Selection;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IWindowCallback;
using Elastos::Droid::View::EIID_IOnWindowDismissedCallback;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnCreateContextMenuListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::EIID_IAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IBaseAdapter;
using Elastos::Droid::Widget::EIID_IListAdapter;
using Elastos::Droid::Widget::EIID_ISpinnerAdapter;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IGridView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_INTERFACE_IMPL_2(CharacterPickerDialog::InnerListener, Object, \
    IAdapterViewOnItemClickListener, IViewOnClickListener)

CharacterPickerDialog::InnerListener::InnerListener(
    /* [in] */ CharacterPickerDialog* host)
    : mHost(host)
{}

ECode CharacterPickerDialog::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

ECode CharacterPickerDialog::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

/*****************************CharacterPickerDialog::OptionsAdapter*****************************/
CharacterPickerDialog::OptionsAdapter::OptionsAdapter()
{
}

ECode CharacterPickerDialog::OptionsAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CharacterPickerDialog* host)
{
    mHost = host;
    return NOERROR;
}

ECode CharacterPickerDialog::OptionsAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** ret)
{
    VALIDATE_NOT_NULL(ret);
    AutoPtr<IView> v;
    AutoPtr<ILayoutInflater> layoutInf = mHost->mInflater;
    layoutInf->Inflate(R::layout::character_picker_button, NULL, (IView**)&v);
    AutoPtr<IButton> b = IButton::Probe(v);
    Char32 c = (mHost->mOptions).GetChar(position);
    ITextView::Probe(b)->SetText(c);
    AutoPtr<CharacterPickerDialog::InnerListener> listener = new CharacterPickerDialog::InnerListener(mHost);
    IView::Probe(b)->SetOnClickListener(listener);
    *ret = IView::Probe(b);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CharacterPickerDialog::OptionsAdapter::GetCount(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    String str = mHost->mOptions;
    *ret = str.GetLength();
    return NOERROR;
}

ECode CharacterPickerDialog::OptionsAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** ret)
{
    VALIDATE_NOT_NULL(ret);

    Char32 c = (mHost->mOptions).GetChar(position);
    String str;
    str.Append(c);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    *ret = (IInterface*)csq.Get();
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

ECode CharacterPickerDialog::OptionsAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = (Int64)position;
    return NOERROR;
}

/*****************************CharacterPickerDialogr*****************************/

CharacterPickerDialog::CharacterPickerDialog()
{}

CharacterPickerDialog::~CharacterPickerDialog()
{}

CAR_INTERFACE_IMPL(CharacterPickerDialog, Dialog, ICharacterPickerDialog)

ECode CharacterPickerDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* view,
    /* [in] */ IEditable* text,
    /* [in] */ const String& options,
    /* [in] */ Boolean insert)
{
    FAIL_RETURN(Dialog::constructor(context, R::style::Theme_Panel));
    mView = view;
    mText = text;
    mOptions = options;
    mInsert = insert;
    return LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
}

ECode CharacterPickerDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Dialog::OnCreate(savedInstanceState);

    AutoPtr<IWindow> iwindow;
    Dialog::GetWindow((IWindow**)&iwindow);
    AutoPtr<IWindowManagerLayoutParams> params;
    iwindow->GetAttributes((IWindowManagerLayoutParams**)&params);

    AutoPtr<IBinder> ibinder;
    params->SetToken((mView->GetApplicationWindowToken((IBinder**)&ibinder), ibinder));
    params->SetType(IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG);
    Int32 flags;
    params->GetFlags(&flags);
    params->SetFlags(flags | IWindow::FEATURE_NO_TITLE);

    Dialog::SetContentView(R::layout::character_picker);

    AutoPtr<IView> gview;
    Dialog::FindViewById(R::id::characterPicker, (IView**)&gview);
    AutoPtr<IGridView> grid = IGridView::Probe(gview);
    AutoPtr<IContext> context;
    Dialog::GetContext((IContext**)&context);
    AutoPtr<OptionsAdapter> adapter = new OptionsAdapter();
    adapter->constructor(context.Get(), this);
    IAdapterView::Probe(grid)->SetAdapter(IAdapter::Probe(adapter));

    AutoPtr<InnerListener> listener = new InnerListener(this);
    AutoPtr<IAdapterView> iav;
    IAdapterView::Probe(grid)->SetOnItemClickListener(listener);

    gview = NULL;
    Dialog::FindViewById(R::id::cancel, (IView**)&gview);
    AutoPtr<IButton> button = IButton::Probe(gview);
    mCancelButton = button;
    IView::Probe(mCancelButton)->SetOnClickListener(listener);
    return NOERROR;
}

ECode CharacterPickerDialog::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Char32 c = mOptions.GetChar(position);
    String str;
    str.Append(c);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    ReplaceCharacterAndClose(csq.Get());
    return NOERROR;
}

void CharacterPickerDialog::ReplaceCharacterAndClose(
    /* [in] */ ICharSequence* replace)
{
    Int32 selEnd = Selection::GetSelectionEnd(ICharSequence::Probe(mText));
    if (mInsert || selEnd == 0) {
        mText->Insert(selEnd, replace);
    } else {
        mText->Replace(selEnd - 1, selEnd, replace);
    }

    Dismiss();
}

ECode CharacterPickerDialog::OnClick(
    /* [in] */ IView* v)
{
    if (Object::Equals(v, IView::Probe(mCancelButton))) {
        Dismiss();
    } else if (IButton::Probe(v)) {
        AutoPtr<ICharSequence> result;
        ITextView::Probe(v)->GetText((ICharSequence**)&result);
        ReplaceCharacterAndClose(result.Get());
    }
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namespace Droid
} // namespace Elastos