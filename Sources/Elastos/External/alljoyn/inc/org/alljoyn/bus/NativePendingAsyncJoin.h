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

#ifndef __ORG_ALLJOYN_BUS_NATIVEPENDINGASYNCJOIN_H__
#define __ORG_ALLJOYN_BUS_NATIVEPENDINGASYNCJOIN_H__

#include "_Org.Alljoyn.Bus.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Droid::Content::IContext;

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativePendingAsyncJoin
    : public Object
{
public:
    NativePendingAsyncJoin(
        /* [in] */ ISessionListener* sessionListener,
        /* [in] */ IOnJoinSessionListener* onJoinSessionListener,
        /* [in] */ IInterface* context);

private:
    NativePendingAsyncJoin(const NativePendingAsyncJoin& other);
    NativePendingAsyncJoin& operator =(const NativePendingAsyncJoin& other);

public:
    AutoPtr<ISessionListener> mSessionListener;
    AutoPtr<IOnJoinSessionListener> mOnJoinSessionListener;
    AutoPtr<IInterface> mContext;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEPENDINGASYNCJOIN_H__
