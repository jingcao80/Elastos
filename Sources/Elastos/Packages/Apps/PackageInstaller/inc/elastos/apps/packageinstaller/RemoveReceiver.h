
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_REMOVERECEIVER_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_REMOVERECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Content.h>
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Object;

namespace Elastos {
namespace Apps {
namespace PackageInstaller {

class RemoveReceiver
    : public Object
    , public IBroadcastReceiver
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace PackageInstaller
} // namespace Apps
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_REMOVERECEIVER_H__

