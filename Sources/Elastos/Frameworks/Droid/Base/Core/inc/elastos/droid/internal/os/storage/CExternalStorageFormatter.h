
#ifndef __ELASTOS_DROID_INTERNAL_OS_STORAGE_CEXTERNALSTORAGEFORMATTER_H__
#define __ELASTOS_DROID_INTERNAL_OS_STORAGE_CEXTERNALSTORAGEFORMATTER_H__

#include "_Elastos_Droid_Internal_Os_Storage_CExternalStorageFormatter.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/storage/StorageEventListener.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Storage::IMountService;
using Elastos::Droid::Os::Storage::IStorageEventListener;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::StorageEventListener;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {
namespace Storage {

CarClass(CExternalStorageFormatter)
    , public Elastos::Droid::App::Service
    , public IExternalStorageFormatter
    , public IDialogInterfaceOnCancelListener
{
private:
    class MyStorageEventListener : public StorageEventListener
    {
    public:
        MyStorageEventListener(
            /* [in] */ CExternalStorageFormatter* owner);

        CARAPI OnStorageStateChanged(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState);

    private:
        CExternalStorageFormatter* mOwner;
    };

    class MyThread : public Thread
    {
    public:
        MyThread(
            /* [in] */ CExternalStorageFormatter* owner,
            /* [in] */ const String& extStoragePath);

        CARAPI Run();

    private:
        CExternalStorageFormatter* mOwner;
        String mExtStoragePath;
    };

    class OnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        OnCancelListener(
            /* [in] */ CExternalStorageFormatter* owner);

        CAR_INTERFACE_DECL()

        CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    private:
        CExternalStorageFormatter* mOwner;
    };

public:
    CExternalStorageFormatter();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI OnCreate();

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* cmd);

    CARAPI OnDestroy();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI UpdateProgressDialog(
        /* [in] */ Int32 msg);

    CARAPI_(AutoPtr<IMountService>) GetMountService();

protected:
    CARAPI_(void) Fail(
        /* [in] */ Int32 msg);

    CARAPI_(void) UpdateProgressState();

public:
    static const String TAG;
    static const AutoPtr<IComponentName> COMPONENT_NAME;

    AutoPtr<IStorageEventListener> mStorageListener;

private:
    // If non-NULL, the volume to format. Otherwise, will use the default external storage directory
    AutoPtr<IStorageVolume> mStorageVolume;

    // Access using getMountService()
    AutoPtr<IMountService> mMountService;

    AutoPtr<IStorageManager> mStorageManager;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<IProgressDialog> mProgressDialog;

    Boolean mFactoryReset;
    Boolean mAlwaysReset;
    String mReason;
};

} // namespace Storage
} // namespace Os
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_OS_STORAGE_CEXTERNALSTORAGEFORMATTER_H__
