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

#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGACTIVITY_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGACTIVITY_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/dialer/calllog/CallLogFragment.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsActivity.h"
#include "elastos/droid/support/v13/app/FragmentPagerAdapter.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Contacts::Common::List::IViewPagerTabs;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Support::V4::View::IViewPager;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Runnable;
using Elastos::Droid::DialerBind::Analytics::AnalyticsActivity;
using Elastos::Droid::Dialer::Voicemail::IVoicemailStatusHelper;
using Elastos::Droid::Support::V13::App::FragmentPagerAdapter;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

class CallLogActivity
    : public AnalyticsActivity
    , public ICallLogActivity
    , public ICallLogQueryHandlerListener
{
public:
    class ViewPagerAdapter
        : public FragmentPagerAdapter
    {
    public:
        ViewPagerAdapter(
            /* [in] */ IFragmentManager* fm,
            /* [in] */ CallLogActivity* host);

        // @Override
        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IFragment** item);

        // @Override
        CARAPI GetPageTitle(
            /* [in] */ Int32 position,
            /* [out] */ ICharSequence** title);

        // @Override
        CARAPI GetCount(
            /* [out] */ Int32* count);

    private:
        CallLogActivity* mHost;
    };

private:
    class WaitForVoicemailTimeoutRunnable
        : public Runnable
    {
    public:
        WaitForVoicemailTimeoutRunnable(
            /* [in] */ CallLogActivity* host);

        CARAPI Run();

    private:
        CallLogActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CallLogActivity();

    CARAPI constructor();

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* isConsumed);

    // @Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* allowToShow);

    // @Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */  Boolean* res);

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI OnVoicemailStatusFetched(
        /* [in] */ ICursor* statusCursor);

    // @Override
    CARAPI OnCallsFetched(
        /* [in] */ ICursor* statusCursor,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnResume();

private:
    static const Int32 WAIT_FOR_VOICEMAIL_PROVIDER_TIMEOUT_MS = 300;
    static const Int32 TAB_INDEX_ALL = 0;
    static const Int32 TAB_INDEX_MISSED = 1;
    static const Int32 TAB_INDEX_VOICEMAIL = 2;

    static const Int32 TAB_INDEX_COUNT_DEFAULT = 2;
    static const Int32 TAB_INDEX_COUNT_WITH_VOICEMAIL = 3;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IViewPager> mViewPager;
    AutoPtr<IViewPagerTabs> mViewPagerTabs;
    AutoPtr<ViewPagerAdapter> mViewPagerAdapter;
    AutoPtr<CallLogFragment> mAllCallsFragment;
    AutoPtr<CallLogFragment> mMissedCallsFragment;
    AutoPtr<CallLogFragment> mVoicemailFragment;
    AutoPtr<IVoicemailStatusHelper> mVoicemailStatusHelper;

    Boolean mSwitchToVoicemailTab;

    AutoPtr<ArrayOf<String> > mTabTitles;

    Boolean mHasActiveVoicemailProvider;

    AutoPtr<IRunnable> mWaitForVoicemailTimeoutRunnable;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGACTIVITY_H__
