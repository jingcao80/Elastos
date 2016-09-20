#include "elastos/droid/providers/media/RingtonePickerActivity.h"
#include "Elastos.CoreLibrary.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.View.h"
#include "R.h"
#include "elastos/droid/R.h"
#include <elastos/droid/widget/AbsListView.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::App::CActivity;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContextWrapper;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::App::CAlertControllerAlertParams;
using Elastos::Droid::Internal::App::EIID_IAlertControllerAlertParamsOnPrepareListViewListener;
using Elastos::Droid::Internal::App::IAlertActivity;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Internal::App::IAlertControllerAlertParamsOnPrepareListViewListener;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManager;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::AbsListView;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::CListView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

//===========================================================
//  RingtonePickerActivity::RingtoneClickListener
//===========================================================
CAR_INTERFACE_IMPL(RingtonePickerActivity::RingtoneClickListener, Object, \
    IDialogInterfaceOnClickListener)

RingtonePickerActivity::RingtoneClickListener::RingtoneClickListener(
    /* [in] */ RingtonePickerActivity* owner)
    : mOwner(owner)
{}

ECode RingtonePickerActivity::RingtoneClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // Save the position of most recently clicked item
    mOwner->mClickedPos = which;

    // Play clip
    mOwner->PlayRingtone(which, 0);
    return NOERROR;
}

//===========================================================
//  RingtonePickerActivity::InnerListener
//===========================================================
CAR_INTERFACE_IMPL_3(RingtonePickerActivity::InnerListener, Object, \
    IDialogInterfaceOnClickListener,
    IAdapterViewOnItemSelectedListener,
    IAlertControllerAlertParamsOnPrepareListViewListener)

RingtonePickerActivity::InnerListener::InnerListener(
    /* [in] */ RingtonePickerActivity* owner)
    : mOwner(owner)
{}

ECode RingtonePickerActivity::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mOwner->OnClick(dialog, which);
}

ECode RingtonePickerActivity::InnerListener::OnPrepareListView(
    /* [in] */ IListView* listView)
{
    return mOwner->OnPrepareListView(listView);
}

ECode RingtonePickerActivity::InnerListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mOwner->OnItemSelected(parent, view, position, id);
}

ECode RingtonePickerActivity::InnerListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return mOwner->OnNothingSelected(parent);
}

//===========================================================
//  RingtonePickerActivity::MyRunnable
//===========================================================
RingtonePickerActivity::MyRunnable::MyRunnable(
    /* [in] */ RingtonePickerActivity* owner)
    : mOwner(owner)
{}

ECode RingtonePickerActivity::MyRunnable::Run()
{
    return mOwner->mCursor->Deactivate();
}

//===========================================================
//  RingtonePickerActivity::InnerRunnable
//===========================================================
RingtonePickerActivity::InnerRunnable::InnerRunnable(
    /* [in] */ RingtonePickerActivity* owner)
    : mOwner(owner)
{}

ECode RingtonePickerActivity::InnerRunnable::Run()
{
    return mOwner->Run();
}

const Int32 RingtonePickerActivity::POS_UNKNOWN = -1;
const String RingtonePickerActivity::TAG("RingtonePickerActivity");
const Int32 RingtonePickerActivity::DELAY_MS_SELECTION_PLAYED = 300;
const String RingtonePickerActivity::SAVE_CLICKED_POS("clicked_pos");

AutoPtr<IRingtone> RingtonePickerActivity::sPlayingRingtone;

CAR_INTERFACE_IMPL_2(RingtonePickerActivity, AlertActivity, IRunnable, IRingtonePickerActivity);

RingtonePickerActivity::RingtonePickerActivity()
    : mType(0)
    , mSilentPos(0)
    , mDefaultRingtonePos(POS_UNKNOWN)
    , mClickedPos(POS_UNKNOWN)
    , mSampleRingtonePos(POS_UNKNOWN)
    , mHasSilentItem(FALSE)
    , mStaticItemCount(0)
    , mHasDefaultItem(FALSE)
{
}

RingtonePickerActivity::~RingtonePickerActivity()
{
}

