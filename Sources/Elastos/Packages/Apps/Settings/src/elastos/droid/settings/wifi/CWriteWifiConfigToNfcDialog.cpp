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

#include "Elastos.Droid.Nfc.h"
#include "elastos/droid/settings/wifi/CWriteWifiConfigToNfcDialog.h"
#include "elastos/droid/settings/wifi/AccessPoint.h"
#include "elastos/droid/settings/wifi/CAccessPoint.h"
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

const String CWriteWifiConfigToNfcDialog::NFC_TOKEN_MIME_TYPE("application/vnd.wfa.wsc");

const String CWriteWifiConfigToNfcDialog::TAG("Elastos.Droid.Settings.Wifi.CWriteWifiConfigToNfcDialog");
const String CWriteWifiConfigToNfcDialog::PASSWORD_FORMAT("102700%s%s");
const Int32 CWriteWifiConfigToNfcDialog::HEX_RADIX = 16;
const AutoPtr<ArrayOf<Char32> > CWriteWifiConfigToNfcDialog::hexArray = String("0123456789ABCDEF").GetChars();

//===============================================================================
//                  CWriteWifiConfigToNfcDialog::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_3(CWriteWifiConfigToNfcDialog::InnerListener, Object, ITextWatcher,
        IViewOnClickListener, ICompoundButtonOnCheckedChangeListener)

CWriteWifiConfigToNfcDialog::InnerListener::InnerListener(
    /* [in] */ CWriteWifiConfigToNfcDialog* host)
    : mHost(host)
{}

ECode CWriteWifiConfigToNfcDialog::InnerListener::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    return mHost->OnTextChanged(s, start, before, count);
}

ECode CWriteWifiConfigToNfcDialog::InnerListener::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return mHost->BeforeTextChanged(s, start, count, after);
}

ECode CWriteWifiConfigToNfcDialog::InnerListener::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return mHost->AfterTextChanged(s);
}

ECode CWriteWifiConfigToNfcDialog::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

ECode CWriteWifiConfigToNfcDialog::InnerListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnCheckedChanged(buttonView, isChecked);
}

//===============================================================================
//                  CWriteWifiConfigToNfcDialog::MyRunnable
//===============================================================================

