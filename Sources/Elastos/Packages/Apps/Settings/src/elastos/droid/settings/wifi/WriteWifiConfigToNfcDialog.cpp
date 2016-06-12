#include "Elastos.Droid.Nfc.h"
#include "elastos/droid/settings/wifi/WriteWifiConfigToNfcDialog.h"
#include "elastos/droid/settings/wifi/AccessPoint.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Nfc::INdefMessage;
// using Elastos::Droid::Nfc::CNdefMessage;
using Elastos::Droid::Nfc::Tech::INdef;
using Elastos::Droid::Nfc::Tech::ITagTechnology;
using Elastos::Droid::Nfc::Tech::INdefHelper;
// using Elastos::Droid::Nfc::Tech::CNdefHelper;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Nfc::INfcAdapterHelper;
// using Elastos::Droid::Nfc::CNfcAdapterHelper;
using Elastos::Droid::Nfc::INdefRecord;
using Elastos::Droid::Nfc::INdefRecordHelper;
// using Elastos::Droid::Nfc::CNdefRecordHelper;
using Elastos::Droid::Nfc::EIID_INfcAdapterReaderCallback;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::EIID_INoCopySpan;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Core::Character;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String WriteWifiConfigToNfcDialog::NFC_TOKEN_MIME_TYPE("application/vnd.wfa.wsc");

const String WriteWifiConfigToNfcDialog::TAG("Elastos.Droid.Settings.Wifi.WriteWifiConfigToNfcDialog");
const String WriteWifiConfigToNfcDialog::PASSWORD_FORMAT("102700%s%s");
const Int32 WriteWifiConfigToNfcDialog::HEX_RADIX = 16;
const AutoPtr<ArrayOf<Char32> > WriteWifiConfigToNfcDialog::hexArray = String("0123456789ABCDEF").GetChars();

//===============================================================================
//                  WriteWifiConfigToNfcDialog::MyRunnable
//===============================================================================

