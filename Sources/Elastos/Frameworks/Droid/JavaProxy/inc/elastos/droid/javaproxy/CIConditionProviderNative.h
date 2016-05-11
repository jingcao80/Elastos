
#ifndef __ELASTOS_DROID_JAVAPROXY_CICONDITIONPROVIDERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CICONDITIONPROVIDERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIConditionProviderNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Notification::IIConditionProvider;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIConditionProviderNative)
    , public Object
    , public IIConditionProvider
    , public IBinder
{
public:
    ~CIConditionProviderNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnConnected();

    CARAPI OnRequestConditions(
        /* [in] */ Int32 relevance);

    CARAPI OnSubscribe(
        /* [in] */ IUri* conditionId);

    CARAPI OnUnsubscribe(
        /* [in] */ IUri* conditionId);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CICONDITIONPROVIDERNATIVE_H__
