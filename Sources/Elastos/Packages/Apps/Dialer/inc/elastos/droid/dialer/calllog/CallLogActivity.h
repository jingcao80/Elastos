
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGACTIVITY_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGACTIVITY_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsActivity.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Runnable;
using Elastos::Droid::DialerBind::Analytics::AnalyticsActivity;
using Elastos::Droid::Dialer::Voicemail::IVoicemailStatusHelper;

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
    // class ViewPagerAdapter
    //     : public FragmentPagerAdapter
    // {
    // public:
    //     ViewPagerAdapter(
    //         /* [in] */ IFragmentManager* fm,
    //         /* [in] */ CallLogActivity* host);

            // @Override
    //     CARAPI GetItem(
    //         /* [in] */ Int32 position,
    //         /* [out] */ IFragment** item);

            // @Override
    //     CARAPI GetPageTitle(
    //         /* [in] */ Int32 position,
    //         /* [out] */ ICharSequence** title);

            // @Override
    //     CARAPI GetCount(
    //         /* [out] */ Int32* count);

    // private:
    //     CallLogActivity* mHost;
    // };

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
    static const Int32 WAIT_FOR_VOICEMAIL_PROVIDER_TIMEOUT_MS; // = 300;
    static const Int32 TAB_INDEX_ALL; // = 0;
    static const Int32 TAB_INDEX_MISSED; // = 1;
    static const Int32 TAB_INDEX_VOICEMAIL; // = 2;

    static const Int32 TAB_INDEX_COUNT_DEFAULT; // = 2;
    static const Int32 TAB_INDEX_COUNT_WITH_VOICEMAIL; // = 3;

    AutoPtr<IHandler> mHandler;
    // TODO:
    // AutoPtr<IViewPager> mViewPager;
    // AutoPtr<IViewPagerTabs> mViewPagerTabs;
    // AutoPtr<ViewPagerAdapter> mViewPagerAdapter;
    AutoPtr<ICallLogFragment> mAllCallsFragment;
    AutoPtr<ICallLogFragment> mMissedCallsFragment;
    AutoPtr<ICallLogFragment> mVoicemailFragment;
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