WriteWifiConfigToNfcDialog::MyRunnable::MyRunnable(
    /* [in] */ WriteWifiConfigToNfcDialog* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

WriteWifiConfigToNfcDialog::MyRunnable::~MyRunnable()
{}

ECode WriteWifiConfigToNfcDialog::MyRunnable::Run()
{
    switch (mId) {
        case 0:
            IView::Probe(mHost->mProgressBar)->SetVisibility(IView::GONE);
            break;
        case 1:
            mHost->EnableSubmitIfAppropriate();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  WriteWifiConfigToNfcDialog::NfcAdapterReaderCallback
//===============================================================================

CAR_INTERFACE_IMPL(WriteWifiConfigToNfcDialog::NfcAdapterReaderCallback, Object,
    INfcAdapterReaderCallback);

WriteWifiConfigToNfcDialog::NfcAdapterReaderCallback::NfcAdapterReaderCallback(
    /* [in] */ WriteWifiConfigToNfcDialog* host)
    : mHost(host)
{}

WriteWifiConfigToNfcDialog::NfcAdapterReaderCallback::~NfcAdapterReaderCallback()
{}

ECode WriteWifiConfigToNfcDialog::NfcAdapterReaderCallback::OnTagDiscovered(
    /* [in] */ ITag* tag)
{
    mHost->HandleWriteNfcEvent(tag);
    return NOERROR;
}

//===============================================================================
//                  WriteWifiConfigToNfcDialog::SetViewTextRunnable::
//===============================================================================

WriteWifiConfigToNfcDialog::SetViewTextRunnable::SetViewTextRunnable(
    /* [in] */ ITextView* textView,
    /* [in] */ Int32 resid)
    : mTextView(textView)
    , mResid(resid)
{}

WriteWifiConfigToNfcDialog::SetViewTextRunnable::~SetViewTextRunnable()
{}

ECode WriteWifiConfigToNfcDialog::SetViewTextRunnable::Run()
{
    return mTextView->SetText(mResid);
}

//===============================================================================
//                  WriteWifiConfigToNfcDialog
//===============================================================================

CAR_INTERFACE_IMPL_4(WriteWifiConfigToNfcDialog, AlertDialog, ITextWatcher,
        INoCopySpan, IViewOnClickListener, ICompoundButtonOnCheckedChangeListener);

WriteWifiConfigToNfcDialog::WriteWifiConfigToNfcDialog()
{}

WriteWifiConfigToNfcDialog::~WriteWifiConfigToNfcDialog()
{}

ECode WriteWifiConfigToNfcDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAccessPoint* accessPoint,
    /* [in] */ IWifiManager* wifiManager)
{
    AlertDialog::constructor(context);

    mContext = context;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    IPowerManager::Probe(obj)->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("WriteWifiConfigToNfcDialog:wakeLock"),
            (IPowerManagerWakeLock**)&mWakeLock);
    mAccessPoint = accessPoint;
    CHandler::New((IHandler**)&mOnTextChangedHandler);
    mWifiManager = wifiManager;
    return NOERROR;
}

ECode WriteWifiConfigToNfcDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<ILayoutInflater> inflate;
    GetLayoutInflater((ILayoutInflater**)&inflate);
    mView = NULL;
    inflate->Inflate(R::layout::write_wifi_config_to_nfc, NULL, (IView**)&mView);

    SetView(mView);
    SetInverseBackgroundForced(TRUE);
    SetTitle(R::string::setup_wifi_nfc_tag);
    SetCancelable(TRUE);
    AutoPtr<IResources> resource;
    mContext->GetResources((IResources**)&resource);
    String str;
    resource->GetString(R::string::write_tag, &str);
    SetButton(IDialogInterface::BUTTON_NEUTRAL, CoreUtils::Convert(str),
            (IDialogInterfaceOnClickListener*)NULL);
    resource->GetString(Elastos::Droid::R::string::cancel, &str);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, CoreUtils::Convert(str),
            (IDialogInterfaceOnClickListener*)NULL);

    AutoPtr<IView> view;
    mView->FindViewById(R::id::password, (IView**)&view);
    mPasswordView = ITextView::Probe(view);
    view = NULL;
    mView->FindViewById(R::id::password_label, (IView**)&view);
    mLabelView = ITextView::Probe(view);
    mPasswordView->AddTextChangedListener((ITextWatcher*)this);
    view = NULL;
    mView->FindViewById(R::id::show_password, (IView**)&view);
    mPasswordCheckBox = ICheckBox::Probe(view);
    ICompoundButton::Probe(mPasswordCheckBox)->SetOnCheckedChangeListener((ICompoundButtonOnCheckedChangeListener*)this);
    view = NULL;
    mView->FindViewById(R::id::progress_bar, (IView**)&view);
    mProgressBar = IProgressBar::Probe(view);

    AlertDialog::OnCreate(savedInstanceState);

    mSubmitButton = NULL;
    GetButton(IDialogInterface::BUTTON_NEUTRAL, (IButton**)&mSubmitButton);
    IView::Probe(mSubmitButton)->SetOnClickListener((IViewOnClickListener*)this);
    IView::Probe(mSubmitButton)->SetEnabled(FALSE);

    mCancelButton = NULL;
    GetButton(IDialogInterface::BUTTON_NEGATIVE, (IButton**)&mCancelButton);
    return NOERROR;
}

