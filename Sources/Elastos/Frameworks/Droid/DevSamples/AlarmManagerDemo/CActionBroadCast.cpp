#include "CActionBroadCast.h"
#include <elastos/utility/logging/Slogger.h>
#include <stdio.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AlarmManagerDemo {

Int32 CActionBroadCast::mNum = 0;

CActionBroadCast::CActionBroadCast()
{
    //PFL_EX(" CActionBroadCast New ");
}

CActionBroadCast::~CActionBroadCast()
{
    //PFL_EX(" CActionBroadCast delete ");
}

ECode CActionBroadCast::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    PFL_EX(" CActionBroadCast RECEIVE New Message !  %d", mNum++);
    Slogger::I("CActionBroadCast::OnReceive"," CActionBroadCast RECEIVE New Message !  %d", mNum++);
    return NOERROR;
}

}  //AlarmManagerDemo
}  //DevSamples
}  //Droid
}  //Elastos