#ifndef __ELASTOS_DROID_APP_CLEGACYPACKAGEDELETEOBSERVER_H__
#define __ELASTOS_DROID_APP_CLEGACYPACKAGEDELETEOBSERVER_H__

#include "_Elastos_Droid_App_CLegacyPackageDeleteObserver.h"
#include "elastos/droid/content/pm/PackageManager.h"

using Elastos::Droid::Content::Pm::PackageManager;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CLegacyPackageDeleteObserver) , public PackageManager::LegacyPackageDeleteObserver
{
public:
    CAR_OBJECT_DECL()
};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_APP_CLEGACYPACKAGEDELETEOBSERVER_H__
