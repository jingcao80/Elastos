
#ifndef __MyTimerTask_H__
#define __MyTimerTask_H__

#include <Elastos.CoreLibrary.h>
#include <Elastos.CoreLibrary.h>
#include <elastos/TimerTask.h>
#include "CTimeService.h"

using Elastos::Utility::ITimer;
using Elastos::Utility::ITimerTask;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppWidgetDemo {

class MyTimerTask
     : public ElRefBase
     , public Elastos::Utility::TimerTask
     , public ITimerTask
{
public:
    MyTimerTask(
        /* [in]  */ CTimeService* service);

    CARAPI_(PInterface) Probe(
       /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
       /* [in] */ IInterface *pObject,
       /* [out] */ InterfaceID *pIID);

    CARAPI Run();

    CARAPI Cancel(
            /* [out] */ Boolean * pValue);

    CARAPI ScheduledExecutionTime(
            /* [out] */ Int64 * pValue);

    CARAPI GetWhen(
            /* [out] */ Int64* when);

    CARAPI SetWhen(
            /* [in] */ Int64 when);

    CARAPI IsScheduled(
            /* [out] */ Boolean* scheduled);

    CARAPI IsCancelled(
            /* [out] */ Boolean* cancelled);

    CARAPI GetPeriod(
            /* [out] */ Int64* period);

    CARAPI SetPeriod(
            /* [in] */ Int64 period);

    CARAPI IsFixedRate(
            /* [out] */ Boolean* fixed);

    CARAPI SetFixedRate(
            /* [in] */ Boolean fixed);

    CARAPI Lock();

    CARAPI Unlock();

    CARAPI SetScheduledTime(
            /* [in] */ Int64 time);

private:
    CARAPI_(Object*) GetSelfLock();

private:
    static Int32 sCount;
    Object mLock;
    CTimeService* mService;
};

} // namespace AppWidgetDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__MyTimerTask_H__
