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

#ifndef __ELASTOS_DROID_JAVAPROXY_CAPPWIDGETHOSTCALLBACKSNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CAPPWIDGETHOSTCALLBACKSNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CAppWidgetHostCallbacksNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Internal::AppWidget::IIAppWidgetHost;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Widget::IRemoteViews;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CAppWidgetHostCallbacksNative)
    , public Object
    , public IIAppWidgetHost
    , public IBinder
{
public:
    ~CAppWidgetHostCallbacksNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI UpdateAppWidget(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IRemoteViews* pViews);

    CARAPI ProviderChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo * pInfo);

    CARAPI ProvidersChanged();

    CARAPI ViewDataChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ Int32 viewId);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CAPPWIDGETHOSTCALLBACKSNATIVE_H__