CWriteWifiConfigToNfcDialog::MyRunnable::MyRunnable(
    /* [in] */ CWriteWifiConfigToNfcDialog* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

CWriteWifiConfigToNfcDialog::MyRunnable::~MyRunnable()
{}

ECode CWriteWifiConfigToNfcDialog::MyRunnable::Run()
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
//                  CWriteWifiConfigToNfcDialog::NfcAdapterReaderCallback
//===============================================================================

CAR_INTERFACE_IMPL(CWriteWifiConfigToNfcDialog::NfcAdapterReaderCallback, Object,
    INfcAdapterReaderCallback);

CWriteWifiConfigToNfcDialog::NfcAdapterReaderCallback::NfcAdapterReaderCallback(
    /* [in] */ CWriteWifiConfigToNfcDialog* host)
    : mHost(host)
{}

CWriteWifiConfigToNfcDialog::NfcAdapterReaderCallback::~NfcAdapterReaderCallback()
{}

ECode CWriteWifiConfigToNfcDialog::NfcAdapterReaderCallback::OnTagDiscovered(
    /* [in] */ ITag* tag)
{
    mHost->HandleWriteNfcEvent(tag);
    return NOERROR;
}

//===============================================================================
//                  CWriteWifiConfigToNfcDialog::SetViewTextRunnable
//===============================================================================

CWriteWifiConfigToNfcDialog::SetViewTextRunnable::SetViewTextRunnable(
    /* [in] */ ITextView* textView,
    /* [in] */ Int32 resid)
    : mTextView(textView)
    , mResid(resid)
{}

CWriteWifiConfigToNfcDialog::SetViewTextRunnable::~SetViewTextRunnable()
{}

ECode CWriteWifiConfigToNfcDialog::SetViewTextRunnable::Run()
{
    return mTextView->SetText(mResid);
}

//===============================================================================
//                  CWriteWifiConfigToNfcDialog
//===============================================================================

CAR_INTERFACE_IMPL(CWriteWifiConfigToNfcDialog, AlertDialog, INoCopySpan)

CAR_OBJECT_IMPL(CWriteWifiConfigToNfcDialog)

CWriteWifiConfigToNfcDialog::CWriteWifiConfigToNfcDialog()
{}

CWriteWifiConfigToNfcDialog::~CWriteWifiConfigToNfcDialog()
{}

ECode CWriteWifiConfigToNfcDialog::constructor(
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

ECode CWriteWifiConfigToNfcDialog::OnCreate(
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
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mPasswordView->AddTextChangedListener(listener);
    view = NULL;
    mView->FindViewById(R::id::show_password, (IView**)&view);
    mPasswordCheckBox = ICheckBox::Probe(view);
    ICompoundButton::Probe(mPasswordCheckBox)->SetOnCheckedChangeListener(listener);
    view = NULL;
    mView->FindViewById(R::id::progress_bar, (IView**)&view);
    mProgressBar = IProgressBar::Probe(view);

    AlertDialog::OnCreate(savedInstanceState);

    mSubmitButton = NULL;
    GetButton(IDialogInterface::BUTTON_NEUTRAL, (IButton**)&mSubmitButton);
    IView::Probe(mSubmitButton)->SetOnClickListener(listener);
    IView::Probe(mSubmitButton)->SetEnabled(FALSE);

    mCancelButton = NULL;
    GetButton(IDialogInterface::BUTTON_NEGATIVE, (IButton**)&mCancelButton);
    return NOERROR;
}

ECode CWriteWifiConfigToNfcDialog::OnClick(
    /* [in] */ IView* v)
{
    mWakeLock->AcquireLock();
    AutoPtr<ICharSequence> seq;
    mPasswordView->GetText((ICharSequence**)&seq);
    String password = TO_STR(seq);
    String wpsNfcConfigurationToken;
    mWifiManager->GetWpsNfcConfigurationToken(((CAccessPoint*)mAccessPoint.Get())->mNetworkId, &wpsNfcConfigurationToken);
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

void CWriteWifiConfigToNfcDialog::HandleWriteNfcEvent(
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

ECode CWriteWifiConfigToNfcDialog::Dismiss()
{
    Boolean res;
    if (mWakeLock->IsHeld(&res), res) {
        mWakeLock->ReleaseLock();
    }

    return AlertDialog::Dismiss();
}

ECode CWriteWifiConfigToNfcDialog::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 count)
{
    AutoPtr<MyRunnable> runnable = new MyRunnable(this, 1);
    Boolean res;
    return mOnTextChangedHandler->Post((IRunnable*)runnable, &res);
}

void CWriteWifiConfigToNfcDialog::EnableSubmitIfAppropriate()
{
    if (mPasswordView != NULL) {
        if (((CAccessPoint*)mAccessPoint.Get())->mSecurity == AccessPoint::SECURITY_WEP) {
            Int32 length;
            mPasswordView->GetLength(&length);
            IView::Probe(mSubmitButton)->SetEnabled(length > 0);
        }
        else if (((CAccessPoint*)mAccessPoint.Get())->mSecurity == AccessPoint::SECURITY_PSK) {
            Int32 length;
            mPasswordView->GetLength(&length);
            IView::Probe(mSubmitButton)->SetEnabled(length >= 8);
        }
    }
    else {
        IView::Probe(mSubmitButton)->SetEnabled(FALSE);
    }
}

void CWriteWifiConfigToNfcDialog::SetViewText(
    /* [in] */ ITextView* view,
    /* [in] */ Int32 resid)
{
    AutoPtr<SetViewTextRunnable> runnable = new SetViewTextRunnable(view, resid);
    AutoPtr<IActivity> activity;
    GetOwnerActivity((IActivity**)&activity);
    activity->RunOnUiThread((IRunnable*)runnable);
}

ECode CWriteWifiConfigToNfcDialog::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    mPasswordView->SetInputType(
            IInputType::TYPE_CLASS_TEXT |
            (isChecked ? IInputType::TYPE_TEXT_VARIATION_VISIBLE_PASSWORD
                    : IInputType::TYPE_TEXT_VARIATION_PASSWORD));
    return NOERROR;
}

AutoPtr< ArrayOf<Byte> > CWriteWifiConfigToNfcDialog::HexStringToByteArray(
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

String CWriteWifiConfigToNfcDialog::ByteArrayToHexString(
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

ECode CWriteWifiConfigToNfcDialog::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode CWriteWifiConfigToNfcDialog::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    return NOERROR;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
