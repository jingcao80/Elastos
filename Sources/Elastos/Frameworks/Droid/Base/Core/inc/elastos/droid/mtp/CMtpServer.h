
#ifndef __ELASTOS_DROID_MTP_CMTPSERVER_H__
#define __ELASTOS_DROID_MTP_CMTPSERVER_H__

#include "_Elastos_Droid_Mtp_CMtpServer.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpServer)
    , public Object
    , public IRunnable
    , public IMtpServer
{
public:
    CMtpServer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IMtpDatabase* database,
        /* [in] */ Boolean usePtp);

    CARAPI Start();

    CARAPI Run();

    CARAPI SendObjectAdded(
        /* [in] */ Int32 handle);

    CARAPI SendObjectRemoved(
        /* [in] */ Int32 handle);

    CARAPI SendDevicePropertyChanged(
        /* [in] */ Int32 property);

    CARAPI AddStorage(
        /* [in] */ IMtpStorage* storage);

    CARAPI RemoveStorage(
        /* [in] */ IMtpStorage* storage);

private:
    CARAPI_(void) NativeSetup(
        /* [in] */ IMtpDatabase* database,
        /* [in] */ Boolean usePtp);

    CARAPI_(void) NativeRun();

    CARAPI_(void) NativeCleanup();

    CARAPI_(void) NativeSendObjectAdded(
        /* [in] */ Int32 handle);

    CARAPI_(void) NativeSendObjectRemoved(
        /* [in] */ Int32 handle);

    CARAPI_(void) NativeSendDevicePropertyChanged(
        /* [in] */ Int32 property);

    CARAPI_(void) NativeAddStorage(
        /* [in] */ IMtpStorage* storage);

    CARAPI_(void) NativeRemoveStorage(
        /* [in] */ Int32 storageId);

private:
    Int64 mNativeContext;

    Object mutex;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MTP_CMTPSERVER_H__
