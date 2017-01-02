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

#include "elastos/droid/server/firewall/AndFilter.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Firewall {

//------------------------------------------------------------------------------
// AndFilter::FACTORY_FilterFactory
//------------------------------------------------------------------------------

AndFilter::FACTORY_FilterFactory::FACTORY_FilterFactory(
    /* [in] */ const String& tag)
{
    FilterFactory::constructor(tag);
}

ECode AndFilter::FACTORY_FilterFactory::NewFilter(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<AndFilter> filter = new AndFilter();
    filter->ReadFromXml(parser);
    *result = IFilter::Probe(filter);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//=======================================================================================
// AndFilter
//=======================================================================================

const AutoPtr<AndFilter::FACTORY_FilterFactory> AndFilter::FACTORY = new AndFilter::FACTORY_FilterFactory(String("and"));

ECode AndFilter::Matches(
    /* [in] */ IIntentFirewall* ifw,
    /* [in] */ IComponentName* resolvedComponent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 receivingUid,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)

    Int32 size = 0;
    mChildren->GetSize(&size);

    for (Int32 i = 0; i < size; i++) {

        AutoPtr<IInterface> filter;
        mChildren->Get(i, (IInterface**)&filter);

        IFilter::Probe(filter)->Matches(ifw, resolvedComponent, intent, callerUid, callerPid,
                resolvedType, receivingUid, ret);
        if (! *ret) {
            return NOERROR;
        }
    }

    *ret = TRUE;
    return NOERROR;
}

} // Firewall
} // Server
} // Droid
} // Elastos
