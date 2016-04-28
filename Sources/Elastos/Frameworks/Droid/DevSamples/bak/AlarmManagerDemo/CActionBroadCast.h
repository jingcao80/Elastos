
#ifndef __ACTIONBROADCAST_H__
#define __ACTIONBROADCAST_H__

#include "Elastos.Droid.Core.h"
#include "Elastos.Core.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "_CActionBroadCast.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AlarmManagerDemo {

class CActionBroadCast
    : public BroadcastReceiver
{
public:
    CActionBroadCast();

    ~CActionBroadCast();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    TO_STRING_IMPL("CActionBroadCast")
private:
    static Int32 mNum;
};

}  //AlarmManagerDemo
}  //DevSamples
}  //Droid
}  //Elastos

#endif //__ACTIONBROADCAST_H__



