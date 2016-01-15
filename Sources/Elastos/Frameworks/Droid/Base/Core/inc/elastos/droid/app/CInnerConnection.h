#ifndef __ELASTOS_DROID_APP_CINNERCONNECTION_H__
#define __ELASTOS_DROID_APP_CINNERCONNECTION_H__

#include "_Elastos_Droid_App_CInnerConnection.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CInnerConnection)
    , public Object
    , public IIServiceConnection
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IServiceDispatcher* sd);

    CARAPI Connected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    IServiceDispatcher* mDispatcher;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CINNERCONNECTION_H__
