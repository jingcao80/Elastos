
#ifndef __ELASTOS_DROID_JAVAPROXY_CCONTENTOBSERVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CCONTENTOBSERVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CContentObserverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CContentObserverNative)
    , public Object
    , public IContentObserver
    , public IBinder
{
public:
    ~CContentObserverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI GetContentObserver(
        /* [out] */ IIContentObserver** observer);

    CARAPI ReleaseContentObserver(
        /* [out] */ IIContentObserver** oldObserver);

    CARAPI DeliverSelfNotifications(
        /* [out] */ Boolean* result);

    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

    CARAPI OnChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri);

    CARAPI OnChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId);

    CARAPI DispatchChange(
        /* [in] */ Boolean selfChange);

    CARAPI DispatchChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri);

    CARAPI DispatchChange(
        /* [in] */ Boolean selfChange,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CCONTENTOBSERVERNATIVE_H__
