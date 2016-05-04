#ifndef  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERAPPLICATIONCONTENTOBSERVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERAPPLICATIONCONTENTOBSERVER_H__

#include "_Elastos_Droid_Launcher2_CLauncherApplicationContentObserver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/LauncherApplication.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncherApplicationContentObserver)
    , public LauncherApplication::MyContentObserver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHERAPPLICATIONCONTENTOBSERVER_H__