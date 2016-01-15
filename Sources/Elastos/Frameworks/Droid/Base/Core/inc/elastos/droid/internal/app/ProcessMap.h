
#ifndef __ELASTOS_DROID_INTERNAL_APP_PROCESSMAP_H__
#define __ELASTOS_DROID_INTERNAL_APP_PROCESSMAP_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IArrayMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ProcessMap
    : public Object
    , public IProcessMap
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI Get(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [out] */ IInterface** value);

    CARAPI Put(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ IInterface* value);

    CARAPI Remove(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI GetMap(
        /* [out] */ IArrayMap** map);

public:
    AutoPtr<IArrayMap> mMap;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_PROCESSMAP_H__
