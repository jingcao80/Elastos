
#include "elastos/droid/settings/CChooseLockPattern.h"
#include "elastos/droid/settings/CChooseLockGeneric.h"
#include "elastos/droid/settings/CEncryptionInterstitial.h"
#include "elastos/droid/settings/notification/CRedactionInterstitial.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Settings::Notification::CRedactionInterstitial;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::ILinearLayoutWithDefaultTouchRecepient;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternViewCell;
using Elastos::Droid::Internal::Widget::ILockPatternViewCellHelper;
using Elastos::Droid::Internal::Widget::CLockPatternViewCellHelper;
using Elastos::Droid::Internal::Widget::DisplayMode_Correct;
using Elastos::Droid::Internal::Widget::DisplayMode_Animate;
using Elastos::Droid::Internal::Widget::DisplayMode_Wrong;
using Elastos::Droid::Internal::Widget::EIID_IOnPatternListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {


//===============================================================================
//                  CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode
//===============================================================================
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::CANCEL = new CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode(
        String("CANCEL"), 0, R::string::cancel, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::CANCEL_DISABLED = new CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode(
        String("CANCEL_DISABLED"), 1, R::string::cancel, FALSE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::RETRY = new CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode(
        String("RETRY"), 2, R::string::lockpattern_retry_button_text, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::RETRY_DISABLED = new CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode(
        String("RETRY_DISABLED"), 3, R::string::lockpattern_retry_button_text, FALSE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::GONE = new CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode(
        String("GONE"), 4, ID_EMPTY_MESSAGE, FALSE);

CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::LeftButtonMode(
    /* [in] */ const String& name,
    /* [in] */ Int32 ordinal,
    /* [in] */ Int32 text,
    /* [in] */ Boolean enabled)
    : mText(text)
    , mEnabled(enabled)
    , mName(name)
    , mOrdinal(ordinal)
{}

//===============================================================================
//                  CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode
//===============================================================================
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONTINUE = new CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode(
        String("CONTINUE"), 0, R::string::lockpattern_continue_button_text, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONTINUE_DISABLED = new CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode(
        String("CONTINUE_DISABLED"), 1, R::string::lockpattern_continue_button_text, FALSE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONFIRM = new CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode(
        String("CONFIRM"), 2, R::string::lockpattern_confirm_button_text, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONFIRM_DISABLED = new CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode(
        String("CONFIRM_DISABLED"), 3, R::string::lockpattern_confirm_button_text, FALSE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode>
CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::OK = new CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode(
        String("OK"), 4, Elastos::Droid::R::string::ok, TRUE);

CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::RightButtonMode(
    /* [in] */ const String& name,
    /* [in] */ Int32 ordinal,
    /* [in] */ Int32 text,
    /* [in] */ Boolean enabled)
    : mText(text)
    , mEnabled(enabled)
    , mName(name)
    , mOrdinal(ordinal)
{}

//===============================================================================
//                  CChooseLockPattern::ChooseLockPatternFragment::Stage
//===============================================================================
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::Stage>
CChooseLockPattern::ChooseLockPatternFragment::Stage::INTRODUCTION = new CChooseLockPattern::ChooseLockPatternFragment::Stage(
        String("INTRODUCTION"), 0, R::string::lockpattern_recording_intro_header,
        CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::CANCEL,
        CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONTINUE_DISABLED,
        ID_EMPTY_MESSAGE, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::Stage>
CChooseLockPattern::ChooseLockPatternFragment::Stage::HELP_SCREEN = new CChooseLockPattern::ChooseLockPatternFragment::Stage(
        String("HELP_SCREEN"), 1, R::string::lockpattern_settings_help_how_to_record,
        CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::GONE,
        CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::OK,
        ID_EMPTY_MESSAGE, FALSE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::Stage>
CChooseLockPattern::ChooseLockPatternFragment::Stage::CHOICE_TOO_SHORT = new CChooseLockPattern::ChooseLockPatternFragment::Stage(
        String("CHOICE_TOO_SHORT"), 2, R::string::lockpattern_recording_incorrect_too_short,
        CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::RETRY,
        CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONTINUE_DISABLED,
        ID_EMPTY_MESSAGE, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::Stage>
CChooseLockPattern::ChooseLockPatternFragment::Stage::FIRST_CHOICE_VALID = new CChooseLockPattern::ChooseLockPatternFragment::Stage(
        String("FIRST_CHOICE_VALID"), 3, R::string::lockpattern_pattern_entered_header,
        CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::RETRY,
        CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONTINUE,
        ID_EMPTY_MESSAGE, FALSE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::Stage>
CChooseLockPattern::ChooseLockPatternFragment::Stage::NEED_TO_CONFIRM = new CChooseLockPattern::ChooseLockPatternFragment::Stage(
        String("NEED_TO_CONFIRM"), 4, R::string::lockpattern_need_to_confirm,
        CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::CANCEL,
        CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONFIRM_DISABLED,
        ID_EMPTY_MESSAGE, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::Stage>
CChooseLockPattern::ChooseLockPatternFragment::Stage::CONFIRM_WRONG = new CChooseLockPattern::ChooseLockPatternFragment::Stage(
        String("CONFIRM_WRONG"), 5, R::string::lockpattern_need_to_unlock_wrong,
        CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::CANCEL,
        CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONFIRM_DISABLED,
        ID_EMPTY_MESSAGE, TRUE);
AutoPtr<CChooseLockPattern::ChooseLockPatternFragment::Stage>
CChooseLockPattern::ChooseLockPatternFragment::Stage::CHOICE_CONFIRMED = new CChooseLockPattern::ChooseLockPatternFragment::Stage(
        String("CHOICE_CONFIRMED"), 6, R::string::lockpattern_pattern_confirmed_header,
        CChooseLockPattern::ChooseLockPatternFragment::LeftButtonMode::CANCEL,
        CChooseLockPattern::ChooseLockPatternFragment::RightButtonMode::CONFIRM,
        ID_EMPTY_MESSAGE, FALSE);

AutoPtr< ArrayOf<CChooseLockPattern::ChooseLockPatternFragment::Stage*> >
CChooseLockPattern::ChooseLockPatternFragment::Stage::sSharedConstantsCache;

CAR_INTERFACE_IMPL(CChooseLockPattern::ChooseLockPatternFragment::Stage, Object, IPatternStage)

CChooseLockPattern::ChooseLockPatternFragment::Stage::Stage(
    /* [in] */ const String& name,
    /* [in] */ Int32 ordinal,
    /* [in] */ Int32 headerMessage,
    /* [in] */ LeftButtonMode* leftMode,
    /* [in] */ RightButtonMode* rightMode,
    /* [in] */ Int32 footerMessage,
    /* [in] */ Boolean patternEnabled)
    : mHeaderMessage(headerMessage)
    , mLeftMode(leftMode)
    , mRightMode(rightMode)
    , mFooterMessage(footerMessage)
    , mPatternEnabled(patternEnabled)
    , mName(name)
    , mOrdinal(ordinal)
{}

Int32 CChooseLockPattern::ChooseLockPatternFragment::Stage::GetOrdinal()
{
    return mOrdinal;
}

AutoPtr< ArrayOf<CChooseLockPattern::ChooseLockPatternFragment::Stage*> >
CChooseLockPattern::ChooseLockPatternFragment::Stage::GetValues()
{
    if (sSharedConstantsCache == NULL) {
        sSharedConstantsCache = ArrayOf<Stage*>::Alloc(7);
        sSharedConstantsCache->Set(0, INTRODUCTION);
        sSharedConstantsCache->Set(1, HELP_SCREEN);
        sSharedConstantsCache->Set(2, CHOICE_TOO_SHORT);
        sSharedConstantsCache->Set(3, FIRST_CHOICE_VALID);
        sSharedConstantsCache->Set(4, NEED_TO_CONFIRM);
        sSharedConstantsCache->Set(5, CONFIRM_WRONG);
        sSharedConstantsCache->Set(6, CHOICE_CONFIRMED);
    }

    return sSharedConstantsCache;
}

//===============================================================================
//                   CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener
//===============================================================================

CAR_INTERFACE_IMPL( CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener, Object, IOnPatternListener)

CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener::LockPatternViewOnPatternListener(
    /* [in] */ ChooseLockPatternFragment* host)
    : mHost(host)
{}

ECode CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener::OnPatternStart()
{
    Boolean res;
    IView::Probe(mHost->mLockPatternView)->RemoveCallbacks(mHost->mClearPatternRunnable, &res);
    PatternInProgress();
    return NOERROR;
}

ECode CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener::OnPatternCleared()
{
    Boolean res;
    return IView::Probe(mHost->mLockPatternView)->RemoveCallbacks(mHost->mClearPatternRunnable, &res);
}

ECode CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener::OnPatternDetected(
    /* [in] */ IList* pattern)
{
    Slogger::I("CChooseLockPatternFragment", ">> enter OnPatternDetected ");
    if (mHost->mUiStage == Stage::NEED_TO_CONFIRM || mHost->mUiStage == Stage::CONFIRM_WRONG) {
        if (mHost->mChosenPattern == NULL) {
            Slogger::E("CChooseLockPatternFragment::LockPatternViewOnPatternListener",
                    "NULL chosen pattern in stage 'need to confirm");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Boolean res;
        if (mHost->mChosenPattern->Equals(pattern, &res), res) {
            mHost->UpdateStage(Stage::CHOICE_CONFIRMED);
        }
        else {
            mHost->UpdateStage(Stage::CONFIRM_WRONG);
        }
    }
    else if (mHost->mUiStage == Stage::INTRODUCTION || mHost->mUiStage == Stage::CHOICE_TOO_SHORT){
        Int32 size;
        if ((pattern->GetSize(&size), size) < ILockPatternUtils::MIN_LOCK_PATTERN_SIZE) {
            mHost->UpdateStage(Stage::CHOICE_TOO_SHORT);
        }
        else {
            mHost->mChosenPattern = NULL;
            CArrayList::New(ICollection::Probe(pattern), (IList**)&mHost->mChosenPattern);
            mHost->UpdateStage(Stage::FIRST_CHOICE_VALID);
        }
    }
    else {
        Slogger::E("CChooseLockPatternFragment::LockPatternViewOnPatternListener",
                "Unexpected stage %s when entering the pattern.", TO_CSTR(mHost->mUiStage));
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Slogger::I("CChooseLockPatternFragment", "<< leave OnPatternDetected ");
    return NOERROR;
}

ECode CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener::OnPatternCellAdded(
    /* [in] */ IList* pattern)
{
    return NOERROR;
}

void CChooseLockPattern::ChooseLockPatternFragment::LockPatternViewOnPatternListener::PatternInProgress()
{
    mHost->mHeaderText->SetText(R::string::lockpattern_recording_inprogress);
    mHost->mFooterText->SetText(CoreUtils::Convert(""));
    IView::Probe(mHost->mFooterLeftButton)->SetEnabled(FALSE);
    IView::Probe(mHost->mFooterRightButton)->SetEnabled(FALSE);
}

//===============================================================================
//                  CChooseLockPattern::ChooseLockPatternFragment::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CChooseLockPattern::ChooseLockPatternFragment::InnerListener, Object, IViewOnClickListener)

CChooseLockPattern::ChooseLockPatternFragment::InnerListener::InnerListener(
    /* [in] */ ChooseLockPatternFragment* host)
    : mHost(host)
{}

ECode CChooseLockPattern::ChooseLockPatternFragment::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//===============================================================================
//                  CChooseLockPattern::ChooseLockPatternFragment::ClearPatternRunnable
//===============================================================================

CChooseLockPattern::ChooseLockPatternFragment::ClearPatternRunnable::ClearPatternRunnable(
    /* [in] */ ChooseLockPatternFragment* host)
    : mHost(host)
{}

ECode CChooseLockPattern::ChooseLockPatternFragment::ClearPatternRunnable::Run()
{
    return mHost->mLockPatternView->ClearPattern();
}

//===============================================================================
//                  CChooseLockPattern::ChooseLockPatternFragment
//===============================================================================
const Int32 CChooseLockPattern::ChooseLockPatternFragment::CONFIRM_EXISTING_REQUEST = 55;
const Int32 CChooseLockPattern::ChooseLockPatternFragment::INFORMATION_MSG_TIMEOUT_MS = 3000;
const Int32 CChooseLockPattern::ChooseLockPatternFragment::WRONG_PATTERN_CLEAR_TIMEOUT_MS = 2000;
const Int32 CChooseLockPattern::ChooseLockPatternFragment::ID_EMPTY_MESSAGE;
const String CChooseLockPattern::ChooseLockPatternFragment::KEY_UI_STAGE("uiStage");
const String CChooseLockPattern::ChooseLockPatternFragment::KEY_PATTERN_CHOICE("chosenPattern");

CChooseLockPattern::ChooseLockPatternFragment::ChooseLockPatternFragment()
    : mPatternSize(ILockPatternUtils::PATTERN_SIZE_DEFAULT)
    , mDone(FALSE)
{}

CChooseLockPattern::ChooseLockPatternFragment::~ChooseLockPatternFragment()
{}

ECode CChooseLockPattern::ChooseLockPatternFragment::constructor()
{
    mChooseNewLockPatternListener = new LockPatternViewOnPatternListener(this);
    mUiStage = Stage::INTRODUCTION;
    mClearPatternRunnable = new ClearPatternRunnable(this);
    return Fragment::constructor();
}

ECode CChooseLockPattern::ChooseLockPatternFragment::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    Slogger::I("CChooseLockPatternFragment", ">> enter OnActivityResult");
    Fragment::OnActivityResult(requestCode, resultCode, data);
    switch (requestCode) {
        case CONFIRM_EXISTING_REQUEST: {
            if (resultCode != IActivity::RESULT_OK) {
                AutoPtr<IActivity> activity;
                GetActivity((IActivity**)&activity);
                activity->SetResult(RESULT_FINISHED);
                activity->Finish();
            }
            UpdateStage(Stage::INTRODUCTION);
            break;
        }
    }
    Slogger::I("CChooseLockPatternFragment", "<< leave OnActivityResult");
    return NOERROR;
}

ECode CChooseLockPattern::ChooseLockPatternFragment::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Fragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mChooseLockSettingsHelper = new ChooseLockSettingsHelper();
    mChooseLockSettingsHelper->constructor(activity);
    if (IChooseLockPattern::Probe(activity) == NULL) {
        Slogger::E("CChooseLockPatternFragment", "Fragment contained in wrong activity");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CChooseLockPattern::ChooseLockPatternFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Slogger::I("CChooseLockPatternFragment", ">> enter OnCreateView");

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    intent->GetByteExtra(String("pattern_size"), ILockPatternUtils::PATTERN_SIZE_DEFAULT, &mPatternSize);
    AutoPtr<ILockPatternViewCellHelper> helper;
    CLockPatternViewCellHelper::AcquireSingleton((ILockPatternViewCellHelper**)&helper);
    helper->UpdateSize(mPatternSize);
    AutoPtr<ILockPatternViewCell> lpvc1, lpvc2, lpvc3, lpvc4;
    helper->Of(0, 0, mPatternSize, (ILockPatternViewCell**)&lpvc1);
    helper->Of(0, 1, mPatternSize, (ILockPatternViewCell**)&lpvc2);
    helper->Of(1, 1, mPatternSize, (ILockPatternViewCell**)&lpvc3);
    helper->Of(2, 1, mPatternSize, (ILockPatternViewCell**)&lpvc4);

    AutoPtr< ArrayOf<IInterface*> > elements = ArrayOf<IInterface*>::Alloc(4);
    elements->Set(0, lpvc1);
    elements->Set(1, lpvc2);
    elements->Set(2, lpvc3);
    elements->Set(3, lpvc4);

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    Int32 capacity = (elements->GetLength() * 110) / 100 + 5;
    AutoPtr<IList> list;
    CArrayList::New(capacity, (IList**)&list);
    coll->AddAll(ICollection::Probe(list), elements);

    coll->UnmodifiableList(list, (IList**)&mAnimatePattern);

    // SetupViews()
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::choose_lock_pattern, NULL, (IView**)&view);
    AutoPtr<IView> tmp;
    view->FindViewById(R::id::headerText, (IView**)&tmp);
    mHeaderText = ITextView::Probe(tmp);
    tmp = NULL;
    view->FindViewById(R::id::lockPattern, (IView**)&tmp);
    mLockPatternView = ILockPatternView::Probe(tmp);
    mLockPatternView->SetOnPatternListener(mChooseNewLockPatternListener);
    Boolean res;
    mLockPatternView->SetTactileFeedbackEnabled(
            (mChooseLockSettingsHelper->Utils()->IsTactileFeedbackEnabled(&res), res));
    mLockPatternView->SetLockPatternSize(mPatternSize);
    mLockPatternView->SetLockPatternUtils(mChooseLockSettingsHelper->Utils());

    tmp = NULL;
    view->FindViewById(R::id::footerText, (IView**)&tmp);
    mFooterText = ITextView::Probe(tmp);

    AutoPtr<IView> footerLeftButtonTmp;
    view->FindViewById(R::id::footerLeftButton, (IView**)&footerLeftButtonTmp);
    mFooterLeftButton = ITextView::Probe(footerLeftButtonTmp);
    AutoPtr<IView> footerRightButtonTmp;
    view->FindViewById(R::id::footerRightButton, (IView**)&footerRightButtonTmp);
    mFooterRightButton = ITextView::Probe(footerRightButtonTmp);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    footerLeftButtonTmp->SetOnClickListener(listener);
    footerRightButtonTmp->SetOnClickListener(listener);

    // make it so unhandled touch events within the unlock screen go to the
    // lock pattern view.
    tmp = NULL;
    view->FindViewById(R::id::topLayout, (IView**)&tmp);
    AutoPtr<ILinearLayoutWithDefaultTouchRecepient> topLayout = ILinearLayoutWithDefaultTouchRecepient::Probe(tmp);
    topLayout->SetDefaultTouchRecepient(IView::Probe(mLockPatternView));

    Boolean confirmCredentials;
    intent->GetBooleanExtra(String("confirm_credentials"), TRUE, &confirmCredentials);

    if (savedInstanceState == NULL) {
        if (confirmCredentials) {
            // first launch. As a security measure, we're in NeedToConfirm mode until we
            // know there isn't an existing password or the user confirms their password.
            UpdateStage(Stage::NEED_TO_CONFIRM);
            Boolean launchedConfirmationActivity =
                mChooseLockSettingsHelper->LaunchConfirmationActivity(
                        CONFIRM_EXISTING_REQUEST, NULL, NULL);
            if (!launchedConfirmationActivity) {
                UpdateStage(Stage::INTRODUCTION);
            }
        }
        else {
            UpdateStage(Stage::INTRODUCTION);
        }
    }
    else {
        // restore from previous state
        String patternString;
        savedInstanceState->GetString(KEY_PATTERN_CHOICE, &patternString);
        if (!patternString.IsNull()) {
            mChosenPattern = NULL;
            mChooseLockSettingsHelper->Utils()->StringToPattern(patternString, (IList**)&mChosenPattern);
        }
        Int32 key;
        savedInstanceState->GetInt32(KEY_UI_STAGE, &key);
        AutoPtr< ArrayOf<Stage*> > values = Stage::GetValues();
        UpdateStage((*values)[key]);
    }
    mDone = FALSE;

    *result = view;
    REFCOUNT_ADD(*result)
    Slogger::I("CChooseLockPatternFragment", "<< leave OnCreateView");
    return NOERROR;
}

ECode CChooseLockPattern::ChooseLockPatternFragment::OnClick(
    /* [in] */ IView* v)
{
    Slogger::I("CChooseLockPatternFragment", ">> enter OnClick ");
    if (v == IView::Probe(mFooterLeftButton)) {
        if (mUiStage->mLeftMode == LeftButtonMode::RETRY) {
            mChosenPattern = NULL;
            mLockPatternView->ClearPattern();
            UpdateStage(Stage::INTRODUCTION);
        }
        else if (mUiStage->mLeftMode == LeftButtonMode::CANCEL) {
            // They are canceling the entire wizard
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            activity->SetResult(RESULT_FINISHED);
            activity->Finish();
        }
        else {
            Slogger::E("CChooseLockPatternFragment",
                    "left footer button pressed, but stage of %s doesn't make sense", TO_CSTR(mUiStage));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else if (v == IView::Probe(mFooterRightButton)) {

        if (mUiStage->mRightMode == RightButtonMode::CONTINUE) {
            if (mUiStage != Stage::FIRST_CHOICE_VALID) {
                Slogger::E("CChooseLockPatternFragment", "expected ui stage %s when button is %s",
                        TO_CSTR(Stage::FIRST_CHOICE_VALID), TO_CSTR(RightButtonMode::CONTINUE));
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            UpdateStage(Stage::NEED_TO_CONFIRM);
        }
        else if (mUiStage->mRightMode == RightButtonMode::CONFIRM) {
            if (mUiStage != Stage::CHOICE_CONFIRMED) {
                Slogger::E("CChooseLockPatternFragment", "expected ui stage %s when button is %s",
                        TO_CSTR(Stage::CHOICE_CONFIRMED), TO_CSTR(RightButtonMode::CONFIRM));
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            SaveChosenPatternAndFinish();
        }
        else if (mUiStage->mRightMode == RightButtonMode::OK) {
            if (mUiStage != Stage::HELP_SCREEN) {
                Slogger::E("CChooseLockPatternFragment",
                        "Help screen is only mode with ok button, but stage is %s", TO_CSTR(mUiStage));
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            mLockPatternView->ClearPattern();
            mLockPatternView->SetDisplayMode(DisplayMode_Correct);
            UpdateStage(Stage::INTRODUCTION);
        }
    }
    Slogger::I("CChooseLockPatternFragment", "<< leave OnClick ");
    return NOERROR;
}

ECode CChooseLockPattern::ChooseLockPatternFragment::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Slogger::I("CChooseLockPatternFragment", ">> enter OnKeyDown ");
    Int32 data;
    if (keyCode == IKeyEvent::KEYCODE_BACK && (event->GetRepeatCount(&data), data == 0)) {
        if (mUiStage == Stage::HELP_SCREEN) {
            UpdateStage(Stage::INTRODUCTION);
            *result = TRUE;
            return NOERROR;
        }
    }
    if (keyCode == IKeyEvent::KEYCODE_MENU && mUiStage == Stage::INTRODUCTION) {
        UpdateStage(Stage::HELP_SCREEN);
        *result = TRUE;
        return NOERROR;
    }
    Slogger::I("CChooseLockPatternFragment", "<< leave OnKeyDown ");
    return NOERROR;
}

ECode CChooseLockPattern::ChooseLockPatternFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Fragment::OnSaveInstanceState(outState);

    outState->PutInt32(KEY_UI_STAGE, mUiStage->GetOrdinal());
    if (mChosenPattern != NULL) {
        String str;
        mChooseLockSettingsHelper->Utils()->PatternToString(mChosenPattern, &str);
        outState->PutString(KEY_PATTERN_CHOICE, str);
    }
    return NOERROR;
}

void CChooseLockPattern::ChooseLockPatternFragment::UpdateStage(
    /* [in] */ Stage* stage)
{
    Slogger::I("CChooseLockPatternFragment", ">> enter UpdateStage ");
    AutoPtr<Stage> previousStage = mUiStage;

    mUiStage = stage;

    // header text, footer text, visibility and
    // enabled state all known from the stage
    if (stage == Stage::CHOICE_TOO_SHORT.Get()) {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(ILockPatternUtils::MIN_LOCK_PATTERN_SIZE));
        String str;
        resources->GetString(stage->mHeaderMessage, args, &str);
        mHeaderText->SetText(CoreUtils::Convert(str));
    }
    else {
        mHeaderText->SetText(stage->mHeaderMessage);
    }
    if (stage->mFooterMessage == ID_EMPTY_MESSAGE) {
        mFooterText->SetText(CoreUtils::Convert(""));
    }
    else {
        mFooterText->SetText(stage->mFooterMessage);
    }

    if (stage->mLeftMode == LeftButtonMode::GONE) {
        IView::Probe(mFooterLeftButton)->SetVisibility(IView::GONE);
    }
    else {
        IView* footerLeftButton = IView::Probe(mFooterLeftButton);
        footerLeftButton->SetVisibility(IView::VISIBLE);
        mFooterLeftButton->SetText(stage->mLeftMode->mText);
        footerLeftButton->SetEnabled(stage->mLeftMode->mEnabled);
    }

    mFooterRightButton->SetText(stage->mRightMode->mText);
    IView::Probe(mFooterRightButton)->SetEnabled(stage->mRightMode->mEnabled);

    // same for whether the patten is enabled
    if (stage->mPatternEnabled) {
        mLockPatternView->EnableInput();
    }
    else {
        mLockPatternView->DisableInput();
    }

    // the rest of the stuff varies enough that it is easier just to handle
    // on a case by case basis.
    mLockPatternView->SetDisplayMode(DisplayMode_Correct);

    if (mUiStage == Stage::INTRODUCTION) {
        mLockPatternView->ClearPattern();
    }
    else if (mUiStage == Stage::HELP_SCREEN) {
        mLockPatternView->SetPattern(DisplayMode_Animate, mAnimatePattern);
    }
    else if (mUiStage == Stage::CHOICE_TOO_SHORT) {
        mLockPatternView->SetDisplayMode(DisplayMode_Wrong);
        PostClearPatternRunnable();
    }
    else if (mUiStage == Stage::FIRST_CHOICE_VALID) {}
    else if (mUiStage == Stage::NEED_TO_CONFIRM) {
        mLockPatternView->ClearPattern();
    }
    else if (mUiStage == Stage::CONFIRM_WRONG) {
        mLockPatternView->SetDisplayMode(DisplayMode_Wrong);
        PostClearPatternRunnable();
    }
    else if (mUiStage == Stage::CHOICE_CONFIRMED) {}

    // If the stage changed, announce the header for accessibility. This
    // is a no-op when accessibility is disabled.
    if (previousStage.Get() != stage) {
        AutoPtr<ICharSequence> cs;
        mHeaderText->GetText((ICharSequence**)&cs);
        IView::Probe(mHeaderText)->AnnounceForAccessibility(cs);
    }
    Slogger::I("CChooseLockPatternFragment", "<< leave UpdateStage ");
}

void CChooseLockPattern::ChooseLockPatternFragment::PostClearPatternRunnable()
{
    Boolean res;
    IView* lpv = IView::Probe(mLockPatternView);
    lpv->RemoveCallbacks(mClearPatternRunnable, &res);
    lpv->PostDelayed(mClearPatternRunnable, WRONG_PATTERN_CLEAR_TIMEOUT_MS, &res);
}

void CChooseLockPattern::ChooseLockPatternFragment::SaveChosenPatternAndFinish()
{
    Slogger::I("CChooseLockPatternFragment", ">> enter SaveChosenPatternAndFinish ");
    if (mDone) return;
    AutoPtr<ILockPatternUtils> utils = mChooseLockSettingsHelper->Utils();
    Boolean lockVirgin;
    utils->IsPatternEverChosen(&lockVirgin);
    lockVirgin = !lockVirgin;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Boolean isFallback;
    intent->GetBooleanExtra(ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, FALSE, &isFallback);

    Boolean required;
    intent->GetBooleanExtra(CEncryptionInterstitial::EXTRA_REQUIRE_PASSWORD, TRUE, &required);
    utils->SetCredentialRequiredToDecrypt(required);
    utils->SetLockPatternSize(mPatternSize);
    utils->SaveLockPattern(mChosenPattern, isFallback);
    utils->SetLockPatternEnabled(TRUE);

    if (lockVirgin) {
        utils->SetVisiblePatternEnabled(TRUE);
    }

    activity->SetResult(RESULT_FINISHED);
    activity->Finish();
    mDone = TRUE;
    StartActivity(CRedactionInterstitial::CreateStartIntent(IContext::Probe(activity)));
    Slogger::I("CChooseLockPatternFragment", "<< leave SaveChosenPatternAndFinish ");
}

//===============================================================================
//                  CChooseLockPattern
//===============================================================================

const Int32 CChooseLockPattern::RESULT_FINISHED = RESULT_FIRST_USER;

CAR_INTERFACE_IMPL(CChooseLockPattern, SettingsActivity, IChooseLockPattern)

CAR_OBJECT_IMPL(CChooseLockPattern)

CChooseLockPattern::CChooseLockPattern()
{}

CChooseLockPattern::~CChooseLockPattern()
{}

ECode CChooseLockPattern::constructor()
{
    return SettingsActivity::constructor();
}

ECode CChooseLockPattern::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = NULL;

    AutoPtr<IIntent> oneIntent;
    SettingsActivity::GetIntent((IIntent**)&oneIntent);
    AutoPtr<IIntent> modIntent;
    CIntent::New(oneIntent, (IIntent**)&modIntent);
    modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, String("Elastos.Droid.Settings.CChooseLockPatternFragment"));
    *intent = modIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

AutoPtr<IIntent> CChooseLockPattern::CreateIntent(
    /* [in] */ IContext* context,
    /* [in] */ Boolean isFallback,
    /* [in] */ Boolean requirePassword,
    /* [in] */ Boolean confirmCredentials)
{
    AutoPtr<IIntent> intent;
    CIntent::New(context, ECLSID_CChooseLockPattern, (IIntent**)&intent);
    intent->PutExtra(String("key_lock_method"), String("pattern"));
    intent->PutBooleanExtra(CChooseLockGeneric::CONFIRM_CREDENTIALS, confirmCredentials);
    intent->PutBooleanExtra(ILockPatternUtils::LOCKSCREEN_BIOMETRIC_WEAK_FALLBACK, isFallback);
    intent->PutBooleanExtra(CEncryptionInterstitial::EXTRA_REQUIRE_PASSWORD, requirePassword);
    return intent;
}

Boolean CChooseLockPattern::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    if (String("Elastos.Droid.Settings.CChooseLockPatternFragment").Equals(fragmentName)) return TRUE;
    return FALSE;
}

ECode CChooseLockPattern::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    // RequestWindowFeature(Window.FEATURE_NO_TITLE);
    SettingsActivity::OnCreate(savedInstanceState);
    AutoPtr<ICharSequence> msg;
    GetText(R::string::lockpassword_choose_your_pattern_header, (ICharSequence**)&msg);
    SetTitle(msg);
    return NOERROR;
}

ECode CChooseLockPattern::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // *** TODO ***
    // chooseLockPatternFragment->OnKeyDown(keyCode, event);
    return SettingsActivity::OnKeyDown(keyCode, event, result);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos