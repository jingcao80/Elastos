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

#ifndef __ORG_ALLJOYN_BUS_ONPINGLISTENER_H__
#define __ORG_ALLJOYN_BUS_ONPINGLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

class OnPingListener
    : public Object
    , public IOnPingListener
{
public:
    CAR_INTERFACE_DECL()

    OnPingListener();

    virtual ~OnPingListener();

    CARAPI constructor();

    CARAPI OnPing(
        /* [in] */ Int32 status,
        /* [in] */ IInterface* context);

private:
    /**
     * Create any native resources held by objects of this class.  Specifically,
     * we allocate a C++ counterpart of this OnPingListener object.
     */
    void Create();

    /**
     * Release any native resources held by objects of this class.
     * Specifically, we may delete a C++ counterpart of this OnPingListener object.
     */
    void Destroy();

private:
    friend class CBusAttachment;

    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ONPINGLISTENER_H__
