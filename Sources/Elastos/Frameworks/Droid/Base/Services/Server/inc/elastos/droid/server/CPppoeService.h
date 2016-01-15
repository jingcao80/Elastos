
#ifndef __ELASTOS_DROID_SERVER_CPPPOESERVICE_H__
#define __ELASTOS_DROID_SERVER_CPPPOESERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_CPppoeService.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IDhcpInfo;
using Elastos::Droid::Net::Pppoe::IPppoeDevInfo;
using Elastos::Droid::Net::Pppoe::IPppoeStateTracker;


namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CPppoeService)
{
private:
    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CPppoeService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CPppoeService::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CPppoeService* mOwner;
    };

    class MyStopInterfaceThread
        : public ThreadBase
    {
    public:
        MyStopInterfaceThread(
            /* [in] */ CPppoeService* owner);

        virtual CARAPI Run();

    private:
        CPppoeService* mOwner;
    };

    class MyResetInterfaceThread
        : public ThreadBase
    {
    public:
        MyResetInterfaceThread(
            /* [in] */ CPppoeService* owner);

        virtual CARAPI Run();

    private:
        CPppoeService* mOwner;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPppoeStateTracker* tracker);

    CARAPI GetDeviceNameList(
        /* [out, callee] */ ArrayOf<String>** list);

    CARAPI SetPppoeState(
        /* [in] */ Int32 state);/*synchronized*/

    CARAPI GetPppoeState(
        /* [out] */ Int32* val);

    CARAPI UpdatePppoeDevInfo(
        /* [in] */ IPppoeDevInfo* info);/*synchronized*/

    CARAPI IsPppoeConfigured(
        /* [out] */ Boolean* result);

    CARAPI GetSavedPppoeConfig(
        /* [out] */ IPppoeDevInfo** info);/*synchronized*/

    CARAPI GetTotalInterface(
        /* [out] */ Int32* val);

    CARAPI SetPppoeMode(
        /* [in] */ const String& mode);/*synchronized*/

    CARAPI IsPppoeDeviceUp(
        /* [out] */ Boolean* result);

    CARAPI GetDhcpInfo(
        /* [out] */ IDhcpInfo** info);

private:
    CARAPI RegisterForBroadcasts();

    CARAPI_(Int32) ScanPppoeDevice();

    CARAPI_(Int32) GetPersistedState();

    CARAPI PersistPppoeEnabled(Boolean enabled);/*synchronized*/

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IPppoeStateTracker> mTracker;
    AutoPtr<ArrayOf<String> > DevName;
    static String TAG;// = "PppoeService";
    Int32 isPppoeEnabled ;
    Int32 mPppoeState; //= PppoeManager.PPPOE_STATE_UNKNOWN;
    AutoPtr<MyBroadcastReceiver> mReceiver;

};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_CPPPOESERVICE_H__
