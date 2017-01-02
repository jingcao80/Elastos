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

#ifndef __Elastos_DevSamples_DFObserver_MyAboutData_H__
#define __Elastos_DevSamples_DFObserver_MyAboutData_H__

#include <_Org.Alljoyn.Bus.h>
#include <elastos/core/Object.h>

using Org::Alljoyn::Bus::IAboutDataListener;
using Elastos::Utility::IMap;

namespace Elastos {
namespace DevSamples {
namespace DFObserver {

class MyAboutData
    : public Object
    , public IAboutDataListener
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetAboutData(
        /* [in] */ const String& language,
        /* [out] */ IMap** data);

    CARAPI GetAnnouncedAboutData(
        /* [out] */ IMap** data);
};

} // namespace DFObserver
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DFObserver_MyAboutData_H__
