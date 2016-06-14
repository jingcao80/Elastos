
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGACTIVITY_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGACTIVITY_H__

#include <AnalyticsActivity.h>

using Elastos::Core::IRunnable;

namespace Elastos{
namespace Apps{
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
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();

        WaitForVoicemailTimeoutRunnable(
            /* [in] */ CallLogActivity* host);

        CARAPI Run();

    private:
        CallLogActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CallLogActivity();

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
        /* [out] */ Boolean* res);

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
        /* [out] */ Boolean result);

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
    AutoPtr<IViewPager> mViewPager;
    AutoPtr<IViewPagerTabs> mViewPagerTabs;
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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CALLLOGACTIVITY_H__
