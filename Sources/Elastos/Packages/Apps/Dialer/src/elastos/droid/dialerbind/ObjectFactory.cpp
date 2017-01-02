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

#include "elastos/droid/dialerbind/ObjectFactory.h"
// #include "elastos/droid/dialer/calllog/CallLogAdapter.h"

// using Elastos::Droid::Dialer::Calllog::CallLogAdapter;

namespace Elastos {
namespace Droid {
namespace DialerBind {

AutoPtr<ICachedNumberLookupService> ObjectFactory::NewCachedNumberLookupService()
{
    // no-op
    return NULL;
}

AutoPtr<CallLogAdapter> ObjectFactory::NewCallLogAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ICallFetcher* callFetcher,
    /* [in] */ ContactInfoHelper* contactInfoHelper,
    /* [in] */ ICallItemExpandedListener* callItemExpandedListener,
    /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    /* [in] */ Boolean isCallLog)
{
    AutoPtr<CallLogAdapter> adapter = new CallLogAdapter();
    adapter->constructor(context, callFetcher, contactInfoHelper,
            callItemExpandedListener, onReportButtonClickListener, isCallLog);
    return adapter;
}

AutoPtr<IDialogFragment> ObjectFactory::GetReportDialogFragment(
    /* [in] */ const String& name)
{
    return NULL;
}

} // DialerBind
} // Droid
} // Elastos

