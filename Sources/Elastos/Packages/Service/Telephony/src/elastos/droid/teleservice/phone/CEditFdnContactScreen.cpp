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

#include "elastos/droid/teleservice/phone/CEditFdnContactScreen.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IIccCard;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IContactsPhones;
using Elastos::Droid::Provider::IContactsPeopleColumns;
using Elastos::Droid::Provider::IContactsPhonesColumns;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::CSelection;
using Elastos::Droid::Text::Method::IKeyListener;
using Elastos::Droid::Text::Method::CDialerKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListener;
using Elastos::Droid::Text::Method::IDialerKeyListenerHelper;
using Elastos::Droid::Text::Method::CDialerKeyListenerHelper;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CEditFdnContactScreen::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CEditFdnContactScreen::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 visibale;
    IView::Probe(mHost->mPinFieldContainer)->GetVisibility(&visibale);
    if (visibale != IView::VISIBLE) {
        return NOERROR;
    }

    if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mNameField)) {
        Boolean res;
        IView::Probe(mHost->mNumberField)->RequestFocus(&res);
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mNumberField)) {
        Boolean res;
        IView::Probe(mHost->mButton)->RequestFocus(&res);
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mButton)) {
        // Authenticate the pin AFTER the contact information
        // is entered, and if we're not busy.
        if (!mHost->mDataBusy) {
            mHost->AuthenticatePin2();
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CEditFdnContactScreen::MyViewOnFocusChangeListener, Object, IViewOnFocusChangeListener)

ECode CEditFdnContactScreen::MyViewOnFocusChangeListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (hasFocus) {
        AutoPtr<ITextView> textView = ITextView::Probe(v);

        AutoPtr<ICharSequence> text;
        textView->GetText((ICharSequence**)&text);
        AutoPtr<ISelection> helper;
        CSelection::AcquireSingleton((ISelection**)&helper);
        helper->SelectAll(ISpannable::Probe(text));
    }
    return NOERROR;
}


CEditFdnContactScreen::QueryHandler::QueryHandler(
    /* [in] */ IContentResolver* cr,
    /* [in] */ CEditFdnContactScreen* host)
    : mHost(host)
{
    AsyncQueryHandler::constructor(cr);
}

ECode CEditFdnContactScreen::QueryHandler::OnQueryComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ ICursor* c)
{
    return NOERROR;
}

ECode CEditFdnContactScreen::QueryHandler::OnInsertComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ IUri* uri)
{
    if (DBG) mHost->Log(String("onInsertComplete"));
    mHost->DisplayProgress(FALSE);
    mHost->HandleResult(uri != NULL, FALSE);
    return NOERROR;
}

ECode CEditFdnContactScreen::QueryHandler::OnUpdateComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    if (DBG) mHost->Log(String("onUpdateComplete"));
    mHost->DisplayProgress(FALSE);
    mHost->HandleResult(result > 0, FALSE);
    return NOERROR;
}

ECode CEditFdnContactScreen::QueryHandler::OnDeleteComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    return NOERROR;
}


ECode CEditFdnContactScreen::MyRunnable::Run()
{
    return mHost->Finish();
}

const String CEditFdnContactScreen::TAG("CEditFdnContactScreen"); //PhoneGlobals::TAG
const Boolean CEditFdnContactScreen::DBG = FALSE;

const Int32 CEditFdnContactScreen::MENU_IMPORT = 1;
const Int32 CEditFdnContactScreen::MENU_DELETE = 2;

const String CEditFdnContactScreen::INTENT_EXTRA_NAME("name");
const String CEditFdnContactScreen::INTENT_EXTRA_NUMBER("number");

const Int32 CEditFdnContactScreen::PIN2_REQUEST_CODE = 100;

const Int32 CEditFdnContactScreen::CONTACTS_PICKER_CODE = 200;

static AutoPtr<ArrayOf<String> > initNUM_PROJECTION()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);

    array->Set(0, IContactsPeopleColumns::DISPLAY_NAME);
    array->Set(1, IContactsPhonesColumns::NUMBER);
    return array;
}

const AutoPtr<ArrayOf<String> > CEditFdnContactScreen::NUM_PROJECTION = initNUM_PROJECTION();

static AutoPtr<IIntent> initCONTACT_IMPORT_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&intent);

    intent->SetType(IContactsPhones::CONTENT_ITEM_TYPE);
    return intent;
}

const AutoPtr<IIntent> CEditFdnContactScreen::CONTACT_IMPORT_INTENT = initCONTACT_IMPORT_INTENT();

CAR_OBJECT_IMPL(CEditFdnContactScreen)

CEditFdnContactScreen::CEditFdnContactScreen()
    : mName(NULL)
    , mNumber(NULL)
    , mPin2(NULL)
    , mAddContact(FALSE)
    , mDataBusy(FALSE)
{
}

ECode CEditFdnContactScreen::constructor()
{
    CHandler::New((IHandler**)&mHandler);

    mClicked = new MyViewOnClickListener(this);

    mOnFocusChangeHandler = new MyViewOnFocusChangeListener(this);
    return Activity::constructor();
}

ECode CEditFdnContactScreen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    ResolveIntent();

    AutoPtr<IWindow> window = GetWindow();
    Boolean res;
    window->RequestFeature(IWindow::FEATURE_INDETERMINATE_PROGRESS, &res);
    SetContentView(Elastos::Droid::TeleService::R::layout::edit_fdn_contact_screen);
    SetupView();
    SetTitle(mAddContact ?
            Elastos::Droid::TeleService::R::string::add_fdn_contact :
            Elastos::Droid::TeleService::R::string::edit_fdn_contact);

    DisplayProgress(FALSE);
    return NOERROR;
}

ECode CEditFdnContactScreen::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* intent)
{
    if (DBG) {
        StringBuilder sb;
        sb += "onActivityResult request:";
        sb += requestCode;
        sb += " result:";
        sb += resultCode;
        Log(sb.ToString());
    }

    switch (requestCode) {
        case PIN2_REQUEST_CODE:
        {
            AutoPtr<IBundle> extras;
            if (intent != NULL) {
                intent->GetExtras((IBundle**)&extras);
            }
            if (extras != NULL) {
                extras->GetString(String("pin2"), &mPin2);
                if (mAddContact) {
                    AddContact();
                }
                else {
                    UpdateContact();
                }
            }
            else if (resultCode != RESULT_OK) {
                // if they cancelled, then we just cancel too.
                if (DBG) Log(String("onActivityResult: cancelled."));
                Finish();
            }
            break;
        }
        // look for the data associated with this number, and update
        // the display with it.
        case CONTACTS_PICKER_CODE:
        {
            if (resultCode != RESULT_OK) {
                if (DBG) Log(String("onActivityResult: cancelled."));
                return NOERROR;
            }
            AutoPtr<ICursor> cursor;
            //try
            {
                AutoPtr<IContentResolver> contentResolver;
                GetContentResolver((IContentResolver**)&contentResolver);

                AutoPtr<IUri> data;
                FAIL_GOTO(intent->GetData((IUri**)&data), FINALLY)
                FAIL_GOTO(contentResolver->Query(data, NUM_PROJECTION, String(NULL), NULL, String(NULL),
                        (ICursor**)&cursor), FINALLY)

                Boolean result;
                if ((cursor == NULL) || (cursor->MoveToFirst(&result), !result)) {
                    Logger::W(TAG,"onActivityResult: bad contact data, no results found.");
                    return NOERROR;
                }
                String text1;
                FAIL_GOTO(cursor->GetString(0, &text1), FINALLY)
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(text1);
                ITextView::Probe(mNameField)->SetText(cchar);
                String text2;
                FAIL_GOTO(cursor->GetString(1, &text2), FINALLY)
                AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(text2);
                ITextView::Probe(mNumberField)->SetText(cchar2);
            } //finally {
        FINALLY:
            if (cursor != NULL) {
                ICloseable::Probe(cursor)->Close();
            }
            //}
            break;
        }
    }
    return NOERROR;
}

ECode CEditFdnContactScreen::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    Activity::OnCreateOptionsMenu(menu, &res);

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);

    assert(0 && "Add--SetIcon");
    // Added the icons to the context menu
    String str1;
    r->GetString(Elastos::Droid::TeleService::R::string::importToFDNfromContacts, &str1);
    AutoPtr<ICharSequence> cchar1 = CoreUtils::Convert(str1);
    AutoPtr<IMenuItem> item1;
    menu->Add(0, MENU_IMPORT, 0, cchar1, (IMenuItem**)&item1);
    item1->SetIcon(Elastos::Droid::TeleService::R::drawable::ic_menu_contact);

    String str2;
    r->GetString(Elastos::Droid::TeleService::R::string::menu_delete, &str2);
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(str2);
    AutoPtr<IMenuItem> item2;
    menu->Add(0, MENU_DELETE, 0, cchar2, (IMenuItem**)&item2);
    item2->SetIcon(Elastos::Droid::R::drawable::ic_menu_delete);

    *result = TRUE;
    return NOERROR;
}

ECode CEditFdnContactScreen::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    Activity::OnPrepareOptionsMenu(menu, &res);
    *result = mDataBusy ? FALSE : res;
    return NOERROR;
}

