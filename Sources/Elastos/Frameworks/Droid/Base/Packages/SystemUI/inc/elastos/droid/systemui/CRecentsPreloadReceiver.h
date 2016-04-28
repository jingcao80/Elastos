#ifndef __CRECENTSPRELOADRECEIVER_H_
#define __CRECENTSPRELOADRECEIVER_H_

#include "_CRecentsPreloadReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

class CRecentsPreloadReceiver : public BroadcastReceiver
{
public:
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    TO_STRING_IMPL("CRecentsPreloadReceiver: ")
};

}// namespace SystemUI
}// namespace DevSamples
}// namespace Droid
}// namespace Elastos

#endif //__CRECENTSPRELOADRECEIVER_H_
