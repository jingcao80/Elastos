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

#ifndef __ELASTOSX_XML_DATATYPE_XMLGREGORIANCALENDAR_H__
#define __ELASTOSX_XML_DATATYPE_XMLGREGORIANCALENDAR_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Core::ICloneable;
using Elastos::Math::IBigDecimal;

namespace Elastosx {
namespace Xml {
namespace Datatype {

class XMLGregorianCalendar
    : public Object
    , public IXMLGregorianCalendar
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI SetTime(
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute,
            /* [in] */ Int32 second);

    CARAPI SetTime(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ IBigDecimal* fractional);

    CARAPI SetTime(
        /* [in] */ Int32 hour,
        /* [in] */ Int32 minute,
        /* [in] */ Int32 second,
        /* [in] */ Int32 millisecond);

    CARAPI GetMillisecond(
        /* [out] */ Int32* second);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* info);
};

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx

#endif
