
#include "elastos/droid/systemui/qs/QSFooter.h"
#include "elastos/droid/systemui/FontSizeUtils.h"
#include "elastos/droid/systemui/statusbar/phone/CSystemUIDialog.h"
#include "R.h"
#include <elastos/droid/view/LayoutInflater.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::SystemUI::StatusBar::Phone::CSystemUIDialog;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_ISecurityControllerCallback;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

QSFooter::UpdateDisplayState::UpdateDisplayState(
    /* [in] */ QSFooter* host)
    : mHost(host)
{}

ECode QSFooter::UpdateDisplayState::Run()
{
    if (mHost->mFooterTextId != 0) {
        mHost->mFooterText->SetText(mHost->mFooterTextId);
    }
    mHost->mRootView->SetVisibility(mHost->mIsVisible ? IView::VISIBLE : IView::GONE);
    IView::Probe(mHost->mFooterIcon)->SetVisibility(mHost->mIsIconVisible ? IView::VISIBLE : IView::INVISIBLE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(QSFooter::Callback, Object, ISecurityControllerCallback)
QSFooter::Callback::Callback(
    /* [in] */ QSFooter* host)
    : mHost(host)
{}

ECode QSFooter::Callback::OnStateChanged()
{
    mHost->RefreshState();
    return NOERROR;
}

const String QSFooter::TAG("QSFooter");
const Int32 QSFooter::H::CLICK = 0;
const Int32 QSFooter::H::REFRESH_STATE = 1;
QSFooter::H::H(
    /* [in] */ ILooper* looper,
    /* [in] */ QSFooter* host)
    : mLocalHost(host)
{
    Handler::constructor(looper);
}

ECode QSFooter::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    String name;
    ECode ec = NOERROR;
    do {
        Int32 what = 0;
        msg->GetWhat(&what);
        if (what == REFRESH_STATE) {
            name = "handleRefreshState";
            ec = mLocalHost->HandleRefreshState();
            if (FAILED(ec)) {
                break;
            }
        }
        else if (what == CLICK) {
            name = "handleClick";
            ec = mLocalHost->HandleClick();
            if (FAILED(ec)) {
                break;
            }
        }
    } while (0);
    if (FAILED(ec)) {
        String error = String("Error in ") + name;
        Logger::W(TAG, error);
        IQSTileHost::Probe(mLocalHost->mHost)->Warn(error);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(QSFooter::QSFooterClickListener, Object, IViewOnClickListener)
QSFooter::QSFooterClickListener::QSFooterClickListener(
    /* [in] */ QSFooter* host)
    : mHost(host)
{}

ECode QSFooter::QSFooterClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}


const Boolean QSFooter::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
CAR_INTERFACE_IMPL_2(QSFooter, Object, IViewOnClickListener, IDialogInterfaceOnClickListener);
QSFooter::QSFooter(
    /* [in] */ IQSPanel* qsPanel,
    /* [in] */ IContext* context)
    : mIsVisible(FALSE)
    , mIsIconVisible(FALSE)
    , mFooterTextId(0)
{
    mCallback = new Callback(this);
    mUpdateDisplayState = new UpdateDisplayState(this);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    inflater->Inflate(R::layout::quick_settings_footer, IViewGroup::Probe(qsPanel), FALSE, (IView**)&mRootView);

    AutoPtr<QSFooterClickListener> listener = new QSFooterClickListener(this);
    mRootView->SetOnClickListener(listener);

    AutoPtr<IView> view;
    mRootView->FindViewById(R::id::footer_text, (IView**)&view);
    mFooterText = ITextView::Probe(view);
    view = NULL;
    mRootView->FindViewById(R::id::footer_icon, (IView**)&view);
    mFooterIcon = IImageView::Probe(view);
    mContext = context;
    CHandler::New((IHandler**)&mMainHandler);
}

void QSFooter::SetHost(
    /* [in] */ IPhoneQSTileHost* host)
{
    mHost = host;
    host->GetSecurityController((ISecurityController**)&mSecurityController);
    AutoPtr<ILooper> looper;
    IQSTileHost::Probe(host)->GetLooper((ILooper**)&looper);
    mHandler = new H(looper, this);
}

void QSFooter::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening) {
        mSecurityController->AddCallback(mCallback);
    }
    else {
        mSecurityController->RemoveCallback(mCallback);
    }
}

ECode QSFooter::OnConfigurationChanged()
{
    FontSizeUtils::UpdateFontSize(mFooterText, R::dimen::qs_tile_text_size);
    return NOERROR;
}