ECode RingtonePickerActivity::constructor()
{
    return AlertActivity::constructor();
}

ECode RingtonePickerActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate");
    AlertActivity::OnCreate(savedInstanceState);

    mInnerRunnable = new InnerRunnable(this);
    mInnerListener = new InnerListener(this);
    mRingtoneClickListener = new RingtoneClickListener(this);
    CHandler::New((IHandler**)&mHandler);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    /*
     * Get whether to show the 'Default' item, and the URI to play when the
     * default is clicked
     */
    intent->GetBooleanExtra(IRingtoneManager::EXTRA_RINGTONE_SHOW_DEFAULT, TRUE, &mHasDefaultItem);
    AutoPtr<IParcelable> pl;
    intent->GetParcelableExtra(IRingtoneManager::EXTRA_RINGTONE_DEFAULT_URI, (IParcelable**)&pl);
    mUriForDefaultItem = IUri::Probe(pl);
    if (mUriForDefaultItem == NULL) {
        AutoPtr<ISettingsSystem> ssy;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ssy);
        mUriForDefaultItem = NULL;
        ssy->GetDEFAULT_RINGTONE_URI((IUri**)&mUriForDefaultItem);
    }

    if (savedInstanceState != NULL) {
        savedInstanceState->GetInt32(SAVE_CLICKED_POS, POS_UNKNOWN, &mClickedPos);
    }
    // Get whether to show the 'Silent' item
    intent->GetBooleanExtra(IRingtoneManager::EXTRA_RINGTONE_SHOW_SILENT, TRUE, &mHasSilentItem);

    // Give the Activity so it can do managed queries
    mRingtoneManager = NULL;
    CRingtoneManager::New((IActivity*)this, (IRingtoneManager**)&mRingtoneManager);

    // Get the types of ringtones to show
    intent->GetInt32Extra(IRingtoneManager::EXTRA_RINGTONE_TYPE, -1, &mType);
    if (mType != -1) {
        mRingtoneManager->SetType(mType);
    }

    mCursor = NULL;
    mRingtoneManager->GetCursor((ICursor**)&mCursor);

    // The volume keys will control the stream that we are choosing a ringtone for
    Int32 vol;
    mRingtoneManager->InferStreamType(&vol);
    SetVolumeControlStream(vol);

    // Get the URI whose list item should have a checkmark
    pl = NULL;
    intent->GetParcelableExtra(IRingtoneManager::EXTRA_RINGTONE_EXISTING_URI, (IParcelable**)&pl);
    mExistingUri = IUri::Probe(pl);

    AutoPtr<IAlertControllerAlertParams> p = mAlertParams;
    p->SetCursor(mCursor);
    p->SetOnClickListener(mRingtoneClickListener);
    p->SetLabelColumn(IMediaStoreMediaColumns::TITLE);
    p->SetIsSingleChoice(TRUE);
    p->SetOnItemSelectedListener(mInnerListener);
    String tmp;
    GetString(Elastos::Droid::R::string::ok, &tmp);
    p->SetPositiveButtonText(CoreUtils::Convert(tmp));
    p->SetPositiveButtonListener(mInnerListener);
    GetString(Elastos::Droid::R::string::cancel, &tmp);
    p->SetNegativeButtonText(CoreUtils::Convert(tmp));
    p->SetPositiveButtonListener(mInnerListener);
    p->SetOnPrepareListViewListener(mInnerListener);
    AutoPtr<ICharSequence> cs;
    intent->GetCharSequenceExtra(IRingtoneManager::EXTRA_RINGTONE_TITLE, (ICharSequence**)&cs);
    p->SetTitle(cs);
    cs = NULL;
    p->GetTitle((ICharSequence**)&cs);
    if (cs == NULL) {
        GetString(Elastos::Droid::R::string::ringtone_picker_title, &tmp);
        p->SetTitle(CoreUtils::Convert(tmp));
    }

    SetupAlert();
    return NOERROR;
}

ECode RingtonePickerActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    AlertActivity::OnSaveInstanceState(outState);
    outState->PutInt32(SAVE_CLICKED_POS, mClickedPos);
    return NOERROR;
}

