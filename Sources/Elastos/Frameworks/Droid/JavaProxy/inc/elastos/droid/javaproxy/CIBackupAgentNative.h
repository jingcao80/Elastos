
#ifndef __ELASTOS_DROID_JAVAPROXY_CIBACKUPAGENTNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CIBACKUPAGENTNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CIBackupAgentNative.h"
#include <jni.h>

using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CIBackupAgentNative)
{
public:
    ~CIBackupAgentNative();

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

    CARAPI DoBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestore(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoFullBackup(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestoreFile(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int64 size,
        /* [in] */ Int32 type,
        /* [in] */ const String& domainName,
        /* [in] */ const String& path,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CIBACKUPAGENTNATIVE_H__
