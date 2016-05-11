
#ifndef __ELASTOS_DROID_JAVAPROXY_CIPARCELFILEDESCRIPTORFACTORYNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIPARCELFILEDESCRIPTORFACTORYNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIParcelFileDescriptorFactoryNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Internal::Os::IIParcelFileDescriptorFactory;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIParcelFileDescriptorFactoryNative)
    , public Object
    , public IIParcelFileDescriptorFactory
    , public IBinder
{
public:
    ~CIParcelFileDescriptorFactoryNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI Open(
        /* [in] */ const String& name,
        /* [in] */ Int32 mode,
        /* [out] */ IParcelFileDescriptor** pfd);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIPARCELFILEDESCRIPTORFACTORYNATIVE_H__
