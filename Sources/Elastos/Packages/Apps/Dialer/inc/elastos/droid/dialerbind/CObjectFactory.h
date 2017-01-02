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

#ifndef __ELASTOS_DROID_DIALERBIND_COBJECTFACTORY_H__
#define __ELASTOS_DROID_DIALERBIND_COBJECTFACTORY_H__

#include "_Elastos_Droid_DialerBind_CObjectFactory.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapter;
using Elastos::Droid::Dialer::CallLog::ICallFetcher;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterCallItemExpandedListener;
using Elastos::Droid::Dialer::CallLog::ICallLogAdapterOnReportButtonClickListener;
using Elastos::Droid::Dialer::CallLog::IContactInfoHelper;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Droid {
namespace DialerBind {

CarClass(CObjectFactory)
    , public Singleton
    , public IObjectFactory
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI NewCachedNumberLookupService(
        /* [out] */ ICachedNumberLookupService** service);

    /**
     * Create a new instance of the call log adapter.
     * @param context The context to use.
     * @param callFetcher Instance of call fetcher to use.
     * @param contactInfoHelper Instance of contact info helper class to use.
     * @param isCallLog Is this call log adapter being used on the call log?
     * @return Instance of CallLogAdapter.
     */
    CARAPI NewCallLogAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ICallFetcher* callFetcher,
        /* [in] */ IContactInfoHelper* contactInfoHelper,
        /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
        /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
        /* [in] */ Boolean isCallLog,
        /* [out] */ ICallLogAdapter** adapter);

    CARAPI GetReportDialogFragment(
        /* [in] */ const String& name,
        /* [out] */ IDialogFragment** fragment);

    static CARAPI_(AutoPtr<ICachedNumberLookupService>) NewCachedNumberLookupService();

    static CARAPI_(AutoPtr<ICallLogAdapter>) NewCallLogAdapter(
        /* [in] */ IContext* context,
        /* [in] */ ICallFetcher* callFetcher,
        /* [in] */ IContactInfoHelper* contactInfoHelper,
        /* [in] */ ICallLogAdapterCallItemExpandedListener* callItemExpandedListener,
        /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
        /* [in] */ Boolean isCallLog);

    static CARAPI_(AutoPtr<IDialogFragment>) GetReportDialogFragment(
        /* [in] */ const String& name);
};

} // DialerBind
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALERBIND_COBJECTFACTORY_H__
