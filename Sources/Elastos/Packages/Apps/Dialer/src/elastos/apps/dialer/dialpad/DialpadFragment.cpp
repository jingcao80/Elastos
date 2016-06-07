
#include "DialpadFragment.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Dialpad {

//=================================================================
// DialpadFragment::DialpadSlidingRelativeLayout
//=================================================================
CAR_INTERFACE_IMPL(DialpadFragment::DialpadSlidingRelativeLayout, RelativeLayout, IDialpadSlidingRelativeLayout)

ECode DialpadFragment::DialpadSlidingRelativeLayout::constructor(
    /* [in] */ IContext* context)
{
    return RelativeLayout::constructor(context);
}

ECode DialpadFragment::DialpadSlidingRelativeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return RelativeLayout::constructor(context, attrs);
}

ECode DialpadFragment::DialpadSlidingRelativeLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return RelativeLayout::constructor(context, attrs, defStyle);
}

ECode DialpadFragment::DialpadSlidingRelativeLayout::GetYFraction(
    /* [out] */ Float* result)
{
    VALUE_NOT_NULL(result);

    Int32 height;
    GetHeight(&height);
    if (height == 0) {
        *result = 0;
        return NOERROR;
    }

    Float y;
    GetTranslationY(&y);
    *result = y / height;
    return NOERROR;
}

ECode DialpadFragment::DialpadSlidingRelativeLayout::SetYFraction(
    /* [in] */ Float yFraction)
{
    Int32 height;
    GetHeight(&height);
    return SetTranslationY(yFraction * height);
}

//=================================================================
// DialpadFragment::DialpadPhoneStateListener
//=================================================================
DialpadFragment::DialpadPhoneStateListener::DialpadPhoneStateListener(
    /* [in] */ DialpadFragment* host)
    : mHost(host)
{}

ECode DialpadFragment::DialpadPhoneStateListener::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    // Log.i(TAG, "PhoneStateListener.onCallStateChanged: "
    //       + state + ", '" + incomingNumber + "'");
    if ((state == ITelephonyManager::CALL_STATE_IDLE) && mHost->IsDialpadChooserVisible()) {
        // Log.i(TAG, "Call ended with dialpad chooser visible!  Taking it down...");
        // Note there's a race condition in the UI here: the
        // dialpad chooser could conceivably disappear (on its
        // own) at the exact moment the user was trying to select
        // one of the choices, which would be confusing.  (But at
        // least that's better than leaving the dialpad chooser
        // onscreen, but useless...)
        mHost->ShowDialpadChooser(FALSE);
    }

    return NOERROR;
}

//=================================================================
// DialpadFragment::ErrorDialogFragment
//=================================================================
const String DialpadFragment::ErrorDialogFragment::ARG_TITLE_RES_ID("argTitleResId");
const String DialpadFragment::ErrorDialogFragment::ARG_MESSAGE_RES_ID("argMessageResId");

CAR_INTERFACE_IMPL(DialpadFragment::ErrorDialogFragment, DialogFragment, IDialpadFragmentErrorDialogFragment)

AutoPtr<IDialpadFragmentErrorDialogFragment> DialpadFragment::ErrorDialogFragment::NewInstance(
    /* [in] */ Int32 messageResId)
{
    return NewInstance(0, messageResId);
}

AutoPtr<IDialpadFragmentErrorDialogFragment> DialpadFragment::ErrorDialogFragment::NewInstance(
    /* [in] */ Int32 titleResId,
    /* [in] */ Int32 messageResId)
{
    AutoPtr<ErrorDialogFragment> fragment = new ErrorDialogFragment();
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutInt32(ARG_TITLE_RES_ID, titleResId);
    args->PutInt32(ARG_MESSAGE_RES_ID, messageResId);
    fragment->SetArguments(args);
    return (IDialpadFragmentErrorDialogFragment*)fragment;
}

ECode DialpadFragment::ErrorDialogFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    DialogFragment::OnCreate(savedInstanceState);
    AutoPtr<IBundle> args;
    GetArguments((IBundle**)&args);
    args->GetInt32(ARG_TITLE_RES_ID, &mTitleResId);
    args->GetInt32(ARG_MESSAGE_RES_ID, &mMessageResId);

    return NOERROR;
}

ECode DialpadFragment::ErrorDialogFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALUE_NOT_NULL(dialog);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);

    if (mTitleResId != 0) {
        builder->SetTitle(mTitleResId);
    }
    if (mMessageResId != 0) {
        builder->SetMessage(mMessageResId);
    }

    AutoPtr<AlertDialogClickListener> listener = new AlertDialogClickListener();
    builder->SetPositiveButton(Elastos::R::string::ok, listener);
    return builder->Create(dialog);
}

//=================================================================
// DialpadFragment::SpacerOnTouchListener
//=================================================================
CAR_INTERFACE_IMPL(DialpadFragment::SpacerOnTouchListener, Object, IViewOnTouchListener)

DialpadFragment::SpacerOnTouchListener::SpacerOnTouchListener(
    /* [in] */ DialpadFragment* host)
    : mHost(host)
{}

ECode DialpadFragment::SpacerOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [in] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    if (mHost->IsDigitsEmpty()) {
        mHost->HideAndClearDialpad(TRUE);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=================================================================
// DialpadFragment::AlertDialogClickListener
//=================================================================
CAR_INTERFACE_IMPL(DialpadFragment::AlertDialogClickListener, Object, IDialogInterfaceOnClickListener)

ECode DialpadFragment::AlertDialogClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Dismiss();
    return NOERROR;
}


//=================================================================
// DialpadFragment::DialpadPopupMenu
//=================================================================
DialpadFragment::DialpadPopupMenu::DialpadPopupMenu(
    /* [in] */ DialpadFragment* host)
    : mHost(host)
{}

ECode DialpadFragment::DialpadPopupMenu::Show()
{
    AutoPtr<IMenu> menu;
    GetMenu((IMenu**)&menu);
    AutoPtr<IMenuItem> sendMessage;
    menu->FindItem(R::id::menu_send_message, (IMenuItem**)&sendMessage);
    sendMessage->SetVisible(mHost->mSmsPackageComponentName != NULL);

    Boolean enable = !mHost->IsDigitsEmpty();
    Int32 size;
    menu->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IMenuItem> item;
        menu->GetItem(i, (IMenuItem**)&item);
        item->SetEnabled(enable);
    }

    return PopupMenu::Show();
}

//=================================================================
// DialpadFragment::DialpadChooserAdapter::ChoiceItem
//=================================================================
DialpadFragment::DialpadChooserAdapter::ChoiceItem::ChoiceItem(
    /* [in] */ const String& s,
    /* [in] */ IBitmap* b,
    /* [in] */ Int32 i)
    : mIcon(b)
    , mId(i)
{
    mText = s;
}

//=================================================================
// DialpadFragment::DialpadChooserAdapter
//=================================================================
const Int32 DialpadFragment::DialpadChooserAdapter::DIALPAD_CHOICE_USE_DTMF_DIALPAD = 101;
const Int32 DialpadFragment::DialpadChooserAdapter::DIALPAD_CHOICE_RETURN_TO_CALL = 102;
const Int32 DialpadFragment::DialpadChooserAdapter::DIALPAD_CHOICE_ADD_NEW_CALL = 103;
const Int32 DialpadFragment::DialpadChooserAdapter::NUM_ITEMS = 3;

