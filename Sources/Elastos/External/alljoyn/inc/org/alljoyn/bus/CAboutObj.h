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

#ifndef __ORG_ALLJOYN_BUS_CABOUTOBJ_H__
#define __ORG_ALLJOYN_BUS_CABOUTOBJ_H__

#include "_Org_Alljoyn_Bus_CAboutObj.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CAboutObj)
    , public Object
    , public IAboutObj
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAboutObj();

    ~CAboutObj();

    CARAPI constructor(
        /* [in] */ IBusAttachment* bus);

    CARAPI constructor(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ Boolean isAboutObjectAnnounced);

    CARAPI Announce(
        /* [in] */ Int16 sessionPort,
        /* [in] */ IAboutDataListener* aboutData);

    /**
     * Cancel the last announce signal sent. If no signals have been sent this
     * method call will return.
     *
     * @return Status.OK on success
     */
    CARAPI Unannounce();

private:
    void Destory();

private:
    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CABOUTOBJ_H__