ECode WriteWifiConfigToNfcDialog::OnClick(
    /* [in] */ IView* v)
{
    mWakeLock->AcquireLock();
    AutoPtr<ICharSequence> seq;
    mPasswordView->GetText((ICharSequence**)&seq);
    String password = TO_STR(seq);
    String wpsNfcConfigurationToken;
    mWifiManager->GetWpsNfcConfigurationToken(((AccessPoint*)mAccessPoint.Get())->mNetworkId, &wpsNfcConfigurationToken);
    String passwordHex = ByteArrayToHexString(password.GetBytes());

    StringBuilder builder;
    builder += "0";
    builder += Character::ForDigit(password.GetLength(), HEX_RADIX);

    String passwordLength = password.GetLength() >= HEX_RADIX
            ? StringUtils::ToString(password.GetLength(), HEX_RADIX)
            : builder.ToString();

    String str("");
    str.AppendFormat(PASSWORD_FORMAT, passwordLength.string(), passwordHex.string());
    passwordHex = str.ToUpperCase();

    if (wpsNfcConfigurationToken.Contains(passwordHex)) {
        mWpsNfcConfigurationToken = wpsNfcConfigurationToken;

        AutoPtr<IActivity> activity;
        GetOwnerActivity((IActivity**)&activity);
        AutoPtr<INfcAdapterHelper> helper;
        assert(0 && "TODO");
        // CNfcAdapterHelper::AcquireSingleton((INfcAdapterHelper**)&helper);
        AutoPtr<INfcAdapter> nfcAdapter;
        helper->GetDefaultAdapter(IContext::Probe(activity), (INfcAdapter**)&nfcAdapter);

        AutoPtr<NfcAdapterReaderCallback> callback = new NfcAdapterReaderCallback(this);
        assert(0 && "TODO");
        nfcAdapter->EnableReaderMode(activity, (INfcAdapterReaderCallback*)callback,
                0 /*INfcAdapter::FLAG_READER_NFC_A |
                INfcAdapter::FLAG_READER_NFC_B |
                INfcAdapter::FLAG_READER_NFC_BARCODE |
                INfcAdapter::FLAG_READER_NFC_F |
                INfcAdapter::FLAG_READER_NFC_V*/,
                NULL);

        IView::Probe(mPasswordView)->SetVisibility(IView::GONE);
        IView::Probe(mPasswordCheckBox)->SetVisibility(IView::GONE);
        IView::Probe(mSubmitButton)->SetVisibility(IView::GONE);
        AutoPtr<IInterface> obj;
        IContext::Probe(activity)->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
        AutoPtr<IInputMethodManager> imm = IInputMethodManager::Probe(obj);
        AutoPtr<IBinder> binder;
        IView::Probe(mPasswordView)->GetWindowToken((IBinder**)&binder);
        Boolean res;
        imm->HideSoftInputFromWindow(binder, 0, &res);

        mLabelView->SetText(R::string::status_awaiting_tap);

        AutoPtr<IView> view;
        mView->FindViewById(R::id::password_layout, (IView**)&view);
        view->SetTextAlignment(IView::TEXT_ALIGNMENT_CENTER);
        IView::Probe(mProgressBar)->SetVisibility(IView::VISIBLE);
    }
    else {
        mLabelView->SetText(R::string::status_invalid_password);
    }
    return NOERROR;
}

void WriteWifiConfigToNfcDialog::HandleWriteNfcEvent(
    /* [in] */ ITag* tag)
{
    AutoPtr<INdefHelper> helper;
    assert(0 && "TODO");
    // CNdefHelper::AcquireSingleton((INdefHelper**)&helper);
    AutoPtr<INdef> ndef;
    helper->GetNdef(tag, (INdef**)&ndef);

    if (ndef != NULL) {
        Boolean res;
        if (ndef->IsWritable(&res), res) {
            AutoPtr<INdefRecordHelper> nrHelper;
            assert(0 && "TODO");
            // CNdefRecordHelper::AcquireSingleton((INdefRecordHelper**)&nrHelper);
            AutoPtr<INdefRecord> record;
            nrHelper->CreateMime(NFC_TOKEN_MIME_TYPE,
                    HexStringToByteArray(mWpsNfcConfigurationToken), (INdefRecord**)&record);
            // try {
            ITagTechnology::Probe(ndef)->Connect();
            AutoPtr<INdefMessage> nmsg;
            assert(0 && "TODO");
            // CNdefMessage::New(record, (INdefMessage**)&nmsg);
            ndef->WriteNdefMessage(nmsg);
            AutoPtr<MyRunnable> runnable = new MyRunnable(this, 0);
            AutoPtr<IActivity> activity;
            GetOwnerActivity((IActivity**)&activity);
            activity->RunOnUiThread((IRunnable*)runnable);
            SetViewText(mLabelView, R::string::status_write_success);
            SetViewText(ITextView::Probe(mCancelButton), Elastos::Droid::R::string::done_label);
            // } catch (IOException e) {
            //     SetViewText(mLabelView, R::string::status_failed_to_write);
            //     Logger::E(TAG, "Unable to write Wi-Fi config to NFC tag.", e);
            //     return;
            // } catch (FormatException e) {
            //     SetViewText(mLabelView, R::string::status_failed_to_write);
            //     Logger::E(TAG, "Unable to write Wi-Fi config to NFC tag.", e);
            //     return;
            // }
        }
        else {
            SetViewText(mLabelView, R::string::status_tag_not_writable);
            Logger::E(TAG, "Tag is not writable");
        }
    }
    else {
        SetViewText(mLabelView, R::string::status_tag_not_writable);
        Logger::E(TAG, "Tag does not support NDEF");
    }
}

