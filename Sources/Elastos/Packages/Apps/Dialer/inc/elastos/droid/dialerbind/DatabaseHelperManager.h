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

#ifndef __ELASTOS_DROID_DIALERBIND_DATABASEHELPERMANAGER_H__
#define __ELASTOS_DROID_DIALERBIND_DATABASEHELPERMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/database/DialerDatabaseHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Dialer::Database::DialerDatabaseHelper;

namespace Elastos {
namespace Droid {
namespace DialerBind {

class DatabaseHelperManager
{
public:
    static CARAPI_(AutoPtr<DialerDatabaseHelper>) GetDatabaseHelper(
        /* [in] */ IContext* context);
};

} // DialerBind
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALERBIND_DATABASEHELPERMANAGER_H__
