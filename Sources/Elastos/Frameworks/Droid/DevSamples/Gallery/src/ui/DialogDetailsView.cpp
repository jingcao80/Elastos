#include "DialogDetailsView.h"
#include "../../R.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;

using Elastos::Droid::App::CDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Utility::CParcelableObjectContainer;

using Elastos::Utility::Logging::Logger;

namespace Gallery {

const String DialogDetailsView::TAG("DialogDetailsView");

DialogDetailsView::MyListener::MyListener(
    /* [in] */ DialogDetailsView* host)
    : mHost(host)
{}

PInterface DialogDetailsView::MyListener::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IViewOnClickListener*)this;
    }
    else if (riid == EIID_IViewOnClickListener) {
        return (IViewOnClickListener*)this;
    }
    else if (riid == EIID_IDialogInterfaceOnDismissListener) {
        return (IDialogInterfaceOnDismissListener*)this;
    }

    return NULL;
}

UInt32 DialogDetailsView::MyListener::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 DialogDetailsView::MyListener::Release()
{
    return ElRefBase::Release();
}

ECode DialogDetailsView::MyListener::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IViewOnClickListener*)this) {
        *pIID = EIID_IViewOnClickListener;
    }
    else if (pObject == (IInterface*)(IDialogInterfaceOnDismissListener*)this) {
        *pIID = EIID_IDialogInterfaceOnDismissListener;
    }
    else {
        return E_INVALID_ARGUMENT;
    }
    return NOERROR;
}

ECode DialogDetailsView::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);

    switch(viewId) {
        case R::id::dialog_info_ok: {
            Logger::D(TAG, "OnClick---dialog_info_ok");
            mHost->mDialog->Dismiss();
            break;
        }
        default:
            break;
    }

    return NOERROR;
}

ECode DialogDetailsView::MyListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (mHost->mListener != NULL) {
        mHost->mListener->OnClose();
    }
    return NOERROR;
}

DialogDetailsView::DialogDetailsView(
    /* [in] */ IActivity* activity,
    // /* [in] */ IDetailsSource* source)
    /* [in] */ ArrayOf<String>* source)
    : mIndex(0)
    , mActivity(activity)
    , mSource(source)
{
    mMyListener = new MyListener(this);
}

ECode DialogDetailsView::Show()
{
    Logger::D(TAG, "Show---");
    // ReloadDetails();
    SetDetails(NULL);
    mDialog->Show();
    return NOERROR;
}

ECode DialogDetailsView::Hide()
{
    Logger::D(TAG, "Hide---");
    mDialog->Hide();
    return NOERROR;
}

ECode DialogDetailsView::ReloadDetails()
{
    // Int32 index = 0;
    // mSource->SetIndex(&index);
    // if (index == -1) return NOERROR;
    // AutoPtr<IMediaDetails> details;
    // mSource->GetDetails((IMediaDetails**)&details);
    // if (details != NULL) {
    //     if (mIndex == index && mDetails == details) return NOERROR;
    //     mIndex = index;
    //     mDetails = details;
    //     SetDetails(details);
    // }
    return NOERROR;
}

ECode DialogDetailsView::SetCloseListener(
    /* [in] */ ICloseListener* listener)
{
    mListener = listener;
    return NOERROR;
}

void DialogDetailsView::SetDetails(
    /* [in] */ MediaDetails* details)
{
    mAdapter = GetArrayAdapter();
    AutoPtr<IView> view;
    if (mDialog == NULL) {
        String title("详细信息");
        AutoPtr<ICharSequence> cs;
        // mActivity->GetString(R::string::details_title, &title);
        // Logger::D(TAG, "SetDetails---title:%s", title.string());
        CStringWrapper::New(title, (ICharSequence**)&cs);
        CDialog::New(mActivity, R::style::MyDialog, (IDialog**)&mDialog);
        mDialog->SetContentView(R::layout::details_list);
        mDialog->FindViewById(R::id::dialog_info_title, (IView**)&view);
        AutoPtr<ITextView> titleView = ITextView::Probe(view);
        titleView->SetText(cs);

        view = NULL;
        mDialog->FindViewById(R::id::dialog_info_ok, (IView**)&view);
        AutoPtr<IButton> okButton = IButton::Probe(view);
        okButton->SetOnClickListener((IViewOnClickListener*)(mMyListener.Get()));
        mDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)(mMyListener.Get()));
    }

    view = NULL;
    mDialog->FindViewById(R::id::photo_info_list, (IView**)&view);
    AutoPtr<IListView> detailsList = IListView::Probe(view);
    detailsList->SetAdapter(IAdapter::Probe(mAdapter));
}

AutoPtr<IArrayAdapter> DialogDetailsView::GetArrayAdapter()
{
    Logger::D(TAG, "GetArrayAdapter()-----");
    AutoPtr<IObjectContainer> dataList;
    CParcelableObjectContainer::New((IObjectContainer**)&dataList);
    StringBuilder sb("名称： ");
    sb += (*mSource)[0];
    AutoPtr<ICharSequence> seq = sb.ToCharSequence();
    dataList->Add(seq);
    sb.Reset();
    sb += "时间： ";
    sb += (*mSource)[1];
    seq = sb.ToCharSequence();
    dataList->Add(seq);
    sb.Reset();
    sb += "宽度： ";
    sb += (*mSource)[2];
    seq = sb.ToCharSequence();
    dataList->Add(seq);
    sb.Reset();
    sb += "高度： ";
    sb += (*mSource)[3];
    seq = sb.ToCharSequence();
    dataList->Add(seq);
    sb.Reset();
    sb += "大小： ";
    sb += (*mSource)[4];
    seq = sb.ToCharSequence();
    dataList->Add(seq);
    sb.Reset();
    sb += "路径： ";
    sb += (*mSource)[5];
    seq = sb.ToCharSequence();
    dataList->Add(seq);

    AutoPtr<IArrayAdapter> arrayAdapter;
    CArrayAdapter::New(mActivity, R::layout::test_item, dataList, (IArrayAdapter**)&arrayAdapter);
    return arrayAdapter;
}

} // namespace Gallery
