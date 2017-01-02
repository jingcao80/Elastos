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


#include "elastos/droid/ext/frameworkdef.h"
#include "_Org.Conscrypt.h"
#include "elastos/droid/net/CRouteInfoHelper.h"
#include "elastos/droid/net/RouteInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CRouteInfoHelper, Singleton, IRouteInfoHelper)

CAR_SINGLETON_IMPL(CRouteInfoHelper)

ECode CRouteInfoHelper::MakeHostRoute(
    /* [in] */ IInetAddress* host,
    /* [in] */ const String& iface,
    /* [out] */ IRouteInfo** result)
{
    return RouteInfo::MakeHostRoute(host, iface, result);
}

ECode CRouteInfoHelper::MakeHostRoute(
    /* [in] */ IInetAddress* host,
    /* [in] */ IInetAddress* gateway,
    /* [in] */ const String& iface,
    /* [out] */ IRouteInfo** result)
{
    return RouteInfo::MakeHostRoute(host, gateway, iface, result);
}

ECode CRouteInfoHelper::SelectBestRoute(
    /* [in] */ ICollection* routes,
    /* [in] */ IInetAddress* dest,
    /* [out] */ IRouteInfo** info)
{
    return RouteInfo::SelectBestRoute(routes, dest, info);
}

} //namespace Net
} //namespace Droid
} //namespace Elastos