ECode WriteWifiConfigToNfcDialog::Dismiss()
{
    Boolean res;
    if (mWakeLock->IsHeld(&res), res) {
        mWakeLock->ReleaseLock();
    }

    return AlertDialog::Dismiss();
}

ECode WriteWifiConfigToNfcDialog::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    AutoPtr<MyRunnable> runnable = new MyRunnable(this, 1);
    Boolean res;
    return mOnTextChangedHandler->Post((IRunnable*)runnable, &res);
}

void WriteWifiConfigToNfcDialog::EnableSubmitIfAppropriate()
{
    if (mPasswordView != NULL) {
        if (((AccessPoint*)mAccessPoint.Get())->mSecurity == AccessPoint::SECURITY_WEP) {
            Int32 length;
            mPasswordView->GetLength(&length);
            IView::Probe(mSubmitButton)->SetEnabled(length > 0);
        }
        else if (((AccessPoint*)mAccessPoint.Get())->mSecurity == AccessPoint::SECURITY_PSK) {
            Int32 length;
            mPasswordView->GetLength(&length);
            IView::Probe(mSubmitButton)->SetEnabled(length >= 8);
        }
    }
    else {
        IView::Probe(mSubmitButton)->SetEnabled(FALSE);
    }
}

void WriteWifiConfigToNfcDialog::SetViewText(
    /* [in] */ ITextView* view,
    /* [in] */ Int32 resid)
{
    AutoPtr<SetViewTextRunnable> runnable = new SetViewTextRunnable(view, resid);
    AutoPtr<IActivity> activity;
    GetOwnerActivity((IActivity**)&activity);
    activity->RunOnUiThread((IRunnable*)runnable);
}

ECode WriteWifiConfigToNfcDialog::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    mPasswordView->SetInputType(
            IInputType::TYPE_CLASS_TEXT |
            (isChecked ? IInputType::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
                    : IInputType::TYPE_TEXT_VARIATION_PASSWORD));
    return NOERROR;
}

AutoPtr< ArrayOf<Byte> > WriteWifiConfigToNfcDialog::HexStringToByteArray(
    /* [in] */ const String& s)
{
    Int32 len = s.GetLength();
    AutoPtr< ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(len / 2);

    for (Int32 i = 0; i < len; i += 2) {
        (*data)[i / 2] = (Byte) ((Character::ToDigit(s.GetChar(i), HEX_RADIX) << 4)
                + Character::ToDigit(s.GetChar(i + 1), HEX_RADIX));
    }

    return data;
}

String WriteWifiConfigToNfcDialog::ByteArrayToHexString(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    AutoPtr< ArrayOf<Char32> > hexChars = ArrayOf<Char32>::Alloc(bytes->GetLength() * 2);
    for ( Int32 j = 0; j < bytes->GetLength(); j++ ) {
        Int32 v = (*bytes)[j] & 0xFF;
        (*hexChars)[j * 2] = (*hexArray)[(v & 0X80000000) | (v >> 4)];
        (*hexChars)[j * 2 + 1] = (*hexArray)[v & 0x0F];
    }

    String str(*hexChars);
    return str;
}

ECode WriteWifiConfigToNfcDialog::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode WriteWifiConfigToNfcDialog::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