ECode RingtonePickerActivity::OnPrepareListView(
    /* [in] */ IListView* listView)
{
    if (mHasDefaultItem) {
        mDefaultRingtonePos = AddDefaultRingtoneItem(listView);
        AutoPtr<IRingtoneManagerHelper> rmh;
        CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&rmh);
        Boolean flag = FALSE;
        rmh->IsDefault(mExistingUri, &flag);
        if (mClickedPos == POS_UNKNOWN && flag) {
            mClickedPos = mDefaultRingtonePos;
        }
    }

    if (mHasSilentItem) {
        mSilentPos = AddSilentItem(listView);

        // The 'Silent' item should use a null Uri
        if (mClickedPos == POS_UNKNOWN && mExistingUri == NULL) {
            mClickedPos = mSilentPos;
        }
    }

    if (mClickedPos == POS_UNKNOWN) {
        Int32 vol;
        mRingtoneManager->GetRingtonePosition(mExistingUri, &vol);
        mClickedPos = GetListPosition(vol);
    }

    // Put a checkmark next to an item.
    mAlertParams->SetCheckedItem(mClickedPos);
    return NOERROR;
}

Int32 RingtonePickerActivity::AddStaticItem(
    /* [in] */ IListView* listView,
    /* [in] */ Int32 textResId)
{
    AutoPtr<ILayoutInflater> lf;
    GetLayoutInflater((ILayoutInflater**)&lf);
    AutoPtr<IView> view;
    lf->Inflate(Elastos::Droid::R::layout::select_dialog_singlechoice_material, IViewGroup::Probe(listView), FALSE, (IView**)&view);
    AutoPtr<ITextView> textView = ITextView::Probe(view);
    textView->SetText(textResId);
    listView->AddHeaderView(IView::Probe(textView));
    mStaticItemCount++;
    assert(0 && "TODO");
    // return ((AbsListView*)(IAbsListView::Probe(listView)))->GetHeaderViewsCount() - 1;
}

Int32 RingtonePickerActivity::AddDefaultRingtoneItem(
    /* [in] */ IListView* listView)
{
    if (mType == IRingtoneManager::TYPE_NOTIFICATION) {
        return AddStaticItem(listView, R::string::notification_sound_default);
    } else if (mType == IRingtoneManager::TYPE_ALARM) {
        return AddStaticItem(listView, R::string::alarm_sound_default);
    }

    return AddStaticItem(listView, R::string::ringtone_default);
}

Int32 RingtonePickerActivity::AddSilentItem(
    /* [in] */ IListView* listView)
{
    return AddStaticItem(listView, Elastos::Droid::R::string::ringtone_silent);
}

ECode RingtonePickerActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Boolean positiveResult = which == IDialogInterface::BUTTON_POSITIVE;

    // Stop playing the previous ringtone
    mRingtoneManager->StopPreviousRingtone();

    if (positiveResult) {
        AutoPtr<IIntent> resultIntent;
        CIntent::New((IIntent**)&resultIntent);
        AutoPtr<IUri> uri;

        if (mClickedPos == mDefaultRingtonePos) {
            // Set it to the default Uri that they originally gave us
            uri = mUriForDefaultItem;
        } else if (mClickedPos == mSilentPos) {
            // A null Uri is for the 'Silent' item
            uri = NULL;
        } else {
            uri = NULL;
            mRingtoneManager->GetRingtoneUri(GetRingtoneManagerPosition(mClickedPos), (IUri**)&uri);
        }
        resultIntent->PutExtra(IRingtoneManager::EXTRA_RINGTONE_PICKED_URI, IParcelable::Probe(uri));
        SetResult(IActivity::RESULT_OK, resultIntent);
    } else {
        SetResult(IActivity::RESULT_CANCELED);
    }

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IView> view;
    window->GetDecorView((IView**)&view);
    AutoPtr<MyRunnable> mr = new MyRunnable(this);
    Boolean flag = FALSE;
    view->Post(IRunnable::Probe(mr), &flag);

    Finish();
    return NOERROR;
}