ECode CEditFdnContactScreen::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_IMPORT:
            StartActivityForResult(CONTACT_IMPORT_INTENT, CONTACTS_PICKER_CODE);
            *result = TRUE;
            return NOERROR;

        case MENU_DELETE:
            DeleteSelected();
            *result = TRUE;
            return NOERROR;
    }

    return Activity::OnOptionsItemSelected(item, result);
}

void CEditFdnContactScreen::ResolveIntent()
{
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    intent->GetStringExtra(INTENT_EXTRA_NAME, &mName);
    intent->GetStringExtra(INTENT_EXTRA_NUMBER, &mNumber);

    mAddContact = TextUtils::IsEmpty(mNumber);
}

void CEditFdnContactScreen::SetupView()
{
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::TeleService::R::id::fdn_name, (IView**)&view);
    mNameField = IEditText::Probe(view);
    if (mNameField != NULL) {
        IView::Probe(mNameField)->SetOnFocusChangeListener(mOnFocusChangeHandler);
        IView::Probe(mNameField)->SetOnClickListener(mClicked);
    }

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::TeleService::R::id::fdn_number, (IView**)&view2);
    mNumberField = IEditText::Probe(view2);
    if (mNumberField != NULL) {
        AutoPtr<IDialerKeyListenerHelper> helper;
        CDialerKeyListenerHelper::AcquireSingleton((IDialerKeyListenerHelper**)&helper);
        AutoPtr<IDialerKeyListener> listener;
        helper->GetInstance((IDialerKeyListener**)&listener);
        ITextView::Probe(mNumberField)->SetKeyListener(IKeyListener::Probe(listener));
        IView::Probe(mNumberField)->SetOnFocusChangeListener(mOnFocusChangeHandler);
        IView::Probe(mNumberField)->SetOnClickListener(mClicked);
    }

    if (!mAddContact) {
        if (mNameField != NULL) {
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mName);
            ITextView::Probe(mNameField)->SetText(cchar);
        }
        if (mNumberField != NULL) {
            AutoPtr<ICharSequence> cchar = CoreUtils::Convert(mNumber);
            ITextView::Probe(mNumberField)->SetText(cchar);
        }
    }

    AutoPtr<IView> view3;
    FindViewById(Elastos::Droid::TeleService::R::id::button, (IView**)&view3);
    mButton = IButton::Probe(view3);
    if (mButton != NULL) {
        IView::Probe(mButton)->SetOnClickListener(mClicked);
    }

    AutoPtr<IView> view4;
    FindViewById(Elastos::Droid::TeleService::R::id::pinc, (IView**)&view4);
    mPinFieldContainer = ILinearLayout::Probe(view4);
}

String CEditFdnContactScreen::GetNameFromTextField()
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mNameField)->GetText((ICharSequence**)&text);

    String field;
    text->ToString(&field);
    return field;
}

String CEditFdnContactScreen::GetNumberFromTextField()
{
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mNumberField)->GetText((ICharSequence**)&text);

    String field;
    text->ToString(&field);
    return field;
}

AutoPtr<IUri> CEditFdnContactScreen::GetContentURI()
{
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> uri;
    helper->Parse(String("content://icc/fdn"), (IUri**)&uri);
    return uri;
}

Boolean CEditFdnContactScreen::IsValidNumber(
    /* [in] */ const String& number)
{
    return (number.GetLength() <= 20);
}

void CEditFdnContactScreen::AddContact()
{
    if (DBG) Log(String("addContact"));

    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    String number;
    helper->ConvertAndStrip(GetNumberFromTextField(), &number);

    if (!IsValidNumber(number)) {
        HandleResult(FALSE, TRUE);
        return;
    }

    AutoPtr<IUri> uri = GetContentURI();

    AutoPtr<IContentValues> bundle;
    CContentValues::New(3, (IContentValues**)&bundle);
    bundle->Put(String("tag"), GetNameFromTextField());
    bundle->Put(String("number"), number);
    bundle->Put(String("pin2"), mPin2);

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    mQueryHandler = new QueryHandler(resolver, this);
    mQueryHandler->StartInsert(0, NULL, uri, bundle);
    DisplayProgress(TRUE);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(Elastos::Droid::TeleService::R::string::adding_fdn_contact, (ICharSequence**)&cchar);
    ShowStatus(cchar);
}

