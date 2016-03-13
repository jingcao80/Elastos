
#ifndef __ELASTOS_DROID_JAVAPROXY_CDATASETOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CDATASETOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CDataSetObserverNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CDataSetObserverNative)
{
public:
    ~CDataSetObserverNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnChanged();

    CARAPI OnInvalidated();

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CDATASETOBSERVERNATIVE_H__
