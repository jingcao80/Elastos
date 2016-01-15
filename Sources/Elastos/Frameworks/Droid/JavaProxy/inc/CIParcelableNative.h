
#ifndef __CIPARCELABLENATIVE_H__
#define __CIPARCELABLENATIVE_H__

#include "_CIParcelableNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIParcelableNative)
{
public:
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

private:
    static const String TAG;

    String mPkgPath;

    AutoPtr<ArrayOf<Byte> > mObject;
};

}
}
}

#endif // __CIPARCELABLENATIVE_H__