ECode RingtonePickerActivity::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    PlayRingtone(position, DELAY_MS_SELECTION_PLAYED);
    return NOERROR;
}

ECode RingtonePickerActivity::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    return NOERROR;
}

void RingtonePickerActivity::PlayRingtone(
    /* [in] */ Int32 position,
    /* [in] */ Int32 delayMs)
{
    mHandler->RemoveCallbacks(mInnerRunnable);
    mSampleRingtonePos = position;
    Boolean flag = FALSE;
    mHandler->PostDelayed(mInnerRunnable, delayMs, &flag);
}

ECode RingtonePickerActivity::Run()
{
    StopAnyPlayingRingtone();
    if (mSampleRingtonePos == mSilentPos) {
        return NOERROR;
    }

    AutoPtr<IRingtone> ringtone;
    AutoPtr<IRingtoneManagerHelper> rmh;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&rmh);
    if (mSampleRingtonePos == mDefaultRingtonePos) {
        if (mDefaultRingtone == NULL) {
            mDefaultRingtone = NULL;
            rmh->GetRingtone(this, mUriForDefaultItem, (IRingtone**)&mDefaultRingtone);
        }
       /*
        * Stream type of mDefaultRingtone is not set explicitly here.
        * It should be set in accordance with mRingtoneManager of this Activity.
        */
        if (mDefaultRingtone != NULL) {
            Int32 st;
            mRingtoneManager->InferStreamType(&st);
            mDefaultRingtone->SetStreamType(st);
        }
        ringtone = mDefaultRingtone;
        mCurrentRingtone = NULL;
    } else {
        ringtone = NULL;
        mRingtoneManager->GetRingtone(GetRingtoneManagerPosition(mSampleRingtonePos), (IRingtone**)&ringtone);
        mCurrentRingtone = ringtone;
    }

    if (ringtone != NULL) {
        ringtone->Play();
    }
    return NOERROR;
}

ECode RingtonePickerActivity::OnStop()
{
    AlertActivity::OnStop();
    Boolean flag = FALSE;
    if (!(IsChangingConfigurations(&flag), flag)) {
        StopAnyPlayingRingtone();
    } else {
        SaveAnyPlayingRingtone();
    }
    return NOERROR;
}

ECode RingtonePickerActivity::OnPause()
{
    AlertActivity::OnPause();
    Boolean flag = FALSE;
    if (!(IsChangingConfigurations(&flag), flag)) {
        StopAnyPlayingRingtone();
    }
    return NOERROR;
}

void RingtonePickerActivity::SaveAnyPlayingRingtone()
{
    Boolean flag = FALSE;
    if (mDefaultRingtone != NULL && (mDefaultRingtone->IsPlaying(&flag), flag)) {
        sPlayingRingtone = mDefaultRingtone;
    } else if (mCurrentRingtone != NULL && (mCurrentRingtone->IsPlaying(&flag), flag)) {
        sPlayingRingtone = mCurrentRingtone;
    }
}

void RingtonePickerActivity::StopAnyPlayingRingtone()
{
    Boolean flag = FALSE;
    if (sPlayingRingtone != NULL && (sPlayingRingtone->IsPlaying(&flag), flag)) {
        sPlayingRingtone->Stop();
    }
    sPlayingRingtone = NULL;

    if (mDefaultRingtone != NULL && (mDefaultRingtone->IsPlaying(&flag), flag)) {
        mDefaultRingtone->Stop();
    }

    if (mRingtoneManager != NULL) {
        mRingtoneManager->StopPreviousRingtone();
    }
}

Int32 RingtonePickerActivity::GetRingtoneManagerPosition(
    /* [in] */ Int32 listPos)
{
    return listPos - mStaticItemCount;
}

Int32 RingtonePickerActivity::GetListPosition(
    /* [in] */ Int32 ringtoneManagerPos)
{
    // If the manager position is -1 (for not found), return that
    if (ringtoneManagerPos < 0) return ringtoneManagerPos;

    return ringtoneManagerPos + mStaticItemCount;
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos