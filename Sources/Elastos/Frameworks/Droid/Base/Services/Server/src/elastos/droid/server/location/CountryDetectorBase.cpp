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

#include "elastos/droid/server/location/CountryDetectorBase.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CountryDetectorBase::CountryDetectorBase(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    CHandler::New((IHandler**)&mHandler);
}

ECode CountryDetectorBase::SetCountryListener(
    /* [in] */ ICountryListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CountryDetectorBase::NotifyListener(
    /* [in] */ ICountry* country)
{
    if (mListener != NULL) {
        mListener->OnCountryDetected(country);
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos