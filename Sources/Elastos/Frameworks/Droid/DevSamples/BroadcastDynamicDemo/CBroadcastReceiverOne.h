
#ifndef __CBROADCASTRECEIVERONE_H__
#define __CBROADCASTRECEIVERONE_H__

#include "_CBroadcastReceiverOne.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BroadcastDynamicDemo {

class CBroadcastReceiverOne : public BroadcastReceiver
{
public:
    CBroadcastReceiverOne();

protected:
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        *info = String("CBroadcastReceiverOne");
        (*info).AppendFormat("%p", this);
        return NOERROR;
    }
private:
};

} // namespace BroadcastDynamicDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
#endif // __CBROADCASTRECEIVERONE_H__
