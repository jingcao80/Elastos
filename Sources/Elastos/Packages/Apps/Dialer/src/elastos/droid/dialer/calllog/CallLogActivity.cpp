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

#include "elastos/droid/dialer/calllog/CallLogActivity.h"
#include "elastos/droid/dialer/calllog/ClearCallLogDialog.h"
#include "elastos/droid/dialer/calllog/CallLogQueryHandler.h"
#include "elastos/droid/contacts/common/interactions/TouchPointManager.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"
#include <elastos/droid/R.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::Contacts::Common::Interactions::TouchPointManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Support::V4::View::IViewPagerOnPageChangeListener;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Dialer::Voicemail::VoicemailStatusHelperImpl;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

static const String TAG("CCallLogActivity");

//=================================================================
// CallLogActivity::ViewPagerAdapter
//=================================================================
CallLogActivity::ViewPagerAdapter::ViewPagerAdapter(
    /* [in] */ IFragmentManager* fm,
    /* [in] */ CallLogActivity* host)
    : FragmentPagerAdapter(fm)
    , mHost(host)
{}

ECode CallLogActivity::ViewPagerAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IFragment** item)
{
    VALIDATE_NOT_NULL(item)

    switch (position) {
        case TAB_INDEX_ALL:
            mHost->mAllCallsFragment = new CallLogFragment();
            mHost->mAllCallsFragment->constructor(CallLogQueryHandler::CALL_TYPE_ALL);
            *item = IFragment::Probe(mHost->mAllCallsFragment);
            REFCOUNT_ADD(*item);
            return NOERROR;
        case TAB_INDEX_MISSED:
            mHost->mMissedCallsFragment = new CallLogFragment();
            mHost->mMissedCallsFragment->constructor(ICalls::MISSED_TYPE);
            *item = IFragment::Probe(mHost->mMissedCallsFragment);
            REFCOUNT_ADD(*item);
            return NOERROR;
        case TAB_INDEX_VOICEMAIL:
            mHost->mVoicemailFragment = new CallLogFragment();
            mHost->mVoicemailFragment->constructor(ICalls::VOICEMAIL_TYPE);
            *item = IFragment::Probe(mHost->mVoicemailFragment);
            REFCOUNT_ADD(*item);
            return NOERROR;
    }

    Logger::E("CallLogActivity", "No fragment at position %d", position);
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CallLogActivity::ViewPagerAdapter::GetPageTitle(
    /* [in] */ Int32 position,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    AutoPtr<ICharSequence> cs = CoreUtils::Convert((*mHost->mTabTitles)[position]);
    *title = cs;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode CallLogActivity::ViewPagerAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mHost->mHasActiveVoicemailProvider ?
            TAB_INDEX_COUNT_WITH_VOICEMAIL : TAB_INDEX_COUNT_DEFAULT;
    return NOERROR;
}


//=================================================================
// CallLogActivity::WaitForVoicemailTimeoutRunnable
//=================================================================
CallLogActivity::WaitForVoicemailTimeoutRunnable::WaitForVoicemailTimeoutRunnable(
    /* [in] */ CallLogActivity* host)
    : mHost(host)
{}

ECode CallLogActivity::WaitForVoicemailTimeoutRunnable::Run()
{
    mHost->mViewPagerTabs->SetViewPager(mHost->mViewPager);
    mHost->mViewPager->SetCurrentItem(TAB_INDEX_ALL);
    mHost->mSwitchToVoicemailTab = FALSE;
    return NOERROR;
}


//=================================================================
// CallLogActivity
//=================================================================
const Int32 CallLogActivity::WAIT_FOR_VOICEMAIL_PROVIDER_TIMEOUT_MS;
const Int32 CallLogActivity::TAB_INDEX_ALL;
const Int32 CallLogActivity::TAB_INDEX_MISSED;
const Int32 CallLogActivity::TAB_INDEX_VOICEMAIL;
const Int32 CallLogActivity::TAB_INDEX_COUNT_DEFAULT;
const Int32 CallLogActivity::TAB_INDEX_COUNT_WITH_VOICEMAIL;

CAR_INTERFACE_IMPL_2(CallLogActivity, AnalyticsActivity, ICallLogActivity, ICallLogQueryHandlerListener);

CallLogActivity::CallLogActivity()
    : mSwitchToVoicemailTab(FALSE)
    , mHasActiveVoicemailProvider(FALSE)
{}

ECode CallLogActivity::constructor()
{
    return AnalyticsActivity::constructor();
}

ECode CallLogActivity::DispatchTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* isConsumed)
{
    Int32 action;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        Float rawX, rawY;
        ev->GetRawX(&rawX);
        ev->GetRawY(&rawY);
        TouchPointManager::GetInstance()->SetPoint((Int32)rawX, (Int32)rawY);
    }
    return AnalyticsActivity::DispatchTouchEvent(ev, isConsumed);
}