DialpadFragment::DialpadChooserAdapter::DialpadChooserAdapter(
    /* [in] */ IContext* context)
{
    mChoiceItems = ArrayOf<ChoiceItem*>::Alloc(NUM_ITEMS);

    // Cache the LayoutInflate to avoid asking for a new one each time.
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);

    // Initialize the possible choices.
    // TODO: could this be specified entirely in XML?

    // - "Use touch tone keypad"
    String str;
    context->GetString(R::string::dialer_useDtmfDialpad, &str);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IBitmapFactory> factory;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
    AutoPtr<IBitmap> bitmap;
    factory->DecodeResource(resources,
            R::drawable::ic_dialer_fork_tt_keypad, (IBitmap**)&bitmap);
    mChoiceItems[0] = new ChoiceItem(str,
            bitmap, DIALPAD_CHOICE_USE_DTMF_DIALPAD));

    // - "Return to call in progress"
    context->GetString(R::string::dialer_returnToInCallScreen, &str);
    bitmap = NULL;
    factory->DecodeResource(resources,
            R::drawable::ic_dialer_fork_current_call, (IBitmap**)&bitmap);
    mChoiceItems[1] = new ChoiceItem(str,
            bitmap, DIALPAD_CHOICE_RETURN_TO_CALL));

    // - "Add call"
    context->GetString(R::string::dialer_addAnotherCall, &str);
    bitmap = NULL;
    factory->DecodeResource(resources,
            R::drawable::ic_dialer_fork_add_call, (IBitmap**)&bitmap);
    mChoiceItems[2] = new ChoiceItem(str,
            bitmap, DIALPAD_CHOICE_ADD_NEW_CALL));
}

ECode DialpadFragment::DialpadChooserAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALUE_NOT_NULL(count);
    *count = NUM_ITEMS;
    return NOERROR;
}

ECode DialpadFragment::DialpadChooserAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALUE_NOT_NULL(item);

    *item = (IInterface*)mChoiceItems[position];
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode DialpadFragment::DialpadChooserAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALUE_NOT_NULL(itemId);
    *itemId = position;
    return NOERROR;
}

ECode DialpadFragment::DialpadChooserAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);

    // When convertView is non-null, we can reuse it (there's no need
    // to reinflate it.)
    AutoPtr<IView> temp;
    if (convertView == NULL) {
        mInflater->Inflate(R::layout::dialpad_chooser_list_item, NULL, (IView**)&temp);
        convertView = temp.Get();
    }

    AutoPtr<IInterface> text;
    convertView->FindViewById(R::id::text, (IInterface**)&text);
    ITextView::Probe(text)->SetText(mChoiceItems[position]->mText);

    AutoPtr<IInterface> icon;
    convertView->FindViewById(R::id::icon, (IInterface**)&icon);
    IImageView::Probe(icon)->setImageBitmap(mChoiceItems[position]->mIcon);

    *view = convertView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

//=================================================================
// DialpadFragment
//=================================================================
const String DialpadFragment::TAG("DialpadFragment");

const Boolean DialpadFragment:: DEBUG = DialtactsActivity::DEBUG;

const Float DialpadFragment::DIALPAD_SLIDE_FRACTION = 0.67f;

const String DialpadFragment::EMPTY_NUMBER;
const Char32 DialpadFragment::PAUSE = ',';
const Char32 DialpadFragment::WAIT = ';';

const Int32 DialpadFragment::TONE_LENGTH_MS = 150;
const Int32 DialpadFragment::TONE_LENGTH_INFINITE = -1;

const Int32 DialpadFragment::TONE_RELATIVE_VOLUME = 80;

const Int32 DialpadFragment::DIAL_TONE_STREAM_TYPE = IAudioManager::STREAM_DTMF;

const String DialpadFragment::ADD_CALL_MODE_KEY("add_call_mode");

const String DialpadFragment::EXTRA_SEND_EMPTY_FLASH("com.android.phone.extra.SEND_EMPTY_FLASH");

const String DialpadFragment::PREF_DIGITS_FILLED_BY_INTENT("pref_digits_filled_by_intent");

CAR_INTERFACE_IMPL_8(DialpadFragment, AnalyticsFragment, IDialpadFragment, IViewOnClickListener,
        IViewOnLongClickListener, IViewOnKeyListener, IAdapterViewOnItemClickListener, ITextWatcher,
        IPopupMenuOnMenuItemClickListener, IDialpadKeyButtonOnPressedListener)

DialpadFragment::DialpadFragment()
    : mStartedFromNewIntent(FALSE)
    , mFirstLaunch(FALSE)
    , mAnimate(FALSE)
{
    CHashSet::New(12, (HashSet**)&mPressedDialpadKeys);

    assert(0 && "TODO");
    // CCallLogAsync::New((ICallLogAsync**)&mCallLog);
    // CHapticFeedback::New((IHapticFeedback**)&mHaptic);

    mPhoneStateListener = new DialpadPhoneStateListener(this);
    mLastNumberDialed = EMPTY_NUMBER;
}

AutoPtr<IIntent> DialpadFragment::GetVoicemailIntent()
{
    AutoPtr<IUri> uri;
    Uri::FromParts(IPhoneAccount::SCHEME_VOICEMAIL,
            String(""), String(NULL), (IUri**)&uri);
    return CallUtil::GetCallIntent(uri);
}

AutoPtr<ITelephonyManager> DialpadFragment::GetTelephonyManager()
{
    AutoPtr<IActivity> activity;
    GetActivity(&activity);
    AutoPtr<IInterface> service;
    activity->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    return ITelephonyManager::Probe(service);
}

AutoPtr<ITelecomManager> DialpadFragment::GetTelecomManager()
{
    AutoPtr<IActivity> activity;
    GetActivity(&activity);
    AutoPtr<IInterface> service;
    activity->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    return ITelecomManager::Probe(service);
}

ECode DialpadFragment::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    mWasEmptyBeforeTextChange = TextUtils::IsEmpty(s);
    return NOERROR;
}

ECode DialpadFragment::OnTextChanged(
    /* [in] */ ICharSequence* input,
    /* [in] */ Int32 start,
    /* [in] */ Int32 before,
    /* [in] */ Int32 changeCount)
{
    if (mWasEmptyBeforeTextChange != TextUtils::IsEmpty(input)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        if (activity != NULL) {
            activity->InvalidateOptionsMenu();
            UpdateMenuOverflowButton(mWasEmptyBeforeTextChange);
        }
    }

    // DTMF Tones do not need to be played here any longer -
    // the DTMF dialer handles that functionality now.
}

ECode DialpadFragment::AfterTextChanged(
    /* [in] */ IEditable* input)
{
    // When DTMF dialpad buttons are being pressed, we delay SpecialCharSequencMgr sequence,
    // since some of SpecialCharSequenceMgr's behavior is too abrupt for the "touch-down"
    // behavior.
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    String str;
    input->ToString(&str);
    if (!mDigitsFilledByIntent &&
            SpecialCharSequenceMgr::HandleChars(activity, str, mDigits)) {
        // A special sequence was entered, clear the digits
        AutoPtr<IEditable> text;
        ITextView::Probe(mDigits)->GetEditableText((IEditable**)&text);
        text->Clear();
    }

    if (IsDigitsEmpty()) {
        mDigitsFilledByIntent = FALSE;
        mDigits->SetCursorVisible(FALSE);
    }

    if (mDialpadQueryListener != NULL) {
        AutoPtr<IEditable> text;
        ITextView::Probe(mDigits)->GetEditableText((IEditable**)&text);
        text->ToString(&str);
        mDialpadQueryListener->OnDialpadQueryChanged(str);
    }
    UpdateDeleteButtonEnabledState();

    return NOERROR;
}

