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

#include "CObjectFactory.h"

namespace Elastos {
namespace Droid {
namespace DialerBind {

CAR_INTERFACE_IMPL(CObjectFactory, Singleton, IObjectFactory);

CAR_SINGLETON_IMPL(CObjectFactory);

ECode CObjectFactory::NewCachedNumberLookupService(
        /* [out] */ ICachedNumberLookupService** service)
{
    VALIDATE_NOT_NULL(service);
    AutoPtr<ICachedNumberLookupService> temp = NewCachedNumberLookupService();
    *service = temp;
    if (*service) {
        REFCOUNT_ADD(*service);
    }
    return NOERROR;
}

ECode CObjectFactory::NewCallLogAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ICallFetcher* callFetcher,
    /* [in] */ IContactInfoHelper* contactInfoHelper,
    /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
    /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    /* [in] */ Boolean isCallLog,
    /* [out] */ ICallLogAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    AutoPtr<ICallLogAdapter> temp = NewCallLogAdapter(context, callFetcher,
            contactInfoHelper, callItemExpandedListener,
            onReportButtonClickListener, isCallLog);
    *adapter = temp;
    REFCOUNT_ADD(*adapter);
    return NOERROR;
}

ECode CObjectFactory::GetReportDialogFragment(
    /* [in] */ const String& name,
    /* [out] */ IDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    AutoPtr<IDialogFragment> temp = GetReportDialogFragment(name);
    *fragment = temp;
    if (*fragment) {
        REFCOUNT_ADD(*fragment);
    }
    return NOERROR;
}

AutoPtr<ICachedNumberLookupService> CObjectFactory::NewCachedNumberLookupService()
{
    // no-op
    return NULL;
}

AutoPtr<ICallLogAdapter> CObjectFactory::NewCallLogAdapter(
    /* [in] */ IContext* context,
    /* [in] */ ICallFetcher* callFetcher,
    /* [in] */ IContactInfoHelper* contactInfoHelper,
    /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
    /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
    /* [in] */ Boolean isCallLog)
{
    AutoPtr<ICallLogAdapter> adapter;
    CCallLogAdapter::New(context, callFetcher, contactInfoHelper,
            callItemExpandedListener, onReportButtonClickListener,
            isCallLog, (ICallLogAdapter**)&adapter);
    return adapter;
}

AutoPtr<IDialogFragment> CObjectFactory::GetReportDialogFragment(
    /* [in] */ const String& name)
{
    return NULL;
}

} // DialerBind
} // Droid
} // Elastos

