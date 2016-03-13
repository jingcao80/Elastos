
#ifndef __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__
#define __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__

#include "_Elastos_Droid_JavaProxy_CIRemoteControlClient.h"
#include <jni.h>

using Elastos::Droid::Media::IIRemoteControlDisplay;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIRemoteControlClient)
{
public:
    ~CIRemoteControlClient();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI OnInformationRequested(
        /* [in] */ Int32 generationId,
        /* [in] */ Int32 infoFlags,
        /* [in] */ Int32 artWidth,
        /* [in] */ Int32 artHeight);

    CARAPI SetCurrentClientGenerationId(
        /* [in] */ Int32 clientGeneration);

    CARAPI PlugRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

    CARAPI UnplugRemoteControlDisplay(
        /* [in] */ IIRemoteControlDisplay* rcd);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIREMOTECONTROLCLIENT_H__
