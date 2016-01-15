#include "CActionServiceCast.h"
#include <stdio.h>

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AlarmManagerDemo {

Int32 CActionServiceCast::mNum = 0;

CActionServiceCast::CActionServiceCast()
{
    PFL_EX(" ################# CActionServiceCast New ");
}

CActionServiceCast::~CActionServiceCast()
{
    PFL_EX(" ################# CActionServiceCast delete ");
}

ECode CActionServiceCast::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    binder=NULL;
    return NOERROR;
}

ECode CActionServiceCast::OnCreate()
{
    Service::OnCreate();
    PFL_EX(" #################CActionServiceCast OnCreate !");
    return NOERROR;
}

ECode CActionServiceCast::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    Service::OnStart(intent, startId);
    PFL_EX(" CActionServiceCast OnStart !  %d", mNum++);
    return NOERROR;
}

ECode CActionServiceCast::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    PFL_EX(" CActionServiceCast OnStartCommand !  %d", mNum++);
    Service::OnStartCommand(intent, flags, startId, result);
    return NOERROR;
}

ECode CActionServiceCast::OnDestroy()
{
    Service::OnDestroy();
    PFL_EX(" #################CActionServiceCast OnDestroy !");
    return NOERROR;
}

}  //AlarmManagerDemo
}  //DevSamples
}  //Droid
}  //Elastos