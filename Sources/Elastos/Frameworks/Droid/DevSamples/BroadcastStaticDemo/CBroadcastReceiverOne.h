
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
namespace BroadcastStaticDemo {

class CBroadcastReceiverOne : public BroadcastReceiver
{
public:

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

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
#endif // __CBROADCASTRECEIVERONE_H__
