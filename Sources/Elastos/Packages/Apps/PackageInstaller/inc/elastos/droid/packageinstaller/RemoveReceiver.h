
#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_REMOVERECEIVER_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_REMOVERECEIVER_H__

#include "elastos/droid/content/BroadcastReceiver.h"
#include <Elastos.Droid.Content.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

class RemoveReceiver
    : public BroadcastReceiver
{
public:
    TO_STRING_IMPL("RemoveReceiver")

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_REMOVERECEIVER_H__

