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

#ifndef __ELASTOS_DROID_VIEW_IGRAPHICSSTATS_H__
#define __ELASTOS_DROID_VIEW_IGRAPHICSSTATS_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class IGrphicsStatsProxy
    : public Object
    , public IIGraphicsStats
{
public:
    IGrphicsStatsProxy(
        /* [in] */ android::sp<android::IBinder> remote);

    CAR_INTERFACE_DECL();

    CARAPI RequestBufferForProcess(
        /* [in] */ const String& packageName,
        /* [in] */ IIGraphicsStatsCallback* cb,
        /* [out] */ IParcelFileDescriptor** fd);

private:
    static const String TAG;
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_requestBufferForProcess;

    android::sp<android::IBinder> mRemote;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_IGRAPHICSSTATS_H__
