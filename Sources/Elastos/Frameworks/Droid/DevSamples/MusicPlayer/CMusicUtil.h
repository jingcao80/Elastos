#ifndef __CMUSICUTIL__H__
#define __CMUSICUTIL__H__

#include <Elastos.Droid.Core.h>
#include <ext/frameworkdef.h>
#include <elastos/utility/etl/HashMap.h>

using namespace Elastos;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IServiceConnection;

namespace MusicPlayer {

class CMusicUtil
{
#if 0
    class ServiceConnectionWrapper : public IServiceConnection
    {
    public:
        ServiceConnectionWrapper(IServiceConnection* conn) : mConn(conn)
        {

        }

        CARAPI OnServiceConnected(IComponentName * pName, IBinder * pService)
        {
            sPlayController = IPlayController::Probe(pService);
            if(NULL != mConn)
            {
                mConn->OnServiceConnected(pName, pService);
            }
        }

        CARAPI OnServiceDisconnected(IComponentName * pName)
        {
            if(NULL != mConn)
            {
                mConn->OnServiceDisconnected(pName);
            }

            sPlayController->Release();
        }

    private:
        AutoPtr<IServiceConnection> mConn;
    };
#endif

public:
//    static AutoPtr<IPlayController> sPlayController;
    static CARAPI BindToService(
        /* [in] */ IContext* context,
        /* [in] */ IServiceConnection* conn);

    static CARAPI UnbindFromService(
        /* [in] */ IContext* context);

private:
    CMusicUtil();

private:
    static HashMap<Int32, AutoPtr<IServiceConnection> > sConnectionMap;
};

} // namespace MusicPlayer

#endif