ECode DialpadFragment::OnCreate(
    /* [in] */ IBundle* state)
{
    AnalyticsFragment::OnCreate(state);
    mFirstLaunch = TRUE;
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mCurrentCountryIso = GeoUtil::GetCurrentCountryIso(activity);

    // try {
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Boolean enable;
    ECode ec = resources->GetBoolean(
            R::bool::config_enable_dialer_key_vibration, &enable);
    if (ec == (ECode)E_RESOURCES_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "Vibrate control bool missing.");
    } else {
        assert(0 && "TODO");
        // mHaptic->Init(activity, enable);
    }
    // } catch (Resources.NotFoundException nfe) {
    //      Log.e(TAG, "Vibrate control bool missing.", nfe);
    // }

    resources->GetString(R::string::config_prohibited_phone_number_regexp,
             &mProhibitedPhoneNumberRegexp);

    if (state != NULL) {
        state->GetBoolean(PREF_DIGITS_FILLED_BY_INTENT, &mDigitsFilledByIntent);
    }

    resources->GetInteger(
            R::integer::dialpad_slide_in_duration, &mDialpadSlideInDuration);

    return NOERROR;
}

ECode DialpadFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedState,
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);

    AutoPtr<IView> fragmentView;
    inflater->Inflate(R::layout::dialpad_fragment,
            container, FALSE, (IView**)&fragmentView);
    fragmentView->BuildLayer();

    AutoPtr<IResources> r;
    GetResources((IResources**)&r);

    AutoPtr<IView> dialpadView;
    fragmentView->FindViewById(R::id::dialpad_view, (IView**)&dialpadView);
    assert(0 && "TODO");
    // mDialpadView = IDialpadView::Probe(dialpadView);
    // mDialpadView->SetCanDigitsBeEdited(TRUE);
    mDigits = mDialpadView.getDigits();
    mDigits->SetKeyListener(UnicodeDialerKeyListener::INSTANCE);
    mDigits->SetOnClickListener(this);
    mDigits->SetOnKeyListener(this);
    mDigits->SetOnLongClickListener(this);
    mDigits->AddTextChangedListener(this);
    mDigits->SetElegantTextHeight(FALSE);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    PhoneNumberFormatter::SetPhoneNumberFormattingTextWatcher(activity, mDigits);
    // Check for the presence of the keypad
    AutoPtr<IView> oneButton;
    fragmentView->FindViewById(R::id::one, (IView**)&oneButton);
    if (oneButton != NULL) {
        ConfigureKeypadListeners(fragmentView);
    }

    assert(0 && "TODO");
    // mDialpadView->GetDeleteButton((IView**)&mDelete);

    if (mDelete != NULL) {
        mDelete->SetOnClickListener(this);
        mDelete->SetOnLongClickListener(this);
    }

    fragmentView->FindViewById(R::id::spacer, (IView**)&mSpacer);
    AutoPtr<SpacerOnTouchListener> listener = new SpacerOnTouchListener(this);
    mSpacer->SetOnTouchListener(listener);

    mDigits->SetCursorVisible(FALSE);

    // Set up the "dialpad chooser" UI; see showDialpadChooser().
    AutoPtr<IView> chooser;
    fragmentView->FindViewById(R::id::dialpadChooser, (IView**)&chooser);
    mDialpadChooser = IListView::Probe(chooser);
    mDialpadChooser->SetOnItemClickListener(this);

    AutoPtr<IView> floatingActionButtonContainer;
    fragmentView->FindViewById(R::id::dialpad_floating_action_button_container,
            (IView**)&floatingActionButtonContainer);
    AutoPtr<IView> floatingActionButton;
    fragmentView->FindViewById(R::id::dialpad_floating_action_button,
            (IView**)&floatingActionButton);
    floatingActionButton->SetOnClickListener(this);
    CFloatingActionButtonController::New(activity, floatingActionButtonContainer,
            floatingActionButton, mFloatingActionButtonController);

    *view = fragmentView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

Boolean DialpadFragment::IsLayoutReady()
{
    return mDigits != NULL;
}

ECode DialpadFragment::GetDigitsWidget(
    /* [out] */ IEditText** text)
{
    VALUE_NOT_NULL(text);

    *text = mDigits;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

Boolean DialpadFragment::FillDigitsIfNecessary(
    /* [in] */ IIntent* intent)
{
    // Only fills digits from an intent if it is a new intent.
    // Otherwise falls back to the previously used number.
    if (!mFirstLaunch && !mStartedFromNewIntent) {
        return FALSE;
    }

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_DIAL.Equals(action) || IIntent::ACTION_VIEW.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri != NULL) {
            String scheme;
            uri->GetScheme(&scheme);
            if (IPhoneAccount::SCHEME_TEL.Equals(scheme)) {
                // Put the requested number into the input area
                String data;
                uri->GetSchemeSpecificPart(&data);
                // Remember it is filled via Intent.
                mDigitsFilledByIntent = TRUE;
                String converted;
                PhoneNumberUtils::ConvertKeypadLettersToDigits(
                        PhoneNumberUtils::ReplaceUnicodeDigits(data), &converted);
                SetFormattedDigits(converted, NULL);
                return TRUE;
            }
            else {
                String type;
                intent->GetType(&type);
                if (IContactsPeople::CONTENT_ITEM_TYPE.Equals(type)
                        || IContactsPhones::CONTENT_ITEM_TYPE.Equals(type)) {
                    // Query the phone number
                    AutoPtr<IActivity> activity;
                    GetActivity((IActivity**)&activity);
                    AutoPtr<IContentResolver> resolver;
                    activity->GetContentResolver((IContentResolver**)&resolver);
                    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(2);
                    args->Set(0, IContactsPhonesColumns::NUMBER);
                    args->Set(0, IContactsPhonesColumns::NUMBER_KEY);

                    AutoPtr<ICursor> c;
                    resolver->Query(uri, args, NULL, NULL, NULL, (ICursor**)&c);
                    if (c != NULL) {
                        // try {
                        Boolean succeeded = FALSE;
                        if (c->MoveToFirst(&succeeded), succeeded) {
                            // Remember it is filled via Intent.
                            mDigitsFilledByIntent = TRUE;
                            // Put the number into the input area
                            String str0, str1;
                            c->GetString(0, &str0);
                            c->GetString(1, &str1);
                            SetFormattedDigits(str0, str1);
                            c->Close();
                            return TRUE;
                        }
                        // } finally {
                        //     c.close();
                        // }
                    }
                }
            }
        }
    }
    return FALSE;
}

Boolean DialpadFragment::IsAddCallMode(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_DIAL.Equals(action) || IIntent::ACTION_VIEW.Equals(action)) {
        // see if we are "adding a call" from the InCallScreen; false by default.
        Boolean extra;
        intent->GetBooleanExtra(ADD_CALL_MODE_KEY, FALSE, &extra);
        return extra;
    }
    else {
        return FALSE;
    }
}

