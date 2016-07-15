module.exports = function(aoElastos){
//--------common definition----begin----
    var CObject = aoElastos.CObject;

    var CString = aoElastos.Core.CString;
    var Droid_New = aoElastos.Droid.New;
    var Core_New = aoElastos.Core.New;

    var R = aoElastos.Application.R;
//--------common definition----end----

//--------.h----begin----

// #ifndef __DIALOGDETAILSVIEW_H__
// #define __DIALOGDETAILSVIEW_H__

// #include "app/Activity.h"
// #include "DetailsHelper.h"

// using Elastos::Droid::App::IActivity;
// using Elastos::Droid::App::IDialog;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::IDialogInterface;
// using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
// using Elastos::Droid::View::IViewOnClickListener;
// using Elastos::Droid::Widget::IArrayAdapter;

// namespace Gallery {

// class DialogDetailsView
//     : public IDetailsViewContainer
    var DialogDetailsView;
// {
// public:
//     class MyListener
//         : public ElRefBase
//         , public IViewOnClickListener
//         , public IDialogInterfaceOnDismissListener
    var MyListener;
//     {
//     public:
//         MyListener(
//             /* [in] */ DialogDetailsView* host);

//         CAR_INTERFACE_DECL();

//         CARAPI OnClick(
//             /* [in] */ IView* v);

//         CARAPI OnDismiss(
//             /* [in] */ IDialogInterface* dialog);

//     private:
//         DialogDetailsView* mHost;
//     };

// friend class MyListener;

// public:
//     DialogDetailsView(
    var _DialogDetailsView;
    DialogDetailsView = function(
//         /* [in] */ IActivity* activity,
        activity,
//         // /* [in] */ IDetailsSource* source);
//         /* [in] */ ArrayOf<String>* source);
        source)
    {
        _DialogDetailsView(this, activity, source);
    }

//     CARAPI Show();

//     CARAPI Hide();

//     CARAPI ReloadDetails();

//     CARAPI SetCloseListener(
//         /* [in] */ ICloseListener* listener);

// private:
//     CARAPI_(void) SetDetails(
//         /* [in] */ MediaDetails* details);
    var SetDetails;

//     CARAPI_(AutoPtr<IArrayAdapter>) GetArrayAdapter();
    var GetArrayAdapter;

// private:
//     static const String TAG;
    var TAG;

// private:
//     Int32 mIndex;
//     AutoPtr<IActivity> mActivity;
//     // private DetailsAdapter mAdapter;
//     AutoPtr<IArrayAdapter> mAdapter;
//     AutoPtr<MediaDetails> mDetails;
//     // AutoPtr<IDetailsSource> mSource;
//     AutoPtr< ArrayOf<String> > mSource;
//     AutoPtr<IDialog> mDialog;
//     AutoPtr<ICloseListener> mListener;
//     AutoPtr<MyListener> mMyListener;
// };

// } // namespace Gallery

// #endif // __DIALOGDETAILSVIEW_H__

//--------.h----end----

//--------.cpp----begin----

// #include "DialogDetailsView.h"
// #include "../../R.h"
// #include <elastos/Logger.h>
// #include <elastos/StringBuilder.h>

// using Elastos::Core::CStringWrapper;
// using Elastos::Core::ICharSequence;
// using Elastos::Core::StringBuilder;

// using Elastos::Droid::App::CDialog;
// using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
// using Elastos::Droid::View::EIID_IViewOnClickListener;
// using Elastos::Droid::Widget::IAdapter;
// using Elastos::Droid::Widget::CArrayAdapter;
// using Elastos::Droid::Widget::IButton;
// using Elastos::Droid::Widget::IListView;
// using Elastos::Droid::Widget::ITextView;
// using Elastos::Droid::Utility::CParcelableObjectContainer;

// using Elastos::Utility::Logging::Logger;

// namespace Gallery {

// const String DialogDetailsView::TAG("DialogDetailsView");
    TAG = "DialogDetailsView";  //private

// DialogDetailsView::MyListener::MyListener(
    DialogDetailsView.MyListener = function(
//     /* [in] */ DialogDetailsView* host)
        host)
//     : mHost(host)
// {}
    {
        this.mHost = host;
    };
    MyListener = DialogDetailsView.MyListener;

// PInterface DialogDetailsView::MyListener::Probe(
//     /* [in]  */ REIID riid)
// {
//     if (riid == EIID_IInterface) {
//         return (PInterface)(IViewOnClickListener*)this;
//     }
//     else if (riid == EIID_IViewOnClickListener) {
//         return (IViewOnClickListener*)this;
//     }
//     else if (riid == EIID_IDialogInterfaceOnDismissListener) {
//         return (IDialogInterfaceOnDismissListener*)this;
//     }

//     return NULL;
// }

// UInt32 DialogDetailsView::MyListener::AddRef()
// {
//     return ElRefBase::AddRef();
// }

// UInt32 DialogDetailsView::MyListener::Release()
// {
//     return ElRefBase::Release();
// }

// ECode DialogDetailsView::MyListener::GetInterfaceID(
//     /* [in] */ IInterface *pObject,
//     /* [out] */ InterfaceID *pIID)
// {
//     if (pIID == NULL) {
//         return E_INVALID_ARGUMENT;
//     }

//     if (pObject == (IInterface*)(IViewOnClickListener*)this) {
//         *pIID = EIID_IViewOnClickListener;
//     }
//     else if (pObject == (IInterface*)(IDialogInterfaceOnDismissListener*)this) {
//         *pIID = EIID_IDialogInterfaceOnDismissListener;
//     }
//     else {
//         return E_INVALID_ARGUMENT;
//     }
//     return NOERROR;
// }

// ECode DialogDetailsView::MyListener::OnClick(
    MyListener.prototype.OnClick = function(
//     /* [in] */ IView* v)
        v)
// {
    {
//     Int32 viewId = 0;
        var viewId = 0;
//     v->GetId(&viewId);
        var viewId = v.GetId();

//     switch(viewId) {
        switch(viewId) {
//         case R::id::dialog_info_ok: {
            case R.id.dialog_info_ok:
//             Logger::D(TAG, "OnClick---dialog_info_ok");
                elog(TAG + "OnClick---dialog_info_ok");
//             mHost->mDialog->Dismiss();
                this.mHost.mDialog.Dismiss();
//             break;
                break;
//         }
//         default:
            default:
//             break;
                break;
//     }
        }

//     return NOERROR;
        return;
// }
    }

// ECode DialogDetailsView::MyListener::OnDismiss(
    DialogDetailsView.MyListener.prototype.OnDismiss = function(
//     /* [in] */ IDialogInterface* dialog)
        dialog)
// {
    {
//     if (mHost->mListener != NULL) {
        if (this.mHost.mListener != null) {
//         mHost->mListener->OnClose();
            this.mHost.mListener.OnClose();
//     }
        }
//     return NOERROR;
        return;
// }
    }

// DialogDetailsView::DialogDetailsView(
    _DialogDetailsView = function(that,
//     /* [in] */ IActivity* activity,
        activity,
//     // /* [in] */ IDetailsSource* source)
//     /* [in] */ ArrayOf<String>* source)
        source)
//     : mIndex(0)
//     , mActivity(activity)
//     , mSource(source)
// {
    {
        that.mIndex = 0;
        that.mActivity = activity;
        that.mSource = source;

//     mMyListener = new MyListener(this);
        that.mMyListener = new MyListener(this);
// }
    }

// ECode DialogDetailsView::Show()
    DialogDetailsView.prototype.Show = function()
// {
    {
//     Logger::D(TAG, "Show---");
        elog(TAG + "Show---");
//     // ReloadDetails();
//     SetDetails(NULL);
        this.SetDetails(this,null);
//     mDialog->Show();
        this.mDialog.Show();
//     return NOERROR;
        return;
// }
    }

// ECode DialogDetailsView::Hide()
    DialogDetailsView.prototype.Hide = function()
// {
    {
//     Logger::D(TAG, "Hide---");
        elog(TAG + "Hide---");
//     mDialog->Hide();
        this.mDialog.Hide();
//     return NOERROR;
        return;
// }
    }

// ECode DialogDetailsView::ReloadDetails()
    DialogDetailsView.prototype.ReloadDetails = function()
// {
    {
//     // Int32 index = 0;
//     // mSource->SetIndex(&index);
//     // if (index == -1) return NOERROR;
//     // AutoPtr<IMediaDetails> details;
//     // mSource->GetDetails((IMediaDetails**)&details);
//     // if (details != NULL) {
//     //     if (mIndex == index && mDetails == details) return NOERROR;
//     //     mIndex = index;
//     //     mDetails = details;
//     //     SetDetails(details);
//     // }
//     return NOERROR;
        return;
// }
    }

// ECode DialogDetailsView::SetCloseListener(
    DialogDetailsView.prototype.SetCloseListener = function(
//     /* [in] */ ICloseListener* listener)
        listener)
// {
    {
//     mListener = listener;
        this.mListener = listener;
//     return NOERROR;
// }
    }

// void DialogDetailsView::SetDetails(
    SetDetails = function(that,
//     /* [in] */ MediaDetails* details)
        details)
// {
    {
//     mAdapter = GetArrayAdapter();
        that.mAdapter = GetArrayAdapter(that);
//     AutoPtr<IView> view;
        var view;
//     if (mDialog == NULL) {
        if (that.mDialog == null) {
//         String title("详细信息");
            var title = "详细信息";
//         AutoPtr<ICharSequence> cs;
            var cs;
//         // mActivity->GetString(R::string::details_title, &title);
//         // Logger::D(TAG, "SetDetails---title:%s", title.string());
//         CStringWrapper::New(title, (ICharSequence**)&cs);
            cs = CString(title);
//         CDialog::New(mActivity, R::style::MyDialog, (IDialog**)&mDialog);
            that.mDialog = Droid_New("Elastos.Droid.App.CDialog", that.mActivity, R.style.MyDialog);
//         mDialog->SetContentView(R::layout::details_list);
            that.mDialog.SetContentView(R.layout.details_list);
//         mDialog->FindViewById(R::id::dialog_info_title, (IView**)&view);
            view = that.mDialog.FindViewById(R.id.dialog_info_title);
//         AutoPtr<ITextView> titleView = ITextView::Probe(view);
            var titleView = view;
//         titleView->SetText(cs);
            titleView.SetText(cs);

//         view = NULL;
            view = null;
//         mDialog->FindViewById(R::id::dialog_info_ok, (IView**)&view);
            view = that.mDialog.FindViewById(R.id.dialog_info_ok);
//         AutoPtr<IButton> okButton = IButton::Probe(view);
            var okButton = view;
//         okButton->SetOnClickListener((IViewOnClickListener*)(mMyListener.Get()));
            okButton.SetOnClickListener(that.mMyListener);
//         mDialog->SetOnDismissListener((IDialogInterfaceOnDismissListener*)(mMyListener.Get()));
            that.mDialog.SetOnDismissListener(that.mMyListener);
//     }
        }

//     view = NULL;
        view = null;
//     mDialog->FindViewById(R::id::photo_info_list, (IView**)&view);
        view = that.mDialog.FindViewById(R.id.photo_info_list);
//     AutoPtr<IListView> detailsList = IListView::Probe(view);
        var detailsList = view;
//     detailsList->SetAdapter(IAdapter::Probe(mAdapter));
        detailsList.SetAdapter(that.mAdapter);
// }
    }

// AutoPtr<IArrayAdapter> DialogDetailsView::GetArrayAdapter()
    GetArrayAdapter = function(that)
// {
    {
//     Logger::D(TAG, "GetArrayAdapter()-----");
        elog(TAG + "GetArrayAdapter()-----");
//     AutoPtr<IObjectContainer> dataList;
        var dataList;
//     CParcelableObjectContainer::New((IObjectContainer**)&dataList);
        dataList = Core_New("Elastos.Utility.CArrayList");
//     StringBuilder sb("名称： ");
        var sb = "名称： ";
//     sb += (*mSource)[0];
        sb += that.mSource[0];
//     AutoPtr<ICharSequence> seq = sb.ToCharSequence();
        var seq = CString(sb);
//     dataList->Add(seq);
        dataList.Add(seq);
//     sb.Reset();
        sb = "";
//     sb += "时间： ";
        sb += "时间： ";
//     sb += (*mSource)[1];
        sb += that.mSource[1];
//     seq = sb.ToCharSequence();
        seq = CString(sb);
//     dataList->Add(seq);
        dataList.Add(seq);
//     sb.Reset();
        sb = "";
//     sb += "宽度： ";
        sb += "宽度： ";
//     sb += (*mSource)[2];
        sb += that.mSource[2];
//     seq = sb.ToCharSequence();
        seq = CString(sb);
//     dataList->Add(seq);
        dataList.Add(seq);
//     sb.Reset();
        sb = "";
//     sb += "高度： ";
        sb += "高度： ";
//     sb += (*mSource)[3];
        sb += that.mSource[3];
//     seq = sb.ToCharSequence();
        seq = CString(sb);
//     dataList->Add(seq);
        dataList.Add(seq);
//     sb.Reset();
        sb = "";
//     sb += "大小： ";
        sb += "大小： ";
//     sb += (*mSource)[4];
        sb += that.mSource[4];
//     seq = sb.ToCharSequence();
        seq = CString(sb);
//     dataList->Add(seq);
        dataList.Add(seq);
//     sb.Reset();
        sb = "";
//     sb += "路径： ";
        sb += "路径： ";
//     sb += (*mSource)[5];
        sb += that.mSource[5];
//     seq = sb.ToCharSequence();
        seq = CString(sb);
//     dataList->Add(seq);
        dataList.Add(seq);

//     AutoPtr<IArrayAdapter> arrayAdapter;
        var arrayAdapter;
//     CArrayAdapter::New(mActivity, R::layout::test_item, dataList, (IArrayAdapter**)&arrayAdapter);
        arrayAdapter = Droid_New("Elastos.Droid.Widget.CArrayAdapter", that.mActivity, R.layout.test_item, dataList);
//     return arrayAdapter;
        return arrayAdapter;
// }
    }

// } // namespace Gallery

//--------.cpp----end----

    return DialogDetailsView;
};  //exports