AutoPtr<IView> QSFooter::GetView()
{
    return mRootView;
}

Boolean QSFooter::HasFooter()
{
    Int32 v = 0;
    return (mRootView->GetVisibility(&v), v) != IView::GONE;
}

ECode QSFooter::OnClick(
    /* [in] */ IView* v)
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(H::CLICK, &tmp);
    return NOERROR;
}

ECode QSFooter::HandleClick()
{
    FAIL_RETURN(IQSTileHost::Probe(mHost)->CollapsePanels());
    // TODO: Delay dialog creation until after panels are collapsed.
    CreateDialog();
    return NOERROR;
}

void QSFooter::RefreshState()
{
    Boolean tmp = FALSE;
    mHandler->SendEmptyMessage(H::REFRESH_STATE, &tmp);
}

ECode QSFooter::HandleRefreshState()
{
    Boolean tmp = FALSE;
    if (mSecurityController->HasDeviceOwner(&tmp), tmp) {
        mFooterTextId = R::string::device_owned_footer;
        mIsVisible = TRUE;
        mIsIconVisible = FALSE;
    }
    else if (mSecurityController->HasProfileOwner(&tmp), tmp) {
        mFooterTextId = R::string::profile_owned_footer;
        mIsVisible = TRUE;
        mIsIconVisible = FALSE;
    }
    else if (mSecurityController->IsVpnEnabled(&tmp), tmp) {
        mFooterTextId = R::string::vpn_footer;
        mIsVisible = TRUE;
        mIsIconVisible = TRUE;
    }
    else {
        mIsVisible = FALSE;
        mIsIconVisible = FALSE;
    }
    return mMainHandler->Post(mUpdateDisplayState, &tmp);
}

ECode QSFooter::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (which == IDialogInterface::BUTTON_NEGATIVE) {
        mSecurityController->DisconnectFromVpn();
    }
    return NOERROR;
}

void QSFooter::CreateDialog()
{
    CSystemUIDialog::New(mContext, (IAlertDialog**)&mDialog);
    IDialog::Probe(mDialog)->SetTitle(GetTitle());

    AutoPtr<ICharSequence> m;
    CString::New(GetMessage(), (ICharSequence**)&m);
    mDialog->SetMessage(m);
    m = NULL;
    CString::New(GetPositiveButton(), (ICharSequence**)&m);
    mDialog->SetButton(IDialogInterface::BUTTON_POSITIVE, m, this);
    Boolean tmp = FALSE;
    if (mSecurityController->IsVpnEnabled(&tmp), tmp) {
        m = NULL;
        CString::New(GetNegativeButton(), (ICharSequence**)&m);
        mDialog->SetButton(IDialogInterface::BUTTON_NEGATIVE, m, this);
    }
    IDialog::Probe(mDialog)->Show();
}

String QSFooter::GetNegativeButton()
{
    Boolean tmp = FALSE;
    String value;
    if (mSecurityController->IsLegacyVpn(&tmp), tmp) {
        mContext->GetString(R::string::disconnect_vpn, &value);
    }
    else {
        mContext->GetString(R::string::disable_vpn, &value);
    }
    return value;
}

String QSFooter::GetPositiveButton()
{
    String value;
    mContext->GetString(R::string::quick_settings_done, &value);
    return value;
}

