#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGER_WAIT_RESULT_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGER_WAIT_RESULT_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CActivityManagerWaitResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityManagerWaitResult)
    , public Object
    , public IActivityManagerWaitResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()


    CActivityManagerWaitResult();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetResult(
        /* [out] */ Int32* result);

    CARAPI SetResult(
        /* [in] */ Int32 result);

    CARAPI GetTimeout(
        /* [out] */ Boolean* timeout);

    CARAPI SetTimeout(
        /* [in] */ Boolean timeout);

    CARAPI GetWho(
        /* [out] */ IComponentName** who);

    CARAPI SetWho(
        /* [in] */ IComponentName* who);

    CARAPI GetThisTime(
        /* [out] */ Int64* thisTime);

    CARAPI SetThisTime(
        /* [in] */ Int64 thisTime);

    CARAPI GetTotalTime(
        /* [out] */ Int64* totalTime);

    CARAPI SetTotalTime(
        /* [in] */ Int64 totalTime);

private:
    Int32 mResult;
    Boolean mTimeout;
    AutoPtr<IComponentName> mWho;
    Int64 mThisTime;
    Int64 mTotalTime;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGER_WAIT_RESULT_H__
