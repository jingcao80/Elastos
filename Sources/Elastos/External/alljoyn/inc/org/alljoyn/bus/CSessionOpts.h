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

#ifndef __ORG_ALLJOYN_BUS_CSESSIONOPTS_H__
#define __ORG_ALLJOYN_BUS_CSESSIONOPTS_H__

#include "_Org_Alljoyn_Bus_CSessionOpts.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CSessionOpts)
    , public Object
    , public ISessionOpts
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Byte traffic,
        /* [in] */ Boolean isMultipoint,
        /* [in] */ Byte proximity,
        /* [in] */ Int16 transports);

    CARAPI GetTraffic(
        /* [out] */ Byte* traffic);

    CARAPI SetTraffic(
        /* [in] */ Byte traffic);

    CARAPI IsMultipoint(
        /* [out] */ Boolean* isMultipoint);

    CARAPI SetMultipoint(
        /* [in] */ Boolean multipoint);

    CARAPI GetProximity(
        /* [out] */ Byte* proximity);

    CARAPI SetProximity(
        /* [in] */ Byte proximity);

    CARAPI GetTransports(
        /* [out] */ Int16* transports);

    CARAPI SetTransports(
        /* [in] */ Int16 transports);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * Holds the traffic type for this SessionOpt
     */
    Byte mTraffic;

    /**
     * Multi-point session capable.
     *
     * A session is multi-point if it can be joined multiple times to form a single
     * session with multi (greater than 2) endpoints. When false, each join attempt
     * creates a new point-to-point session.
     */
    Boolean mIsMultipoint;

    /**
     * Holds the proximity for this SessionOpt
     */
    Byte mProximity;

    /**
     * Holds the allowed transports for this SessionOpt
     */
    Int16 mTransports;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CSESSIONOPTS_H__
