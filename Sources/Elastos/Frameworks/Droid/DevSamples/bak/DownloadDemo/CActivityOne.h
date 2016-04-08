#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "_CActivityOne.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::App::IDownloadManagerRequest;
using Elastos::Droid::Content::BroadcastReceiver;
// using Elastos::Droid::Content::IBroadcastReceiver
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace DownloadDemo {

class CActivityOne : public Activity
{
public:
    class MyListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        MyListener(
            /* [in] */ CActivityOne* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mHost;
    };

    class MyReceiver
        : public BroadcastReceiver
    {
    public:
        MyReceiver(
            /* [in] */ CActivityOne* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CActivityOne* mHost;

    };

    class MyRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();

        MyRunnable(
            /* [in] */ CActivityOne* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CActivityOne* mHost;
    };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

public:
    AutoPtr<IButton> mDBt;
    AutoPtr<IButton> mRBt;
    AutoPtr<IDownloadManager> mDownloadManager;
    Int64 mDownloadId;
    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IRunnable> mRunnable;

};

} // namespace DownloadDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__