ECode CallLogActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate");

    AnalyticsActivity::OnCreate(savedInstanceState);

    CHandler::New((IHandler**)&mHandler);

    SetContentView(Elastos::Droid::Dialer::R::layout::call_log_activity);
    GetWindow()->SetBackgroundDrawable(NULL);

    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    actionBar->SetDisplayShowHomeEnabled(TRUE);
    actionBar->SetDisplayHomeAsUpEnabled(TRUE);
    actionBar->SetDisplayShowTitleEnabled(TRUE);
    actionBar->SetElevation(0);

    Int32 startingTab = TAB_INDEX_ALL;
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        Int32 callType;
        intent->GetInt32Extra(ICalls::EXTRA_CALL_TYPE_FILTER, -1, &callType);
        if (callType == ICalls::MISSED_TYPE) {
            startingTab = TAB_INDEX_MISSED;
        }
        else if (callType == ICalls::VOICEMAIL_TYPE) {
            startingTab = TAB_INDEX_VOICEMAIL;
        }
    }

    mTabTitles = ArrayOf<String>::Alloc(TAB_INDEX_COUNT_WITH_VOICEMAIL);
    String alltitle;
    GetString(Elastos::Droid::Dialer::R::string::call_log_all_title, &alltitle);
    mTabTitles->Set(0, alltitle);
    String missedtitle;
    GetString(Elastos::Droid::Dialer::R::string::call_log_missed_title, &missedtitle);
    mTabTitles->Set(1, missedtitle);
    String voicemailtitle;
    GetString(Elastos::Droid::Dialer::R::string::call_log_voicemail_title, &voicemailtitle);
    mTabTitles->Set(2, voicemailtitle);

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Dialer::R::id::call_log_pager, (IView**)&view);
    mViewPager = IViewPager::Probe(view);

    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    mViewPagerAdapter = new ViewPagerAdapter(manager, this);
    mViewPager->SetAdapter(mViewPagerAdapter);
    mViewPager->SetOffscreenPageLimit(2);

    view = NULL;
    FindViewById(Elastos::Droid::Dialer::R::id::viewpager_header, (IView**)&view);
    mViewPagerTabs = IViewPagerTabs::Probe(view);
    mViewPager->SetOnPageChangeListener(IViewPagerOnPageChangeListener::Probe(mViewPagerTabs));

    mWaitForVoicemailTimeoutRunnable = new WaitForVoicemailTimeoutRunnable(this);
    if (startingTab == TAB_INDEX_VOICEMAIL) {
        // The addition of the voicemail tab is an asynchronous process, so wait till the tab
        // is added, before attempting to switch to it. If the querying of CP2 for voicemail
        // providers takes too long, give up and show the first tab instead.
        mSwitchToVoicemailTab = TRUE;
        Boolean result;
        mHandler->PostDelayed(mWaitForVoicemailTimeoutRunnable,
                WAIT_FOR_VOICEMAIL_PROVIDER_TIMEOUT_MS, &result);
    }
    else {
        mViewPagerTabs->SetViewPager(mViewPager);
        mViewPager->SetCurrentItem(startingTab);
    }

    mVoicemailStatusHelper = new VoicemailStatusHelperImpl();
    return NOERROR;
}

ECode CallLogActivity::OnResume()
{
    AnalyticsActivity::OnResume();
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<CallLogQueryHandler> callLogQueryHandler = new CallLogQueryHandler();
    callLogQueryHandler->constructor(resolver, (ICallLogQueryHandlerListener*)this);
    callLogQueryHandler->FetchVoicemailStatus();
    return NOERROR;
}

ECode CallLogActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* allowToShow)
{
    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::call_log_options, menu);
    *allowToShow = TRUE;

    return NOERROR;
}

ECode CallLogActivity::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IMenuItem> itemDeleteAll;
    menu->FindItem(Elastos::Droid::Dialer::R::id::delete_all, (IMenuItem**)&itemDeleteAll);

    // If onPrepareOptionsMenu is called before fragments loaded. Don't do anything.
    if (mAllCallsFragment != NULL && itemDeleteAll != NULL) {
        AutoPtr<CallLogAdapter> adapter = mAllCallsFragment->GetAdapter();
        Boolean isEmpty;
        itemDeleteAll->SetVisible(adapter != NULL && (adapter->IsEmpty(&isEmpty), !isEmpty));
    }
    *res = TRUE;
    return NOERROR;
}

ECode CallLogActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 id;
    item->GetItemId(&id);
    AutoPtr<IIntent> intent;
    switch (id) {
        case Elastos::Droid::R::id::home:
            CIntent::New(this, ECLSID_CDialtactsActivity, (IIntent**)&intent);
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
            StartActivity(intent);
            *res = TRUE;
            return NOERROR;
        case R::id::delete_all:
            AutoPtr<IFragmentManager> manager;
            GetFragmentManager((IFragmentManager**)&manager);
            ClearCallLogDialog::Show(manager);
            *res = TRUE;
            return NOERROR;
    }
    return AnalyticsActivity::OnOptionsItemSelected(item, res);
}

ECode CallLogActivity::OnVoicemailStatusFetched(
    /* [in] */ ICursor* statusCursor)
{
    Boolean isFinishing = FALSE;
    if (IsFinishing(&isFinishing), isFinishing) {
        return NOERROR;
    }

    mHandler->RemoveCallbacks(mWaitForVoicemailTimeoutRunnable);
    // Update mHasActiveVoicemailProvider, which controls the number of tabs displayed.
    Int32 activeSources;
    mVoicemailStatusHelper->GetNumberActivityVoicemailSources(statusCursor, &activeSources);
    if ((activeSources > 0) != mHasActiveVoicemailProvider) {
        mHasActiveVoicemailProvider = activeSources > 0;
        mViewPagerAdapter->NotifyDataSetChanged();
        mViewPagerTabs->SetViewPager(mViewPager);
        if (mSwitchToVoicemailTab) {
            mViewPager->SetCurrentItem(TAB_INDEX_VOICEMAIL, FALSE);
        }
    }
    else if (mSwitchToVoicemailTab) {
        // The voicemail tab was requested, but it does not exist because there are no
        // voicemail sources. Just fallback to the first item instead.
        mViewPagerTabs->SetViewPager(mViewPager);
    }

    return NOERROR;
}

ECode CallLogActivity::OnCallsFetched(
    /* [in] */ ICursor* statusCursor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Return false; did not take ownership of cursor
    *result = FALSE;
    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
