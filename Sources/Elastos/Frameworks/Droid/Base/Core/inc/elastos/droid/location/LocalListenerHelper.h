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

#ifndef __ELASTOS_DROID_LOCATION_LOCALLISTENERHELPER_H__
#define __ELASTOS_DROID_LOCATION_LOCALLISTENERHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Location {

class LocalListenerHelper
    : public Object
    , public ILocalListenerHelper
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Add(
        /* [in] */ IInterface* listener,
        /* [out] */ Boolean* result);

    CARAPI Remove(
        /* [in] */ IInterface* listener);

    virtual CARAPI RegisterWithServer(
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI UnregisterFromServer() = 0;

    CARAPI Foreach(
        /* [in] */ ILocalListenerHelperListenerOperation* operation);

protected:
    LocalListenerHelper(
        /* [in] */ const String& name);

private:
    AutoPtr<IHashSet> mListeners;
    const String mTag;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_LOCALLISTENERHELPER_H__
