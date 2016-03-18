
#ifndef __ELASTOS_DROID_JAVAPROXY_CTEXTSERVICESSESSIONLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CTEXTSERVICESSESSIONLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CTextServicesSessionListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::TextService::IISpellCheckerSession;
using Elastos::Droid::Internal::TextService::IITextServicesSessionListener;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CTextServicesSessionListenerNative)
    , public Object
    , public IITextServicesSessionListener
    , public IBinder
{
public:
    ~CTextServicesSessionListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnServiceConnected(
        /* [in] */ IISpellCheckerSession* spellCheckerSession);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CTEXTSERVICESSESSIONLISTENERNATIVE_H__
