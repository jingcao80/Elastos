
#include "elastos/droid/phone/CPhoneApp.h"


namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CEditFdnContactScreen::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CEditFdnContactScreen::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 visibale;
    mPinFieldContainer->GetVisibility(&visibale);
    if (visibale != IView::VISIBLE) {
        return NOERROR;
    }

    if (TO_IINTERFACE(v) == TO_IINTERFACE(mNameField)) {
        mNumberField->RequestFocus();
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mNumberField)) {
        mButton->RequestFocus();
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mButton)) {
        // Authenticate the pin AFTER the contact information
        // is entered, and if we're not busy.
        if (!mDataBusy) {
            AuthenticatePin2();
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CEditFdnContactScreen::MyViewOnClickListener, Object, IViewOnFocusChangeListener)

ECode CEditFdnContactScreen::MyViewOnFocusChangeListener::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (hasFocus) {
        AutoPtr<ITextView> textView = ITextView::Probe(v);

        AutoPtr<ICharSequence> text;
        textView->GetText((ICharSequence**)&text)
        Selection::SelectAll(ISpannable::Probe(text));
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
    return mHost->HandleResult(uri != NULL, FALSE);
}

ECode CEditFdnContactScreen::QueryHandler::OnUpdateComplete(
    /* [in] */ Int32 token,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int32 result)
{
    if (DBG) mHost->Log(String("onUpdateComplete"));
    mHost->DisplayProgress(FALSE);
    return mHost->HandleResult(result > 0, FALSE);
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

const String CEditFdnContactScreen::TAG(PhoneGlobals::TAG);
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

    array->Set(0, IPeopleColumns::DISPLAY_NAME);
    array->Set(1, IPhonesColumns::NUMBER);
    return array;
}

const AutoPtr<ArrayOf<String> > NUM_PROJECTION = initNUM_PROJECTION();

static AutoPtr<IIntent> initCONTACT_IMPORT_INTENT()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_GET_CONTENT, (IIntent**)&intent);

    intent->SetType(android.provider.Contacts.Phones.CONTENT_ITEM_TYPE);
    return intent;
}

const AutoPtr<IIntent> CONTACT_IMPORT_INTENT = initCONTACT_IMPORT_INTENT();

CAR_INTERFACE_IMPL(CEditFdnContactScreen, Activity, IEditFdnContactScreen)

CAR_OBJECT_IMPL(CEditFdnContactScreen)

CEditFdnContactScreen::CEditFdnContactScreen()
    : mName(NULL)
    , mNumber(NULL)
    , mPin2(NULL)
    , mAddContact(FALSE)
    , mDataBusy(FALSE)
{
    mHandler = new Handler();

    mClicked = new MyViewOnClickListener(this);

    mOnFocusChangeHandler = new MyViewOnFocusChangeListener(this);
}

ECode CEditFdnContactScreen::constructor()
{
    return Activity::constructor();
}

ECode CEditFdnContactScreen::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    ResolveIntent();

    AutoPtr<IWindow> window = GetWindow();
    window->RequestFeature(IWindow::FEATURE_INDETERMINATE_PROGRESS);
    SetContentView(R.layout.edit_fdn_contact_screen);
    SetupView();
    SetTitle(mAddContact ?
            R.string.add_fdn_contact : R.string.edit_fdn_contact);

    return DisplayProgress(FALSE);
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

        // look for the data associated with this number, and update
        // the display with it.
        case CONTACTS_PICKER_CODE:
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
                FAIL_GOTO(contentResolver->Query(data, NUM_PROJECTION, NULL, NULL, NULL, (ICursor**)&cursor), FINALLY)

                Boolean result;
                if ((cursor == NULL) || (cursor->MoveToFirst(&result), !result)) {
                    Logger::W(TAG,"onActivityResult: bad contact data, no results found.");
                    return NOERROR;
                }
                String text1;
                FAIL_GOTO(cursor->GetString(0, &text1), FINALLY)
                mNameField->SetText(text1);
                String text2;
                FAIL_GOTO(cursor->GetString(1, &text2), FINALLY)
                mNumberField->SetText(text2);
            } //finally {
        FINALLY:
            if (cursor != NULL) {
                cursor->Close();
            }
            //}
            break;
    }
    return NOERROR;
}

ECode CEditFdnContactScreen::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Activity::OnCreateOptionsMenu(menu);

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);

