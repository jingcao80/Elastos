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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_EXTENSIONS_EXTENSIONSFACTORY_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_EXTENSIONS_EXTENSIONSFACTORY_H__

#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos.Droid.Dialer.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Contacts::Common::List::IExtendedPhoneDirectoriesManager;
using Elastos::Utility::IProperties;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Extensions {

/*
 * A framework for adding extensions to Dialer. This class reads a property file from
 * assets/contacts_extensions.properties and loads extension classes that an app has defined. If
 * an extension class was not defined, null is returned.
 */
class ExtensionsFactory
{
public:
    static CARAPI_(void) Init(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IExtendedPhoneDirectoriesManager>) GetExtendedPhoneDirectoriesManager();

private:
    static CARAPI_(AutoPtr<IInterface>) CreateInstance(
        /* [in] */ const String& className);

private:
    static const String TAG;// = "ExtensionsFactory";

    // Config filename for mappings of various class names to their custom
    // implementations.
    static const String EXTENSIONS_PROPERTIES;// = "contacts_extensions.properties";

    static const String EXTENDED_PHONE_DIRECTORIES_KEY;// = "extendedPhoneDirectories";

    static AutoPtr<IProperties> sProperties;
    static AutoPtr<IExtendedPhoneDirectoriesManager> sExtendedPhoneDirectoriesManager;
};

} // namespace Extensions
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_EXTENSIONS_EXTENSIONSFACTORY_H__