void DialpadFragment::ConfigureScreenFromIntent(
    /* [in] */ IActivity* parent)
{
    // If we were not invoked with a DIAL intent,
    if (IDialtactsActivity::Probe(parent) == NULL) {
        SetStartedFromNewIntent(FALSE);
        return;
    }
    // See if we were invoked with a DIAL intent. If we were, fill in the appropriate
    // digits in the dialer field.
    AutoPtr<IIntent> intent;
    parent->GetIntent((IIntent**)&intent);

    if (!IsLayoutReady()) {
        // This happens typically when parent's Activity#onNewIntent() is called while
        // Fragment#onCreateView() isn't called yet, and thus we cannot configure Views at
        // this point. onViewCreate() should call this method after preparing layouts, so
        // just ignore this call now.
        Logger::I(TAG,
                "Screen configuration is requested before onCreateView() is called. Ignored");
        return;
    }

    Boolean needToShowDialpadChooser = FALSE;

    // Be sure *not* to show the dialpad chooser if this is an
    // explicit "Add call" action, though.
    Boolean isAddCallMode = IsAddCallMode(intent);
    if (!isAddCallMode) {

        // Don't show the chooser when called via onNewIntent() and phone number is present.
        // i.e. User clicks a telephone link from gmail for example.
        // In this case, we want to show the dialpad with the phone number.
        Boolean digitsFilled = FillDigitsIfNecessary(intent);
        if (!(mStartedFromNewIntent && digitsFilled)) {

            String action;
            intent->GetAction(&action);
            if (IIntent::ACTION_DIAL.Equals(action) || IIntent::ACTION_VIEW.Equals(action)
                    || IIntent::ACTION_MAIN.Equals(action)) {
                // If there's already an active call, bring up an intermediate UI to
                // make the user confirm what they really want to do.
                if (IsPhoneInUse()) {
                    needToShowDialpadChooser = TRUE;
                }
            }

        }
    }
    ShowDialpadChooser(needToShowDialpadChooser);
    SetStartedFromNewIntent(FALSE);
}

ECode DialpadFragment::SetStartedFromNewIntent(
    /* [in] */ Boolean value)
{
    mStartedFromNewIntent = value;
    return NOERROR;
}

void DialpadFragment::SetFormattedDigits(
    /* [in] */ const String& data,
    /* [in] */ const String& normalizedNumber)
{
    // strip the non-dialable numbers out of the data string.
    String dialString = PhoneNumberUtils::ExtractNetworkPortion(data);
    dialString =
            PhoneNumberUtils::FormatNumber(dialString, normalizedNumber, mCurrentCountryIso);
    if (!TextUtils::IsEmpty(dialString)) {
        AutoPtr<IEditable> digits;
        mDigits->GetEditableText((IEditable**)&digits);
        Int32 length;
        ICharSequence::Probe(digits)->GetLength(&length);
        digits->Replace(0, length, dialString);
        // for some reason this isn't getting called in the digits.replace call above..
        // but in any case, this will make sure the background drawable looks right
        AfterTextChanged(digits);
    }
}

void DialpadFragment::ConfigureKeypadListeners(
    /* [in] */ IView* fragmentView)
{
    const Int32 buttonIds[] = {
            R::id::one, R::id::two, R::id::three, R::id::four, R::id::five,
            R::id::six, R::id::seven, R::id::eight, R::id::nine, R::id::star,
            R::id::zero, R::id::pound};

    AutoPtr<IDialpadKeyButton> dialpadKey;

    Int32 length = sizeof(buttonIds) / sizeof(Int32);
    for (int i = 0; i < length; i++) {
        AutoPtr<IView> view;
        fragmentView->FindViewById(buttonIds[i], (IView**)&view);
        dialpadKey = IDialpadKeyButton::Probe(view);
        dialpadKey->SetOnPressedListener(this);
    }

    // Long-pressing one button will initiate Voicemail.
    AutoPtr<IView> one;
    fragmentView->FindViewById(R::id::one, (IView**)&one);
    IDialpadKeyButton::Probe(one)->SetOnLongClickListener(this);

    // Long-pressing zero button will enter '+' instead.
    AutoPtr<IView> zero;
    fragmentView->FindViewById(R::id::zero, (IView**)&zero);
    IDialpadKeyButton::Probe(zero)->SetOnLongClickListener(this);
}

ECode DialpadFragment::OnResume()
{
    AnalyticsFragment::OnResume();

    AutoPtr<IActivity> activity;
    GetActivity(&activity);
    mDialpadQueryListener = IDialtactsActivity::Probe(activity);

    AutoPtr<IStopWatch> stopWatch = StopWatch::Start(String("Dialpad.onResume"));

    // Query the last dialed number. Do it first because hitting
    // the DB is 'slow'. This call is asynchronous.
    QueryLastOutgoingCall();

    stopWatch->Lap(String("qloc"));

    AutoPtr<IContentResolver> contentResolver;
    activity->GetContentResolver((IContentResolver**)&contentResolver);

    // retrieve the DTMF tone play back setting.
    Int32 value;
    Settings::System::GetInt32(contentResolver,
            ISettingsSystem::DTMF_TONE_WHEN_DIALING, 1, &value);
    mDTMFToneEnabled = value == 1;

    stopWatch->Lap(String("dtwd"));

    // Retrieve the haptic feedback setting.
    assert(0 && "TODO");
    // mHaptic->CheckSystemSetting();

    stopWatch->Lap(String("hptc"));

    // if the mToneGenerator creation fails, just continue without it.  It is
    // a local audio signal, and is not as important as the dtmf tone itself.
    {    AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator == NULL) {
            // try {
            ECode ec = CToneGenerator::New(DIAL_TONE_STREAM_TYPE,
                    TONE_RELATIVE_VOLUME, (IToneGenerator**)&mToneGenerator);
            if (ec == (ECode) E_RUNTIME_EXCEPTION) {
                Logger::W(TAG, "Exception caught while creating local tone generator: %x", ec);
                mToneGenerator = NULL;
            }
            // } catch (RuntimeException e) {
            //     Log.w(TAG, "Exception caught while creating local tone generator: " + e);
            //     mToneGenerator = null;
            // }
        }
    }
    stopWatch->Lap(String("tg"));

    mPressedDialpadKeys->Clear();

    ConfigureScreenFromIntent(activity);

    stopWatch->Lap(String("fdin"));

    // While we're in the foreground, listen for phone state changes,
    // purely so that we can take down the "dialpad chooser" if the
    // phone becomes idle while the chooser UI is visible.
    GetTelephonyManager()->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_CALL_STATE);

    stopWatch->Lap(String("tm"));

    // Potentially show hint text in the mDigits field when the user
    // hasn't typed any digits yet.  (If there's already an active call,
    // this hint text will remind the user that he's about to add a new
    // call.)
    //
    // TODO: consider adding better UI for the case where *both* lines
    // are currently in use.  (Right now we let the user try to add
    // another call, but that call is guaranteed to fail.  Perhaps the
    // entire dialer UI should be disabled instead.)
    if (IsPhoneInUse()) {
        String str;
        activity->GetString(R::string::dialerDialpadHintText, &str);
        AutoPtr<ISpannableString> hint;
        CSpannableString::New(str, (ISpannableString**)&hint);
        AutoPtr<IRelativeSizeSpan> span;
        CRelativeSizeSpan::New(0.8f), (IRelativeSizeSpan**)&span);
        Int32 length;
        hint->GetLength(&length);
        hint->SetSpan(span, 0, length, 0);
        mDigits->SetHint(hint);
    }
    else {
        // Common case; no hint necessary.
        mDigits->SetHint(NULL);

        // Also, a sanity-check: the "dialpad chooser" UI should NEVER
        // be visible if the phone is idle!
        ShowDialpadChooser(FALSE);
    }

    mFirstLaunch = FALSE;

    stopWatch->Lap(String("hnt"));

    UpdateDeleteButtonEnabledState();

    stopWatch->Lap(String("bes"));

    stopWatch->StopAndLog(TAG, 50);

    mSmsPackageComponentName = DialerUtils::GetSmsComponent(activity);

    // Populate the overflow menu in onResume instead of onCreate, so that if the SMS activity
    // is disabled while Dialer is paused, the "Send a text message" option can be correctly
    // removed when resumed.
    assert(0 && "TODO");
    // mDialpadView->GetOverflowMenuButton(&mOverflowMenuButton);
    mOverflowPopupMenu = BuildOptionsMenu(mOverflowMenuButton);
    AutoPtr<IIViewOnTouchListener> listener;
    mOverflowPopupMenu->GetDragToOpenListener((IIViewOnTouchListener**)&listener);
    mOverflowMenuButton->SetOnTouchListener(listener);
    mOverflowMenuButton->SetOnClickListener(this);
    mOverflowMenuButton->SetVisibility(IsDigitsEmpty() ? IView::INVISIBLE : IView::VISIBLE);

    return NOERROR;
}