String QSFooter::GetMessage()
{
    Boolean tmp = FALSE;
    if (mSecurityController->HasDeviceOwner(&tmp), tmp) {
        if (mSecurityController->HasProfileOwner(&tmp), tmp) {
            if (mSecurityController->IsVpnEnabled(&tmp), tmp) {
                if (mSecurityController->IsLegacyVpn(&tmp), tmp) {
                    AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(3);
                    AutoPtr<ICharSequence> obj;
                    String v;
                    mSecurityController->GetDeviceOwnerName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(0, obj);
                    obj = NULL;

                    mSecurityController->GetProfileOwnerName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(1, obj);
                    obj = NULL;

                    mSecurityController->GetLegacyVpnName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(2, obj);
                    mContext->GetString(R::string::monitoring_description_legacy_vpn_device_and_profile_owned,
                            objs, &v);
                    return v;
                }
                else {
                    AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(3);
                    AutoPtr<ICharSequence> obj;
                    String v;
                    mSecurityController->GetDeviceOwnerName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(0, obj);
                    obj = NULL;

                    mSecurityController->GetProfileOwnerName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(1, obj);
                    obj = NULL;

                    mSecurityController->GetVpnApp(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(2, obj);

                    mContext->GetString(R::string::monitoring_description_vpn_device_and_profile_owned,
                            objs, &v);
                    return v;
                }
            }
            else {
                AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(2);
                AutoPtr<ICharSequence> obj;
                String v;
                mSecurityController->GetDeviceOwnerName(&v);
                CString::New(v, (ICharSequence**)&obj);
                objs->Set(0, obj);
                obj = NULL;

                mSecurityController->GetProfileOwnerName(&v);
                CString::New(v, (ICharSequence**)&obj);
                objs->Set(1, obj);
                mContext->GetString(R::string::monitoring_description_device_and_profile_owned,
                    objs, &v);
                return v;
            }
        }
        else {
            if (mSecurityController->IsVpnEnabled(&tmp), tmp) {
                AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(2);
                AutoPtr<ICharSequence> obj;
                String v;
                if (mSecurityController->IsLegacyVpn(&tmp), tmp) {
                    mSecurityController->GetDeviceOwnerName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(0, obj);
                    obj = NULL;

                    mSecurityController->GetLegacyVpnName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(1, obj);
                    mContext->GetString(R::string::monitoring_description_legacy_vpn_device_owned,
                            objs, &v);
                    return v;
                }
                else {
                    mSecurityController->GetDeviceOwnerName(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(0, obj);
                    obj = NULL;

                    mSecurityController->GetVpnApp(&v);
                    CString::New(v, (ICharSequence**)&obj);
                    objs->Set(1, obj);

                    mContext->GetString(R::string::monitoring_description_vpn_device_owned,
                            objs, &v);
                    return v;
                }
            }
            else {
                AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
                AutoPtr<ICharSequence> obj;
                String v;
                mSecurityController->GetDeviceOwnerName(&v);
                CString::New(v, (ICharSequence**)&obj);
                objs->Set(0, obj);
                mContext->GetString(R::string::monitoring_description_device_owned,
                        objs, &v);
                return v;
            }
        }
    }
    else if (mSecurityController->HasProfileOwner(&tmp), tmp) {
        if (mSecurityController->IsVpnEnabled(&tmp), tmp) {
            AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(2);
            AutoPtr<ICharSequence> obj;
            String v;

            if (mSecurityController->IsLegacyVpn(&tmp), tmp) {
                mSecurityController->GetProfileOwnerName(&v);
                CString::New(v, (ICharSequence**)&obj);
                objs->Set(0, obj);
                obj = NULL;

                mSecurityController->GetLegacyVpnName(&v);
                CString::New(v, (ICharSequence**)&obj);
                objs->Set(1, obj);

                mContext->GetString(R::string::monitoring_description_legacy_vpn_profile_owned,
                        objs, &v);
                return v;
            }
            else {
                mSecurityController->GetProfileOwnerName(&v);
                CString::New(v, (ICharSequence**)&obj);
                objs->Set(0, obj);
                obj = NULL;

                mSecurityController->GetVpnApp(&v);
                CString::New(v, (ICharSequence**)&obj);
                objs->Set(1, obj);
                mContext->GetString(R::string::monitoring_description_vpn_profile_owned,
                        objs, &v);
                return v;
            }
        }
        else {
            AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
            AutoPtr<ICharSequence> obj;
            String v;
            mSecurityController->GetProfileOwnerName(&v);
            CString::New(v, (ICharSequence**)&obj);
            objs->Set(0, obj);
            mContext->GetString(R::string::monitoring_description_profile_owned,
                    objs, &v);
            return v;
        }
    }
    else {
        AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
        AutoPtr<ICharSequence> obj;
        String v;

        if (mSecurityController->IsLegacyVpn(&tmp), tmp) {
            mSecurityController->GetLegacyVpnName(&v);
            CString::New(v, (ICharSequence**)&obj);
            objs->Set(0, obj);
            mContext->GetString(R::string::monitoring_description_legacy_vpn,
                    objs, &v);
            return v;

        }
        else {
            mSecurityController->GetVpnApp(&v);
            CString::New(v, (ICharSequence**)&obj);
            objs->Set(0, obj);
            mContext->GetString(R::string::monitoring_description_vpn,
                    objs, &v);
            return v;
        }
    }
}

Int32 QSFooter::GetTitle()
{
    Boolean tmp = FALSE;
    if (mSecurityController->HasDeviceOwner(&tmp), tmp) {
        return R::string::monitoring_title_device_owned;
    }
    if (mSecurityController->HasProfileOwner(&tmp), tmp) {
        return R::string::monitoring_title_profile_owned;
    }
    return R::string::monitoring_title;
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
