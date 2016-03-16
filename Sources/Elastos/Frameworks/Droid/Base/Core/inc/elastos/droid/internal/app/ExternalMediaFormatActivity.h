
#ifndef __ELASTOS_DROID_INTERNAL_APP_EXTERNALMEDIAFORMATACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_EXTERNALMEDIAFORMATACTIVITY_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/app/AlertActivity.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageVolume;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ExternalMediaFormatActivity
    : public AlertActivity
    , public IExternalMediaFormatActivity
    , public IDialogInterfaceOnClickListener
{
private:
    class StorageReceiver : public BroadcastReceiver
    {
    public:
        StorageReceiver(
            /* [in] */ ExternalMediaFormatActivity* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        ExternalMediaFormatActivity* mHost;
    };

    class OnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        OnClickListener(
            /* [in] */ ExternalMediaFormatActivity* host);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ExternalMediaFormatActivity* mHost;
    };

public:
    ExternalMediaFormatActivity();

    CAR_INTERFACE_DECL()

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnResume();

    CARAPI OnPause();

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    static const Int32 POSITIVE_BUTTON;

    AutoPtr<IStorageManager> mStorageManager;
    AutoPtr<IStorageVolume> mStorageVolume;

    AutoPtr<IBroadcastReceiver> mStorageReceiver;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_EXTERNALMEDIAFORMATACTIVITY_H__
