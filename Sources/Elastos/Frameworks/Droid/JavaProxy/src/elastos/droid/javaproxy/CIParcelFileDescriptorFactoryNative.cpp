
#include "elastos/droid/javaproxy/CIParcelFileDescriptorFactoryNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Os::EIID_IIParcelFileDescriptorFactory;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIParcelFileDescriptorFactoryNative::TAG("CIParcelFileDescriptorFactoryNative");

CAR_INTERFACE_IMPL_2(CIParcelFileDescriptorFactoryNative, Object, IIParcelFileDescriptorFactory, IBinder)

CAR_OBJECT_IMPL(CIParcelFileDescriptorFactoryNative)

CIParcelFileDescriptorFactoryNative::~CIParcelFileDescriptorFactoryNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIParcelFileDescriptorFactoryNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIParcelFileDescriptorFactoryNative::Open(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    assert(0);
    return NOERROR;
}

ECode CIParcelFileDescriptorFactoryNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIParcelFileDescriptorFactoryNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIParcelFileDescriptorFactoryNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos
