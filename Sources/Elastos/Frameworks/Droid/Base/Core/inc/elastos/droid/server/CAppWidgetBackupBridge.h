#ifndef __ELASTOS_DROID_SERVER_CAPPWIDGETBACKUPBRIDGE_H__
#define __ELASTOS_DROID_SERVER_CAPPWIDGETBACKUPBRIDGE_H__

#include "_Elastos_Droid_Server_CAppWidgetBackupBridge.h"
#include "Elastos.Droid.Core.Server.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Server::IWidgetBackupProvider;
using Elastos::Core::Singleton;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CAppWidgetBackupBridge)
    , public Singleton
    , public IAppWidgetBackupBridge
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI Register(
        /* [in] */ IWidgetBackupProvider* instance);

    CARAPI GetWidgetParticipants(
        /* [in] */ Int32 userId,
        /* [out] */ IList** wp);

    CARAPI GetWidgetState(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<Byte>** ws);

    CARAPI RestoreStarting(
        /* [in] */ Int32 userId);

    CARAPI RestoreWidgetState(
        /* [in] */ const String& packageName,
        /* [in] */ ArrayOf<Byte>* restoredState,
        /* [in] */ Int32 userId);

    CARAPI RestoreFinished(
        /* [in] */ Int32 userId);

private:
    static AutoPtr<IWidgetBackupProvider> sAppWidgetService;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CAPPWIDGETBACKUPBRIDGE_H__
