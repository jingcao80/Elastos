
#ifndef __CBROADCASTRECEIVERTWO_H__
#define __CBROADCASTRECEIVERTWO_H__

#include "_CBroadcastReceiverTwo.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BroadcastStaticDemo {

class CBroadcastReceiverTwo : public BroadcastReceiver
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
        *info = String("CBroadcastReceiverTwo");
        (*info).AppendFormat("%p", this);
        return NOERROR;
    }
private:
};

} // namespace BroadcastStaticDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
#endif // __CBROADCASTRECEIVERTWO_H__
