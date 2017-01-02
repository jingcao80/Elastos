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

#include "elastos/droid/server/CKillSwitchService.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Os::EIID_IIKillSwitchService;
using Elastos::Droid::Os::Binder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CKillSwitchService::TAG("CKillSwitchService");
Boolean CKillSwitchService::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);

//=======================================================================
// CKillSwitchService::
//=======================================================================
CAR_OBJECT_IMPL(CKillSwitchService)

CAR_INTERFACE_IMPL(CKillSwitchService, Object, IIKillSwitchService)

CKillSwitchService::CKillSwitchService()
{
    CArrayList::New((IList**)&mPackagesAllowedToWrite);
}

ECode CKillSwitchService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String deviceKillSwitchLib;
    res->GetString(
            R::string::config_killSwitchLib, &deviceKillSwitchLib);

    String deviceKillSwitchClass;
    res->GetString(
            R::string::config_killSwitchClass, &deviceKillSwitchClass);

    if (!deviceKillSwitchLib.IsEmpty() && !deviceKillSwitchClass.IsEmpty()) {
        // DexClassLoader loader = new DexClassLoader(deviceKillSwitchLib,
        //         new ContextWrapper(mContext).getCacheDir().getAbsolutePath(),
        //         NULL,
        //         ClassLoader.getSystemClassLoader());
        // // try {
        // Class<?> klass = loader.loadClass(deviceKillSwitchClass);
        // Constructor<?> constructor = klass.getConstructor();
        // mKillSwitchImpl = (IKillSwitch) constructor.newInstance();
        // if (DEBUG) Slog.d(TAG, "KillSwitch class loaded");

        AutoPtr<ArrayOf<String> > stringArray;
        res->GetStringArray(
                R::array::config_packagesAllowedAccessToKillSwitch, (ArrayOf<String>**)&stringArray);
        AutoPtr<IList> l;
        Arrays::AsList(stringArray, (IList**)&l);
        mPackagesAllowedToWrite->AddAll(ICollection::Probe(l));
        // } catch (Exception e) {
        //     mKillSwitchImpl = NULL;
        //     mPackagesAllowedToWrite.clear();
        //     Slog.w(TAG, "Could not instantiate KillSwitch "
        //             + deviceKillSwitchClass + " from class "
        //             + deviceKillSwitchLib, e);
        // }
    }
    return NOERROR;
}

Boolean CKillSwitchService::VerifyWritePermission()
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<ArrayOf<String> > packagesForUid;
    pm->GetPackagesForUid(Binder::GetCallingUid(), (ArrayOf<String>**)&packagesForUid);
    if (packagesForUid != NULL) {
        for (Int32 i = 0; i < packagesForUid->GetLength(); i++) {
            AutoPtr<ICharSequence> p;
            CString::New((*packagesForUid)[i], (ICharSequence**)&p);
            Boolean bContain = FALSE;
            mPackagesAllowedToWrite->Contains(p, &bContain);
            if (bContain) {
                return TRUE;
            }
        }
    }
    // throw new SecurityException("not in security access list");
    return FALSE;
}

ECode CKillSwitchService::HasKillSwitch(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mKillSwitchImpl != NULL;
    return NOERROR;
}

ECode CKillSwitchService::SetDeviceUuid(
    /* [in] */ const String& uuid)
{
    VerifyWritePermission();
    if (mKillSwitchImpl != NULL) {
        mKillSwitchImpl->SetDeviceUuid(uuid);
        return NOERROR;
    }
    else {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

ECode CKillSwitchService::GetDeviceUuid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    VerifyWritePermission();
    if (mKillSwitchImpl != NULL) {
        return mKillSwitchImpl->GetDeviceUuid(result);
    }
    else {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

ECode CKillSwitchService::IsDeviceLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mKillSwitchImpl != NULL) {
        return mKillSwitchImpl->IsDeviceLocked(result);
    }
    else {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

ECode CKillSwitchService::SetDeviceLocked(
    /* [in] */ Boolean locked)
{
    VerifyWritePermission();
    if (mKillSwitchImpl != NULL) {
        return mKillSwitchImpl->SetDeviceLocked(locked);
    }
    else {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

ECode CKillSwitchService::SetAccountId(
    /* [in] */ const String& value)
{
    VerifyWritePermission();
    if (mKillSwitchImpl != NULL) {
        return mKillSwitchImpl->SetAccountId(value);
    }
    else {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

ECode CKillSwitchService::GetAccountId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    VerifyWritePermission();
    if (mKillSwitchImpl != NULL) {
        return mKillSwitchImpl->GetAccountId(result);
    }
    else {
        // throw new UnsupportedOperationException();
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
