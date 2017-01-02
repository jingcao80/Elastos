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

#include "FilePreferencesFactoryImpl.h"
#include "FilePreferencesImpl.h"
#include "CSystem.h"

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Utility {
namespace Prefs {

//TODO:
Boolean FilePreferencesFactoryImpl::sInit;// = FilePreferencesFactoryImpl::InitStatic();
AutoPtr<IPreferences> FilePreferencesFactoryImpl::USER_ROOT;
AutoPtr<IPreferences> FilePreferencesFactoryImpl::SYSTEM_ROOT;

CAR_INTERFACE_IMPL(FilePreferencesFactoryImpl, Object, IPreferencesFactory)

Boolean FilePreferencesFactoryImpl::InitStatic()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String property;
    system->GetProperty(String("user.home"), &property);
    USER_ROOT = new FilePreferencesImpl(property + String("/.java/.userPrefs"), TRUE);

    system->GetProperty(String("java.home"), &property);
    SYSTEM_ROOT = new FilePreferencesImpl(property + String("/.systemPrefs"), FALSE);
    return TRUE;
}

FilePreferencesFactoryImpl::FilePreferencesFactoryImpl()
{
}

ECode FilePreferencesFactoryImpl::UserRoot(
    /* [out] */ IPreferences** outpre)
{
    VALIDATE_NOT_NULL(outpre);
    *outpre = USER_ROOT;
    REFCOUNT_ADD(*outpre);
    return NOERROR;
}

ECode FilePreferencesFactoryImpl::SystemRoot(
    /* [out] */ IPreferences** outpre)
{
    VALIDATE_NOT_NULL(outpre);
    *outpre = SYSTEM_ROOT;
    REFCOUNT_ADD(*outpre);
    return NOERROR;
}

ECode FilePreferencesFactoryImpl::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("Elastos.Utility.Prefs.FilePreferencesFactoryImpl");
    return NOERROR;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
