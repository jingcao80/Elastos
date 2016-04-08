
#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "_CMainActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Media::IMediaRecorder;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace RecorderDemo {

class CMainActivity
    : public Activity
{
private:
    class RecorderClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        RecorderClickListener(
            /* [in] */  CMainActivity* host,
            /* [in] */  IMediaRecorder* mr,
            /* [in] */  IButton* btn)
             : mHost(host)
             , mRecordButton(btn)
             , mMediaRecorder(mr)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMainActivity* mHost;
        IButton* mRecordButton;
        IMediaRecorder* mMediaRecorder;
    };

    class StopClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        StopClickListener(
            /* [in] */  CMainActivity* host,
            /* [in] */  IMediaRecorder* mr,
            /* [in] */  IButton* btn)
             : mHost(host)
             , mRecordButton(btn)
             , mMediaRecorder(mr)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMainActivity* mHost;
        IButton* mRecordButton;
        IMediaRecorder* mMediaRecorder;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnRestart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private:
    AutoPtr<IButton> mRecordButton;
    AutoPtr<IButton> mStopButton;
    AutoPtr<IMediaRecorder> mMediaRecorder;
};

} // namespace RecorderDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CMAINACTIVITY_H__

