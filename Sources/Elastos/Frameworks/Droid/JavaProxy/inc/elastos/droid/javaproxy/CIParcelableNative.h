
#ifndef __ELASTOS_DROID_JAVAPROXY_CIPARCELABLENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPARCELABLENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIParcelableNative.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIParcelableNative)
    , public Object
    , public IParcelable
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& pkgPath,
        /* [in] */ ArrayOf<Byte>* obj);

    CARAPI_(String) GetPackagePath();

    CARAPI GetObject(
        /* [out, callee] */ ArrayOf<Byte>** obj);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel * pSource);

    CARAPI WriteToParcel(
        /* [in] */ IParcel * pDest);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    String mPkgPath;

    AutoPtr<ArrayOf<Byte> > mObject;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CIPARCELABLENATIVE_H__
