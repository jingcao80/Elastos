//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