ECode DialpadFragment::OnPause()
{
    AnalyticsFragment::OnPause();

    // Stop listening for phone state changes.
    GetTelephonyManager()->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_NONE);

    // Make sure we don't leave this activity with a tone still playing.
    StopTone();
    mPressedDialpadKeys->Clear();

    {    AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator != NULL) {
            mToneGenerator->ReleaseResources();
            mToneGenerator = NULL;
        }
    }
    // TODO: I wonder if we should not check if the AsyncTask that
    // lookup the last dialed number has completed.
    mLastNumberDialed = EMPTY_NUMBER;  // Since we are going to query again, free stale number.

    SpecialCharSequenceMgr::Cleanup();

    return NOERROR;
}

ECode DialpadFragment::OnStop()
{
    AnalyticsFragment::OnStop();

    if (mClearDigitsOnStop) {
        mClearDigitsOnStop = FALSE;
        ClearDialpad();
    }
    return NOERROR;
}

ECode DialpadFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    AnalyticsFragment::OnSaveInstanceState(outState);
    outState->PutBoolean(PREF_DIGITS_FILLED_BY_INTENT, mDigitsFilledByIntent);
    return NOERROR;
}

void DialpadFragment::KeyPressed(
    /* [in] */ Int32 keyCode)
{
    AutoPtr<IView> view;
    GetView((IView**)&view);
    Int32 y;
    view->GetTranslationY(&y);
    if (y != 0) {
        return;
    }
    switch (keyCode) {
        case IKeyEvent::KEYCODE_1:
            PlayTone(IToneGenerator::TONE_DTMF_1, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_2:
            PlayTone(IToneGenerator::TONE_DTMF_2, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_3:
            PlayTone(IToneGenerator::TONE_DTMF_3, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_4:
            PlayTone(IToneGenerator::TONE_DTMF_4, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_5:
            PlayTone(IToneGenerator::TONE_DTMF_5, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_6:
            PlayTone(IToneGenerator::TONE_DTMF_6, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_7:
            PlayTone(IToneGenerator::TONE_DTMF_7, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_8:
            PlayTone(IToneGenerator::TONE_DTMF_8, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_9:
            PlayTone(IToneGenerator::TONE_DTMF_9, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_0:
            PlayTone(IToneGenerator::TONE_DTMF_0, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_POUND:
            PlayTone(IToneGenerator::TONE_DTMF_P, TONE_LENGTH_INFINITE);
            break;
        case IKeyEvent::KEYCODE_STAR:
            PlayTone(IToneGenerator::TONE_DTMF_S, TONE_LENGTH_INFINITE);
            break;
        default:
            break;
    }

    assert(0 && "TODO");
    // mHaptic->Vibrate();
    AutoPtr<IKeyEvent> event;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IKeyEvent**)&event);
    mDigits->OnKeyDown(keyCode, event);

    // If the cursor is at the end of the text we hide it.
    Int32 length;
    mDigits->GetLength(&length);
    Int32 start, end;
    ITextView::Probe(mDigits)->GetSelectionStart(&start);
    ITextView::Probe(mDigits)->GetSelectionEnd(&end);
    if (length == start && length == end) {
        ITextView::Probe(mDigits)->SetCursorVisible(FALSE);
    }
}

ECode DialpadFragment::OnKey(
    /* [in] */ IView* view,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Int32 id;
    view->GetId(&id);
    switch (id) {
        case R::id::digits:
            if (keyCode == IKeyEvent::KEYCODE_ENTER) {
                HandleDialButtonPressed();
                *result = TRUE;
                return NOERROR;
            }
            break;
    }
    *result = FALSE;
    return NOERROR;
}

ECode DialpadFragment::OnPressed(
    /* [in] */ IView* view,
    /* [in] */ Boolean pressed)
{
    if (DEBUG) {
        String str;
        view->ToString(&str);
        Logger::D(TAG, "onPressed(). view: %s, pressed: %d", str.string(), pressed);
    }

    if (pressed) {
        Int32 id;
        view->GetId(&id);
        switch (id) {
            case R::id::one: {
                KeyPressed(IKeyEvent::KEYCODE_1);
                break;
            }
            case R::id::two: {
                KeyPressed(IKeyEvent::KEYCODE_2);
                break;
            }
            case R::id::three: {
                KeyPressed(IKeyEvent::KEYCODE_3);
                break;
            }
            case R::id::four: {
                KeyPressed(IKeyEvent::KEYCODE_4);
                break;
            }
            case R::id::five: {
                KeyPressed(IKeyEvent::KEYCODE_5);
                break;
            }
            case R::id::six: {
                KeyPressed(IKeyEvent::KEYCODE_6);
                break;
            }
            case R::id::seven: {
                KeyPressed(IKeyEvent::KEYCODE_7);
                break;
            }
            case R::id::eight: {
                KeyPressed(IKeyEvent::KEYCODE_8);
                break;
            }
            case R::id::nine: {
                KeyPressed(IKeyEvent::KEYCODE_9);
                break;
            }
            case R::id::zero: {
                KeyPressed(IKeyEvent::KEYCODE_0);
                break;
            }
            case R::id::pound: {
                KeyPressed(IKeyEvent::KEYCODE_POUND);
                break;
            }
            case R::id::star: {
                KeyPressed(IKeyEvent::KEYCODE_STAR);
                break;
            }
            default: {
                String str;
                view->ToString(&str);
                Logger::Wtf(TAG, "Unexpected onTouch(ACTION_DOWN) event from: %s", str.string());
                break;
            }
        }
        mPressedDialpadKeys->Add(view);
    }
    else {
        mPressedDialpadKeys->Remove(view);
        Boolean empty;
        if (mPressedDialpadKeys->IsEmpty(&empty), empty) {
            StopTone();
        }
    }

    return NOERROR;
}

AutoPtr<IPopupMenu> DialpadFragment::BuildOptionsMenu(
    /* [in] */ IView* invoker)
{
    AutoPtr<DialpadPopupMenu> popupMenu = new DialpadPopupMenu(this);
    AutoPtr<IActivity> activity;
    GetActivity(&activity);
    popupMenu->constructor(activity, invoker);
    popupMenu->Inflate(R::menu::dialpad_options);
    popupMenu->SetOnMenuItemClickListener(this);

    return (IPopupMenu*)popupMenu;
}

ECode DialpadFragment::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);
    switch (id) {
        case R::id::dialpad_floating_action_button:
            assert(0 && "TODO");
            // mHaptic->Vibrate();
            HandleDialButtonPressed();
            break;
        case R::id::deleteButton: {
            KeyPressed(IKeyEvent::KEYCODE_DEL);
            break;
        }
        case R::id::digits: {
            if (!IsDigitsEmpty()) {
                mDigits->SetCursorVisible(TRUE);
            }
            break;
        }
        case R::id::dialpad_overflow: {
            mOverflowPopupMenu->Show();
            break;
        }
        default: {
            String str;
            view->ToString(&str);
            Logger::Wtf(TAG, "Unexpected onClick() event from: ", str);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode DialpadFragment::OnLongClick(
    /* [in] */ IView* view,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    AutoPtr<IEditable> digits;
    mDigits->GetText((IEditable**)&digits);
    Int32 id;
    view->GetId(&id);
    switch (id) {
        case R::id::deleteButton: {
            digits->Clear();
            *result = TRUE;
            return NOERROR;
        }
        case R::id::one: {
            // '1' may be already entered since we rely on onTouch() event for numeric buttons.
            // Just for safety we also check if the digits field is empty or not.
            String text;
            mDigits->GetText(&text);
            if (IsDigitsEmpty() || TextUtils::Equals(text, "1")) {
                // We'll try to initiate voicemail and thus we want to remove irrelevant string.
                RemovePreviousDigitIfPossible();

                AutoPtr<IActivity> activity;
                if (IsVoicemailAvailable()) {
                    CallVoicemail();
                }
                else if (GetActivity((IActivity**)&activity), activity != null) {
                    // Voicemail is unavailable maybe because Airplane mode is turned on.
                    // Check the current status and show the most appropriate error message.
                    AutoPtr<IContentResolver> resolver;
                    activity->GetContentResolver(&resolver);
                    Int32 value;
                    Settings::System::GetInt32(resolver,
                            ISettingsSystem::AIRPLANE_MODE_ON, 0, &value);
                    Boolean isAirplaneModeOn = value != 0;
                    if (isAirplaneModeOn) {
                        AutoPtr<DialogFragment> dialogFragment = ErrorDialogFragment::NewInstance(
                                R::string::dialog_voicemail_airplane_mode_message);
                        AutoPtr<IFragmentManager> manager;
                        GetFragmentManager((IFragmentManager**)&manager);
                        dialogFragment->Show(manager,
                                String("voicemail_request_during_airplane_mode"));
                    }
                    else {
                        AutoPtr<DialogFragment> dialogFragment = ErrorDialogFragment::NewInstance(
                                R::string::dialog_voicemail_not_ready_message);
                        AutoPtr<IFragmentManager> manager;
                        GetFragmentManager((IFragmentManager**)&manager);
                        dialogFragment->Show(manager, String("voicemail_not_ready"));
                    }
                }
                *result = TRUE;
                return NOERROR;
            }
            *result = FALSE;
            return NOERROR;
        }
        case R::id::zero: {
            // Remove tentative input ('0') done by onTouch().
            RemovePreviousDigitIfPossible();
            KeyPressed(IKeyEvent::KEYCODE_PLUS);

            // Stop tone immediately
            StopTone();
            mPressedDialpadKeys->Remove(view);

            *result = TRUE;
            return NOERROR;
        }
        case R::id::digits: {
            // Right now EditText does not show the "paste" option when cursor is not visible.
            // To show that, make the cursor visible, and return false, letting the EditText
            // show the option by itself.
            mDigits->SetCursorVisible(TRUE);
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

void DialpadFragment::RemovePreviousDigitIfPossible()
{
    Int32 currentPosition;
    ITextView::Probe(mDigits)->GetSelectionStart(&currentPosition);
    if (currentPosition > 0) {
        mDigits->SetSelection(currentPosition);
        AutoPtr<IEditableText> text;
        mDigits->GetEditableText((IEditableText**)&text);
        text->Delete(currentPosition - 1, currentPosition);
    }
}

ECode DialpadFragment::CallVoicemail()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    DialerUtils::StartActivityWithErrorToast(activity, GetVoicemailIntent());
    HideAndClearDialpad(FALSE);

    return NOERROR;
}

void DialpadFragment::HideAndClearDialpad(
    /* [in] */ Boolean animate)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IDialtactsActivity::Probe(activity)->HideDialpadFragment(animate, TRUE);
}

void DialpadFragment::HandleDialButtonPressed()
{
    if (IsDigitsEmpty()) { // No number entered.
        HandleDialButtonClickWithEmptyDigits();
    }
    else {
        ICharSequence text;
        mDigits->GetText(&text);
        String number;
        text->ToString(&number);

        // "persist.radio.otaspdial" is a temporary hack needed for one carrier's automated
        // test equipment.
        // TODO: clean it up.
        Boolean result;
        if (!number.IsNull()
                && !TextUtils::isEmpty(mProhibitedPhoneNumberRegexp)
                && StringUtils::Matches(number, mProhibitedPhoneNumberRegexp, &result), result) {
            Logger::I(TAG, "The phone number is prohibited explicitly by a rule.");
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            if (activity != NULL) {
                AutoPtr<DialogFragment> dialogFragment = ErrorDialogFragment::NewInstance(
                        R::string::dialog_phone_call_prohibited_message);
                AutoPtr<IFragmentManager> manager;
                GetFragmentManager((IFragmentManager**)&manager);
                dialogFragment->Show(manager, String("phone_prohibited_dialog"));
            }

            // Clear the digits just in case.
            ClearDialpad();
        }
        else {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            String str;
            AutoPtr<IIntent> intent = CallUtil::GetCallIntent(number,
                    IDialtactsActivity::Probe(activity) ?
                    IDialtactsActivity::Probe(activity)->GetCallOrigin(&str), str : String(NULL)));
            DialerUtils::StartActivityWithErrorToast(activity, intent);
            HideAndClearDialpad(FALSE);
        }
    }
}

ECode DialpadFragment::ClearDialpad()
{
    AutoPtr<IEditable> text;
    ITextView::Probe(mDigits)->GetEditableText(&text)
    return text->Clear();
}

void DialpadFragment::HandleDialButtonClickWithEmptyDigits()
{
    if (PhoneIsCdma() && IsPhoneInUse()) {
        // TODO: Move this logic into services/Telephony
        //
        // This is really CDMA specific. On GSM is it possible
        // to be off hook and wanted to add a 3rd party using
        // the redial feature.
        StartActivity(NewFlashIntent());
    }
    else {
        if (!TextUtils::IsEmpty(mLastNumberDialed)) {
            // Recall the last number dialed.
            mDigits->SetText(mLastNumberDialed);

            // ...and move the cursor to the end of the digits string,
            // so you'll be able to delete digits using the Delete
            // button (just as if you had typed the number manually.)
            //
            // Note we use mDigits.getText().length() here, not
            // mLastNumberDialed.length(), since the EditText widget now
            // contains a *formatted* version of mLastNumberDialed (due to
            // mTextWatcher) and its length may have changed.
            AutoPtr<ICharSequence> text;
            mDigits->GetText((ICharSequence**)&text);
            Int32 length;
            text->GetLength(&length);
            mDigits->SetSelection(length);
        }
        else {
            // There's no "last number dialed" or the
            // background query is still running. There's
            // nothing useful for the Dial button to do in
            // this case.  Note: with a soft dial button, this
            // can never happens since the dial button is
            // disabled under these conditons.
            PlayTone(IToneGenerator::TONE_PROP_NACK);
        }
    }
}

void DialpadFragment::PlayTone(
    /* [in] */ Int32 tone)
{
    PlayTone(tone, TONE_LENGTH_MS);
}

void DialpadFragment::PlayTone(
    /* [in] */ Int32 tone,
    /* [in] */ Int32 durationMs)
{
    // if local tone playback is disabled, just return.
    if (!mDTMFToneEnabled) {
        return;
    }

    // Also do nothing if the phone is in silent mode.
    // We need to re-check the ringer mode for *every* playTone()
    // call, rather than keeping a local flag that's updated in
    // onResume(), since it's possible to toggle silent mode without
    // leaving the current activity (via the ENDCALL-longpress menu.)
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> audioManager;
    IContext::Probe(activity)->GetSystemService(
            IContext::AUDIO_SERVICE, (IInterface**)&audioManager);
    Int32 ringerMode;
    IAudioManager::Probe(audioManager)->GetRingerMode(&ringerMode);
    if ((ringerMode == IAudioManager::RINGER_MODE_SILENT)
        || (ringerMode == IAudioManager::RINGER_MODE_VIBRATE)) {
        return;
    }

    {    AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator == NULL) {
            Logger::W(TAG, "playTone: mToneGenerator == null, tone: %d", tone);
            return;
        }

        // Start the new tone (will stop any playing tone)
        mToneGenerator->StartTone(tone, durationMs);
    }
}

void DialpadFragment::StopTone()
{
    // if local tone playback is disabled, just return.
    if (!mDTMFToneEnabled) {
        return;
    }
    {    AutoLock syncLock(mToneGeneratorLock);
        if (mToneGenerator == NULL) {
            Logger::W(TAG, "stopTone: mToneGenerator == null");
            return;
        }
        mToneGenerator->StopTone();
    }
}

void DialpadFragment::ShowDialpadChooser(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity == NULL) {
        return;
    }
    // Check if onCreateView() is already called by checking one of View objects.
    if (!IsLayoutReady()) {
        return;
    }

    if (enabled) {
        Logger::I(TAG, "Showing dialpad chooser!");
        if (mDialpadView != NULL) {
            mDialpadView->SetVisibility(IView::GONE);
        }

        mFloatingActionButtonController->SetVisible(FALSE);
        mDialpadChooser->SetVisibility(IView::VISIBLE);

        // Instantiate the DialpadChooserAdapter and hook it up to the
        // ListView.  We do this only once.
        if (mDialpadChooserAdapter == NULL) {
            mDialpadChooserAdapter = (IDialpadChooserAdapter*)new DialpadChooserAdapter(activity);
        }
        mDialpadChooser->SetAdapter(mDialpadChooserAdapter);
    }
    else {
        Logger::I(TAG, "Displaying normal Dialer UI.");
        if (mDialpadView != NULL) {
            mDialpadView->SetVisibility(IView::VISIBLE);
        }
        else {
            mDigits->SetVisibility(IView::VISIBLE);
        }

        mFloatingActionButtonController->SetVisible(TRUE);
        mDialpadChooser->SetVisibility(IView::GONE);
    }
}

Boolean DialpadFragment::IsDialpadChooserVisible()
{
    Boolean visible;
    mDialpadChooser->GetVisibility(&visible);
    return visible == IView::VISIBLE;
}

ECode DialpadFragment::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> view;
    parent->GetItemAtPosition(position, (IInterface**)&view);
    Int32 itemId = ((DialpadChooserAdapter::ChoiceItem*)item)->mId;
    switch (itemId) {
        case DialpadChooserAdapter::DIALPAD_CHOICE_USE_DTMF_DIALPAD:
            // Log.i(TAG, "DIALPAD_CHOICE_USE_DTMF_DIALPAD");
            // Fire off an intent to go back to the in-call UI
            // with the dialpad visible.
            ReturnToInCallScreen(TRUE);
            break;

        case DialpadChooserAdapter::DIALPAD_CHOICE_RETURN_TO_CALL:
            // Log.i(TAG, "DIALPAD_CHOICE_RETURN_TO_CALL");
            // Fire off an intent to go back to the in-call UI
            // (with the dialpad hidden).
            ReturnToInCallScreen(FALSE);
            break;

        case DialpadChooserAdapter::DIALPAD_CHOICE_ADD_NEW_CALL:
            // Log.i(TAG, "DIALPAD_CHOICE_ADD_NEW_CALL");
            // Ok, guess the user really did want to be here (in the
            // regular Dialer) after all.  Bring back the normal Dialer UI.
            ShowDialpadChooser(FALSE);
            break;

        default:
            Logger::W(TAG, "onItemClick: unexpected itemId: %s", itemId);
            break;
    }

    return NOERROR;
}

void DialpadFragment::ReturnToInCallScreen(
    /* [in] */ Boolean showDialpad)
{
    GetTelecomManager()->ShowInCallScreen(showDialpad);

    // Finally, finish() ourselves so that we don't stay on the
    // activity stack.
    // Note that we do this whether or not the showCallScreenWithDialpad()
    // call above had any effect or not!  (That call is a no-op if the
    // phone is idle, which can happen if the current call ends while
    // the dialpad chooser is up.  In this case we can't show the
    // InCallScreen, and there's no point staying here in the Dialer,
    // so we just take the user back where he came from...)
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    activity->Finish();
}

ECode DialpadFragment::IsPhoneInUse(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    GetTelecomManager()->IsInCall(&result);

    return NOERROR;
}

Boolean DialpadFragment::PhoneIsCdma()
{
    Int32 type;
    GetTelephonyManager()::GetPhoneType(&type);
    return type == ITelephonyManager::PHONE_TYPE_CDMA;
}

ECode DialpadFragment::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case R::id::menu_add_contact: {
            AutoPtr<ICharSequence> digits;
            ITextView::Probe(mDigits)->GetText((ICharSequence**)&digits);
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            DialerUtils::StartActivityWithErrorToast(activity,
                    DialtactsActivity::GetAddNumberToContactIntent(digits));
            *result = TRUE;
            return NOERROR;
        }
        case R::id::menu_2s_pause:
            UpdateDialString(PAUSE);
            *result = TRUE;
            return NOERROR;
        case R::id::menu_add_wait:
            UpdateDialString(WAIT);
            *result = TRUE;
            return NOERROR;
        case R::id::menu_send_message: {
            AutoPtr<ICharSequence> digits;
            ITextView::Probe(mDigits)->GetText((ICharSequence**)&digits);
            String str;
            digits->ToString(&str);
            AutoPtr<IUri> uri;
            Uri::FromParts(IContactsUtils::SCHEME_SMSTO, str, String(NULL), (IUri**)&uri);
            AutoPtr<IIntent> smsIntent;
            CIntent::New(IIntent::ACTION_SENDTO, uri, (IIntent**)&smsIntent);
            smsIntent->SetComponent(mSmsPackageComponentName);

            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            DialerUtils::StartActivityWithErrorToast(activity, smsIntent);
            *result = TRUE;
            return NOERROR;
        }
        default:
            *result = FALSE;
            return NOERROR;
    }
}

ECode DialpadFragment::UpdateDialString(
    /* [in] */ Char32 newDigit)
{
    if (newDigit != WAIT && newDigit != PAUSE) {
        Logger::E(TAG, "Not expected for anything other than PAUSE & WAIT");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException(
        //         "Not expected for anything other than PAUSE & WAIT");
    }

    Int32 selectionStart;
    Int32 selectionEnd;

    // SpannableStringBuilder editable_text = new SpannableStringBuilder(mDigits.getText());
    Int32 anchor;
    mDigits->GetSelectionStart(&anchor);
    Int32 point;
    mDigits->GetSelectionEnd(&point);

    selectionStart = Math::Min(anchor, point);
    selectionEnd = Math::Max(anchor, point);

    if (selectionStart == -1) {
        Int32 length;
        mDigits->GetLength(&length);
        selectionStart = selectionEnd = length;
    }

    AutoPtr<IEditable> digits;
    ITextView::Probe(mDigits)->GetEditableText((IEditable**)&digits);

    if (CanAddDigit(digits, selectionStart, selectionEnd, newDigit)) {
        digits->Replace(selectionStart, selectionEnd, String(&newDigit));

        if (selectionStart != selectionEnd) {
          // Unselect: back to a regular cursor, just pass the character inserted.
          mDigits->SetSelection(selectionStart + 1);
        }
    }

    return NOERROR;
}

void DialpadFragment::UpdateDeleteButtonEnabledState()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity == NULL) {
        return;
    }
    Boolean digitsNotEmpty = !IsDigitsEmpty();
    mDelete->SetEnabled(digitsNotEmpty);
}

void DialpadFragment::UpdateMenuOverflowButton(
    /* [in] */ Boolean transitionIn)
{
    mDialpadView->GetOverflowMenuButton((IView**)&mOverflowMenuButton);
    if (transitionIn) {
        assert(0 && "TODO");
        // AnimUtils::FadeIn(mOverflowMenuButton, IAnimUtils::DEFAULT_DURATION);
    }
    else {
        assert(0 && "TODO");
        // AnimUtils::FadeOut(mOverflowMenuButton, IAnimUtils::DEFAULT_DURATION);
    }
}

Boolean DialpadFragment::IsVoicemailAvailable()
{
    // try {
    String number;
    ECode ec = GetTelephonyManager()->GetVoiceMailNumber(&number);
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        // Possibly no READ_PHONE_STATE privilege.
        Logger::W(TAG, "SecurityException is thrown. Maybe privilege isn't sufficient.");
        return FALSE;
    }
    return !number.IsNull();
    // } catch (SecurityException se) {
    //     // Possibly no READ_PHONE_STATE privilege.
    //     Log.w(TAG, "SecurityException is thrown. Maybe privilege isn't sufficient.");
    // }
}

ECode DialpadFragment::CanAddDigit(
    /* [in] */ ICharSequence* digits,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Char32 newDigit,
    /* [out] */ Boolean* result)
{
    if(newDigit != WAIT && newDigit != PAUSE) {
        Logger::E(TAG, "Should not be called for anything other than PAUSE & WAIT");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException(
        //     "Should not be called for anything other than PAUSE & WAIT");
    }

    // False if no selection, or selection is reversed (end < start)
    if (start == -1 || end < start) {
        *result = FALSE;
        return NOERROR;
    }

    // unsupported selection-out-of-bounds state
    Int32 length;
    digits->GetLength(&length);
    if (start > length || end > length) {
        *result = FALSE;
        return NOERROR;
    }

    // Special digit cannot be the first digit
    if (start == 0) {
        *result = FALSE;
        return NOERROR;
    }

    if (newDigit == WAIT) {
        // preceding char is ';' (WAIT)
        Char32 c;
        digits->GetCharAt(start - 1, &c);
        if (c == WAIT) {
            *result = FALSE;
            return NOERROR;
        }

        // next char is ';' (WAIT)
        digits->GetCharAt(end, &c);
        if ((length > end) && (c == WAIT)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    *result = TRUE;
    return NOERROR;
}

Boolean DialpadFragment::IsDigitsEmpty()
{
    Int32 length;
    ITextView::Probe(mDigits)->GetLength(&length);
    return length == 0;
}

void DialpadFragment::QueryLastOutgoingCall()
{
    mLastNumberDialed = EMPTY_NUMBER;
    assert(0 && "TODO");
    // CallLogAsync.GetLastOutgoingCallArgs lastCallArgs =
    //         new CallLogAsync.GetLastOutgoingCallArgs(
    //             getActivity(),
    //             new CallLogAsync.OnLastOutgoingCallComplete() {
    //                 @Override
    //                 public void lastOutgoingCall(String number) {
    //                     // TODO: Filter out emergency numbers if
    //                     // the carrier does not want redial for
    //                     // these.
    //                     // If the fragment has already been detached since the last time
    //                     // we called queryLastOutgoingCall in onResume there is no point
    //                     // doing anything here.
    //                     if (getActivity() == null) return;
    //                     mLastNumberDialed = number;
    //                     updateDeleteButtonEnabledState();
    //                 }
    //             });
    // mCallLog.getLastOutgoingCall(lastCallArgs);
}

AutoPtr<IIntent> DialpadFragment::NewFlashIntent()
{
    AutoPtr<IIntent> intent = CallUtil::GetCallIntent(EMPTY_NUMBER);
    intent->PutBooleanExtra(EXTRA_SEND_EMPTY_FLASH, TRUE);
    return intent;
}

ECode DialpadFragment::OnHiddenChanged(
    /* [in] */ Boolean hidden)
{
    AnalyticsFragment::OnHiddenChanged(hidden);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IView> view;
    GetView((IView**)&view);
    AutoPtr<IView> dialpadView;
    view->FindViewById(R::id::dialpad_view, (IView**)&dialpadView);

    if (activity == NULL) return NOERROR;

    if (!hidden && !IsDialpadChooserVisible()) {
        if (mAnimate) {
            IDialpadView::Probe(dialpadView)->AnimateShow();
        }
        mFloatingActionButtonController->ScaleIn(mAnimate ? mDialpadSlideInDuration : 0);
        IDialtactsActivity::Probe(activity)->OnDialpadShown();
        mDigits->RequestFocus();
    }
    if (hidden && mAnimate) {
        mFloatingActionButtonController->ScaleOut();
    }

    return NOERROR;
}

ECode DialpadFragment::SetAnimate(
    /* [in] */ Boolean value)
{
    mAnimate = value;
    return NOERROR;
}

ECode DialpadFragment::GetAnimate(
    /* [out] */ Boolean* value)
{
    VALUE_NOT_NULL(value);

    *value = mAnimate;
    return NOERROR;
}

ECode DialpadFragment::SetYFraction(
    /* [in] */ Float yFraction)
{
    AutoPtr<IView> view;
    GetView((IView**)&view);
    return IDialpadSlidingRelativeLayout::Probe(view)->SetYFraction(yFraction);
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos
