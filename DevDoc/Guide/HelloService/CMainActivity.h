#ifndef __CMAINACTIVITY_H__
#define __CMAINACTIVITY_H__

#include "app/Activity.h"
#include "_CMainActivity.h"
#include "CBindHelloService.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace HelloService {

class CMainActivity : public Activity
{
public:
    class ClickListener
        : public IViewOnClickListener
        , public ElRefBase
    {
    public:
        ClickListener(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMainActivity* mHost;
    };

    class ServiceConnection
        : public IServiceConnection
        , public ElRefBase
    {
    public:
        CServiceConnection(
            /* [in] */ CMainActivity* host);

        CAR_INTERFACE_DECL();

        CARAPI OnServiceConnected (
            /*[in]*/ IComponentName* name,
            /*[in]*/ IBinder* service);

        CARAPI OnServiceDisconnected (
            /*[in]*/ IComponentName* name);

    private:
        CMainActivity* mHost;
    };

public :
    CMainActivity();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStart();

    CARAPI OnRestart();

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnStop();

    CARAPI OnDestroy();

private :
    AutoPtr<IServiceConnection> mConn;
    AutoPtr<BindHelloService> mBindService;
    AutoPtr<ClickListener> mClickListener;
    static const String TAG;
};

} // namespace HelloService
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CMAINACTIVITY_H__
