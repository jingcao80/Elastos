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

#ifndef __ELASTOS_DROID_DIALER_DIALERAPPLICATION_H__
#define __ELASTOS_DROID_DIALER_DIALERAPPLICATION_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/app/Application.h>

using Elastos::Droid::App::Application;
using Elastos::Droid::Contacts::Common::IContactPhotoManager;

namespace Elastos {
namespace Droid {
namespace Dialer {

class DialerApplication
    : public Application
{
public:

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI GetSystemService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** object);

private:
    AutoPtr<IContactPhotoManager> mContactPhotoManager;
};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALERAPPLICATION_H__
