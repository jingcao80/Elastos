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

#ifndef __ELASTOS_DROID_INTERNAL_TEXTSERVICE_ITEXTSERVICESMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TEXTSERVICE_ITEXTSERVICESMANAGER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::TextService::ISpellCheckerInfo;
using Elastos::Droid::View::TextService::ISpellCheckerSubtype;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace TextService {

class ITextServicesManagerProxy
    : public Object
    , public IITextServicesManager
{
public:
    ITextServicesManagerProxy(
        /* [in] */ android::sp<android::IBinder>& remote);

    CAR_INTERFACE_DECL();

    CARAPI GetCurrentSpellChecker(
        /* [in] */ const String& locale,
        /* [out] */ ISpellCheckerInfo** info);

    CARAPI GetCurrentSpellCheckerSubtype(
        /* [in] */ const String& locale,
        /* [in] */ Boolean allowImplicitlySelectedSubtype,
        /* [out] */ ISpellCheckerSubtype** subtype);

    CARAPI GetSpellCheckerService(
        /* [in] */ const String& sciId,
        /* [in] */ const String& locale,
        /* [in] */ IITextServicesSessionListener* tsListener,
        /* [in] */ IISpellCheckerSessionListener* scListener,
        /* [in] */ IBundle* bundle);

    CARAPI FinishSpellCheckerService(
        /* [in] */ IISpellCheckerSessionListener* listener);

    CARAPI SetCurrentSpellChecker(
        /* [in] */ const String& locale,
        /* [in] */ const String& sciId);

    CARAPI SetCurrentSpellCheckerSubtype(
        /* [in] */ const String& locale,
        /* [in] */ Int32 hashCode);

    CARAPI SetSpellCheckerEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsSpellCheckerEnabled(
        /* [out] */ Boolean* ebl);

    CARAPI GetEnabledSpellCheckers(
        /* [out, callee] */ ArrayOf<ISpellCheckerInfo*>** infoArray);

private:
    static const String TAG;
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_getCurrentSpellChecker;
    static const Int32 TRANSACTION_getCurrentSpellCheckerSubtype;
    static const Int32 TRANSACTION_isSpellCheckerEnabled;

    android::sp<android::IBinder> mRemote;
};

} // namespace TextService
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TEXTSERVICE_ITEXTSERVICESMANAGER_H__
