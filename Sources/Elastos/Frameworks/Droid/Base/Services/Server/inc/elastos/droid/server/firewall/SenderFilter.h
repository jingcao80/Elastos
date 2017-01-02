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

#ifndef __ELASTOS_DROID_Server_Firewall_SenderFilter_H__
#define __ELASTOS_DROID_Server_Firewall_SenderFilter_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/server/firewall/FilterFactory.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Utility::IArrayList;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

class SenderFilter
    : public Object
{
public:
    class FACTORY_FilterFactory
        : public FilterFactory
    {
    public:
        FACTORY_FilterFactory(
            /* [in] */ const String& tag);

        CARAPI NewFilter(
            /* [in] */ IXmlPullParser* parser,
            /* [out] */ IFilter** result);
    };

    class SIGNATURE_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Matches(
            /* [in] */ IIntentFirewall* ifw,
            /* [in] */ IComponentName* resolvedComponent,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ const String& resolvedType,
            /* [in] */ Int32 receivingUid,
            /* [out] */ Boolean *ret);
    };

    class SYSTEM_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Matches(
            /* [in] */ IIntentFirewall* ifw,
            /* [in] */ IComponentName* resolvedComponent,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ const String& resolvedType,
            /* [in] */ Int32 receivingUid,
            /* [out] */ Boolean *ret);
    };

    class SYSTEM_OR_SIGNATURE_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Matches(
            /* [in] */ IIntentFirewall* ifw,
            /* [in] */ IComponentName* resolvedComponent,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ const String& resolvedType,
            /* [in] */ Int32 receivingUid,
            /* [out] */ Boolean *ret);
    };

    class USER_ID_Filter
        : public Object
        , public IFilter
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Matches(
            /* [in] */ IIntentFirewall* ifw,
            /* [in] */ IComponentName* resolvedComponent,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 callerUid,
            /* [in] */ Int32 callerPid,
            /* [in] */ const String& resolvedType,
            /* [in] */ Int32 receivingUid,
            /* [out] */ Boolean *ret);
    };

public:
    CAR_INTERFACE_DECL()

    //@Override
    CARAPI Matches(
        /* [in] */ IIntentFirewall* ifw,
        /* [in] */ IComponentName* resolvedComponent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 receivingUid,
        /* [out] */ Boolean *ret);

    static Boolean IsPrivilegedApp(
        /* [in] */ Int32 callerUid,
        /* [in] */ Int32 callerPid);

public:
    static const AutoPtr<FACTORY_FilterFactory> FACTORY;

    static const AutoPtr<SIGNATURE_Filter> SIGNATURE;
    static const AutoPtr<SYSTEM_Filter> SYSTEM;
    static const AutoPtr<SYSTEM_OR_SIGNATURE_Filter> SYSTEM_OR_SIGNATURE;
    static const AutoPtr<USER_ID_Filter> USER_ID;

private:
    static const String ATTR_TYPE;

    static const String VAL_SIGNATURE;
    static const String VAL_SYSTEM;
    static const String VAL_SYSTEM_OR_SIGNATURE;
    static const String VAL_USER_ID;
};

} // Firewall
} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_Server_Firewall_SenderFilter_H__
