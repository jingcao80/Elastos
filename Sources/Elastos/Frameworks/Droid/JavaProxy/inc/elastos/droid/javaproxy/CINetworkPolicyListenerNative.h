
#ifndef __ELASTOS_DROID_JAVAPROXY_CINETWORKPOLICYLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINETWORKPOLICYLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CINetworkPolicyListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Net::IINetworkPolicyListener;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CINetworkPolicyListenerNative)
    , public Object
    , public IINetworkPolicyListener
    , public IBinder
{
public:
    ~CINetworkPolicyListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnUidRulesChanged(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 uidRules);

    CARAPI OnMeteredIfacesChanged(
        /* [in] */ ArrayOf<String>* meteredIfaces);

    CARAPI OnRestrictBackgroundChanged(
        /* [in] */ Boolean restrictBackground);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CINETWORKPOLICYLISTENERNATIVE_H__
