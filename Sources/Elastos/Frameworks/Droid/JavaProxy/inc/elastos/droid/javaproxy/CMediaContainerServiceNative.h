
#ifndef __ELASTOS_DROID_JAVAPROXY_CMEDIACONTAINERSERVICENATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CMEDIACONTAINERSERVICENATIVE_H__

#include "_Elastos_Droid_JavaProxy_CMediaContainerServiceNative.h"
#include <jni.h>

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CMediaContainerServiceNative)
{
public:
    ~CMediaContainerServiceNative();

    CARAPI CopyResourceToContainer(
        /* [in] */ Elastos::Droid::Net::IUri * pPackageURI,
        /* [in] */ const String& containerId,
        /* [in] */ const String& key,
        /* [in] */ const String& resFileName,
        /* [in] */ const String& publicResFileName,
        /* [in] */ Boolean isExternal,
        /* [in] */ Boolean isForwardLocked,
        /* [out] */ String * pPath);

    CARAPI CopyResource(
        /* [in] */ Elastos::Droid::Net::IUri * pPackageURI,
        /* [in] */ Elastos::Droid::Content::Pm::IContainerEncryptionParams * pEncryptionParams,
        /* [in] */ Elastos::Droid::Os::IParcelFileDescriptor * pOutStream,
        /* [out] */ Int32 * pRes);

    CARAPI GetMinimalPackageInfo(
        /* [in] */ const String& packagePath,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 threshold,
        /* [out] */ Elastos::Droid::Content::Pm::IPackageInfoLite ** ppPkgLite);

    CARAPI CheckInternalFreeStorage(
        /* [in] */ Elastos::Droid::Net::IUri * pFileUri,
        /* [in] */ Boolean isForwardLocked,
        /* [in] */ Int64 threshold,
        /* [out] */ Boolean * pRes);

    CARAPI CheckExternalFreeStorage(
        /* [in] */ Elastos::Droid::Net::IUri * pFileUri,
        /* [in] */ Boolean isForwardLocked,
        /* [out] */ Boolean * pRes);

    CARAPI GetObbInfo(
        /* [in] */ const String& filename,
        /* [out] */ Elastos::Droid::Content::Res::IObbInfo ** ppObbInfo);

    CARAPI CalculateDirectorySize(
        /* [in] */ const String& directory,
        /* [out] */ Int64 * pSize);

    CARAPI GetFileSystemStats(
        /* [in] */ const String& path,
        /* [out, callee] */ ArrayOf<Int64> ** ppStats);

    CARAPI ClearDirectory(
        /* [in] */ const String& directory);

    CARAPI CalculateInstalledSize(
        /* [in] */ const String& packagePath,
        /* [in] */ Boolean isForwardLocked,
        /* [out] */ Int64 * pSize);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ Handle32 jVM,
        /* [in] */ Handle32 jInstance);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CMEDIACONTAINERSERVICENATIVE_H__