void CEditFdnContactScreen::UpdateContact()
{
    if (DBG) Log(String("updateContact"));

    const String name = GetNameFromTextField();
    AutoPtr<IPhoneNumberUtils> helper;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
    String number;
    helper->ConvertAndStrip(GetNumberFromTextField(), &number);

    if (!IsValidNumber(number)) {
        HandleResult(FALSE, TRUE);
        return;
    }
    AutoPtr<IUri> uri = GetContentURI();

    AutoPtr<IContentValues> bundle;
    CContentValues::New((IContentValues**)&bundle);
    bundle->Put(String("tag"), mName);
    bundle->Put(String("number"), mNumber);
    bundle->Put(String("newTag"), name);
    bundle->Put(String("newNumber"), number);
    bundle->Put(String("pin2"), mPin2);

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    mQueryHandler = new QueryHandler(resolver, this);
    mQueryHandler->StartUpdate(0, NULL, uri, bundle, String(NULL), NULL);
    DisplayProgress(TRUE);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(Elastos::Droid::TeleService::R::string::updating_fdn_contact, (ICharSequence**)&cchar);
    ShowStatus(cchar);
}

void CEditFdnContactScreen::DeleteSelected()
{
    // delete ONLY if this is NOT a new contact.
    if (!mAddContact) {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetClass((IContext*)this, ECLSID_CDeleteFdnContactScreen);
        intent->PutExtra(INTENT_EXTRA_NAME, mName);
        intent->PutExtra(INTENT_EXTRA_NUMBER, mNumber);
        StartActivity(intent);
    }
    Finish();
}

void CEditFdnContactScreen::AuthenticatePin2()
{
    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClass((IContext*)this, ECLSID_CGetPin2Screen);
    StartActivityForResult(intent, PIN2_REQUEST_CODE);
}

void CEditFdnContactScreen::DisplayProgress(
    /* [in] */ Boolean flag)
{
    // indicate we are busy.
    mDataBusy = flag;
    AutoPtr<IWindow> window = GetWindow();
    window->SetFeatureInt(
            IWindow::FEATURE_INDETERMINATE_PROGRESS,
            mDataBusy ? IWindow::PROGRESS_VISIBILITY_ON : IWindow::PROGRESS_VISIBILITY_OFF);
    // make sure we don't allow calls to save when we're
    // not ready for them.
    IView::Probe(mButton)->SetClickable(!mDataBusy);
}

void CEditFdnContactScreen::ShowStatus(
    /* [in] */ ICharSequence* statusMsg)
{
    if (statusMsg != NULL) {
        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        helper->MakeText(this, statusMsg, IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();
    }
}

void CEditFdnContactScreen::HandleResult(
    /* [in] */ Boolean success,
    /* [in] */ Boolean invalidNumber)
{
    if (success) {
        if (DBG) Log(String("handleResult: success!"));

        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> cchar;
        resources->GetText(mAddContact ?
                Elastos::Droid::TeleService::R::string::fdn_contact_added :
                Elastos::Droid::TeleService::R::string::fdn_contact_updated, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }
    else {
        if (DBG) Log(String("handleResult: failed!"));
        if (invalidNumber) {
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            AutoPtr<ICharSequence> cchar;
            resources->GetText(Elastos::Droid::TeleService::R::string::fdn_invalid_number, (ICharSequence**)&cchar);
            ShowStatus(cchar);
        }
        else {
            AutoPtr<IPhoneFactory> helper;
            CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
            AutoPtr<IPhone> phone;
            helper->GetDefaultPhone((IPhone**)&phone);
            AutoPtr<IIccCard> card;
            phone->GetIccCard((IIccCard**)&card);

            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            Boolean res;
            card->GetIccPin2Blocked(&res);
            if (res) {
                AutoPtr<ICharSequence> cchar;
                resources->GetText(Elastos::Droid::TeleService::R::string::fdn_enable_puk2_requested,
                        (ICharSequence**)&cchar);
                ShowStatus(cchar);
            }
            else if (card->GetIccPuk2Blocked(&res), res) {
                AutoPtr<ICharSequence> cchar;
                resources->GetText(Elastos::Droid::TeleService::R::string::puk2_blocked,
                        (ICharSequence**)&cchar);
                ShowStatus(cchar);
            }
            else {
                // There's no way to know whether the failure is due to incorrect PIN2 or
                // an inappropriate phone number.
                AutoPtr<ICharSequence> cchar;
                resources->GetText(Elastos::Droid::TeleService::R::string::pin2_or_fdn_invalid,
                        (ICharSequence**)&cchar);
                ShowStatus(cchar);
            }
        }
    }

    AutoPtr<IRunnable> r = new MyRunnable(this);

    Boolean res;
    mHandler->PostDelayed(r, 2000, &res);
}

void CEditFdnContactScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[EditFdnContact] ") + msg);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos