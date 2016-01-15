
#ifndef __CDATASETOBSERVERNATIVE_H__
#define __CDATASETOBSERVERNATIVE_H__

#include "_CDataSetObserverNative.h"
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

#endif // __CDATASETOBSERVERNATIVE_H__
