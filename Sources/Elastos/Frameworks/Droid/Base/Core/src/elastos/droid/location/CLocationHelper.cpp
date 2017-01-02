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

#include "elastos/droid/location/CLocationHelper.h"
#include "elastos/droid/location/Location.h"

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CLocationHelper, Singleton, ILocationHelper)

CAR_SINGLETON_IMPL(CLocationHelper)

ECode CLocationHelper::Convert(
    /* [in] */ Double coordinate,
    /* [in] */ Int32 outputType,
    /* [out] */ String* str)
{
    return Location::Convert(coordinate, outputType, str);
}

ECode CLocationHelper::Convert(
    /* [in] */ const String& coordinate,
    /* [out] */ Double* d)
{
    return Location::Convert(coordinate, d);
}

ECode CLocationHelper::DistanceBetween(
    /* [in] */ Double startLatitude,
    /* [in] */ Double startLongitude,
    /* [in] */ Double endLatitude,
    /* [in] */ Double endLongitude,
    /* [in] */ ArrayOf<Float>* results)
{
    return Location::DistanceBetween(startLatitude, startLongitude,endLatitude,endLongitude,results);
}

}
}
}