assert(0 && "Add--SetIcon");
    // Added the icons to the context menu
    String str1;
    r->GetString(R.string.importToFDNfromContacts, &str1);
    menu->Add(0, MENU_IMPORT, 0, str1);
    menu->SetIcon(R.drawable.ic_menu_contact);

    String str2;
    r->GetString(R.string.menu_delete, &str2);
    menu->Add(0, MENU_DELETE, 0, str2)
    menu->SetIcon(android.R.drawable.ic_menu_delete);

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
    FindViewById(R.id.fdn_name, (IView**)&view);
    mNameField = IEditText::Probe(view);
    if (mNameField != NULL) {
        mNameField->SetOnFocusChangeListener(mOnFocusChangeHandler);
        mNameField->SetOnClickListener(mClicked);
    }

    AutoPtr<IView> view2;
    FindViewById(R.id.fdn_number, (IView**)&view2);
    mNumberField = IEditText::Probe(view2);
    if (mNumberField != NULL) {
        mNumberField->SetKeyListener(DialerKeyListener.getInstance());
        mNumberField->SetOnFocusChangeListener(mOnFocusChangeHandler);
        mNumberField->SetOnClickListener(mClicked);
    }

    if (!mAddContact) {
        if (mNameField != NULL) {
            mNameField->SetText(mName);
        }
        if (mNumberField != NULL) {
            mNumberField->SetText(mNumber);
        }
    }

    AutoPtr<IView> view3;
    FindViewById(R.id.button, (IView**)&view3);
    mButton = IButton::Probs(view3);
    if (mButton != NULL) {
        mButton->SetOnClickListener(mClicked);
    }

    AutoPtr<IView> view4;
    FindViewById(R.id.pinc, (IView**)&view4);
    mPinFieldContainer = ILinearLayout::Probe(view4);
}

String CEditFdnContactScreen::GetNameFromTextField()
{
    AutoPtr<ICharSequence> text;
    mNameField->GetText((ICharSequence**)&text);

    String field;
    text->ToString(&field);
    return field;
}

String CEditFdnContactScreen::GetNumberFromTextField()
{
    AutoPtr<ICharSequence> text;
    mNumberField->GetText((ICharSequence**)&text);

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
    if (DBG) Log(String("addContact");

    const String number = PhoneNumberUtils::ConvertAndStrip(GetNumberFromTextField());

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

    mQueryHandler = new QueryHandler(GetContentResolver(), this);
    mQueryHandler->startInsert(0, null, uri, bundle);
    DisplayProgress(TRUE);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(R.string.adding_fdn_contact, (ICharSequence**)&cchar);
    ShowStatus(cchar);
}

void CEditFdnContactScreen::UpdateContact()
{
    if (DBG) Log(String("updateContact"));

    const String name = GetNameFromTextField();
    const String number = PhoneNumberUtils::ConvertAndStrip(GetNumberFromTextField());

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

    mQueryHandler = new QueryHandler(GetContentResolver(), this);
    mQueryHandler->StartUpdate(0, NULL, uri, bundle, NULL, NULL);
    DisplayProgress(TRUE);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> cchar;
    resources->GetText(R.string.updating_fdn_contact, (ICharSequence**)&cchar);
    ShowStatus(cchar);
}

void CEditFdnContactScreen::DeleteSelected()
{
    // delete ONLY if this is NOT a new contact.
    if (!mAddContact) {
        AutoPrt<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetClass(this, DeleteFdnContactScreen.class);
        intent->PutExtra(INTENT_EXTRA_NAME, mName);
        intent->PutExtra(INTENT_EXTRA_NUMBER, mNumber);
        StartActivity(intent);
    }
    Finish();
}

void CEditFdnContactScreen::AuthenticatePin2()
{
    AutoPrt<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClass(this, GetPin2Screen.class);
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
            mDataBusy ? PROGRESS_VISIBILITY_ON : PROGRESS_VISIBILITY_OFF);
    // make sure we don't allow calls to save when we're
    // not ready for them.
    mButton->SetClickable(!mDataBusy);
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
                R.string.fdn_contact_added : R.string.fdn_contact_updated, (ICharSequence**)&cchar);
        ShowStatus(cchar);
    }
    else {
        if (DBG) Log(String("handleResult: failed!"));
        if (invalidNumber) {
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            AutoPtr<ICharSequence> cchar;
            resources->GetText(R.string.fdn_invalid_number, (ICharSequence**)&cchar);
            ShowStatus(cchar);
        }
        else {
            AutoPtr<IPhone> phone;
            PhoneFactory::GetDefaultPhone((IPhone**)&phone);
            AutoPtr<IIccCard> card;
            phone->GetIccCard((IIccCard*)&card);

            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            Boolean res;
            card->GetIccPin2Blocked(&res);
            if (res) {
                AutoPtr<ICharSequence> cchar;
                resources->GetText(R.string.fdn_enable_puk2_requested, (ICharSequence**)&cchar);
                ShowStatus(cchar);
            }
            else if (card->GetIccPuk2Blocked(&res), res) {
                AutoPtr<ICharSequence> cchar;
                resources->GetText(R.string.puk2_blocked, (ICharSequence**)&cchar);
                ShowStatus(cchar);
            }
            else {
                // There's no way to know whether the failure is due to incorrect PIN2 or
                // an inappropriate phone number.
                AutoPtr<ICharSequence> cchar;
                resources->GetText(R.string.pin2_or_fdn_invalid, (ICharSequence**)&cchar);
                ShowStatus(cchar);
            }
        }
    }

    AutoPtr<IRunnable> r = new MyRunnable(this);

    mHandler->PostDelayed(r, 2000);
    return NOERROR;
}

void CEditFdnContactScreen::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, String("[EditFdnContact] ") + msg);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos