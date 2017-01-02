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

#ifndef __ELASTOS_UTILITY_PREFS_FILEPREFERENCESFACTORYIMPL_H__
#define __ELASTOS_UTILITY_PREFS_FILEPREFERENCESFACTORYIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Prefs {

class FilePreferencesFactoryImpl
    : public Object
    , public IPreferencesFactory
{
public:
    CAR_INTERFACE_DECL()

    FilePreferencesFactoryImpl();

    CARAPI UserRoot(
        /* [out] */ IPreferences** outpre);

    CARAPI SystemRoot(
        /* [out] */ IPreferences** outpre);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    static CARAPI_(Boolean) InitStatic();

private:
    static Boolean sInit;
    //  user root preferences
    static AutoPtr<IPreferences> USER_ROOT;

    //  system root preferences
    static AutoPtr<IPreferences> SYSTEM_ROOT;

};

} // namespace Prefs
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__PREFS_FILEPREFERENCESFACTORYIMPL_H__
