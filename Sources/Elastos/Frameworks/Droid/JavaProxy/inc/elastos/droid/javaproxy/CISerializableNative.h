
#ifndef __ELASTOS_DROID_JAVAPROXY_CISERIALIZABLENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CISERIALIZABLENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CISerializableNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CISerializableNative)
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean isDexClassLoader,
        /* [in] */ const String& pkgPath,
        /* [in] */ const String& optimizedDirectory,
        /* [in] */ ArrayOf<Byte>* obj);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel * pSource);

    CARAPI WriteToParcel(
        /* [in] */ IParcel * pDest);

public:
    Boolean mIsDexClassLoader;
    String mPkgPath;
    String mOptimizedDirectory;
    AutoPtr<ArrayOf<Byte> > mObject;

private:
    static const String TAG;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CISERIALIZABLENATIVE_H__
