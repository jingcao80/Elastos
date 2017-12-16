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

#ifndef __ELASTOS_DROID_INTERNAL_APP_IVOICEINTERACTOR_H__
#define __ELASTOS_DROID_INTERNAL_APP_IVOICEINTERACTOR_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class IVoiceInteractorProxy
    : public Object
    , public IIVoiceInteractor
{
public:
    CAR_INTERFACE_DECL();

    IVoiceInteractorProxy(
        /* [in] */ const android::sp<android::IBinder>& bpBinder);

    CARAPI StartConfirmation(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* prompt,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartCompleteVoice(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartAbortVoice(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ ICharSequence* message,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI StartCommand(
        /* [in] */ const String& callingPackage,
        /* [in] */ IIVoiceInteractorCallback* clb,
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ IIVoiceInteractorRequest** result);

    CARAPI SupportsCommands(
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<String>* commands,
        /* [out, callee] */ ArrayOf<Boolean>** result);

private:
    const android::sp<android::IBinder> mBpBinder;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_IVOICEINTERACTOR_H__
