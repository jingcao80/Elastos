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

#ifndef __ELASTOS_DROID_WIDGET_REMOTEVIEWSSERVICE_H__
#define __ELASTOS_DROID_WIDGET_REMOTEVIEWSSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#define HASH_FOR_CONTENT
#include "elastos/droid/ext/frameworkhash.h"

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Service.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntentFilterComparison;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::Widget::IIRemoteViewsFactory;

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

class RemoteViewsService 
    : public Elastos::Droid::App::Service
    , public IRemoteViewsService
{
public:
    class RemoteViewsFactoryAdapter
        : public Object
        , public IIRemoteViewsFactory
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        RemoteViewsFactoryAdapter();

        CARAPI constructor(
            /* [in] */ IRemoteViewsFactory* factory,
            /* [in] */ Boolean isCreated);

        CARAPI IsCreated(
            /* [out] */ Boolean* result);

        CARAPI OnDataSetChanged();

        CARAPI OnDataSetChangedAsync();

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetViewAt(
            /* [in] */ Int32 position,
            /* [out] */ IParcelable** views);

        CARAPI GetLoadingView(
            /* [out] */ IParcelable** views);


        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* id);

        CARAPI HasStableIds(
            /* [out] */ Boolean* res);

        CARAPI OnDestroy(
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IRemoteViewsFactory> mFactory;
        Boolean mIsCreated;
    };

    CAR_INTERFACE_DECL()

    virtual CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

private:
    static const String TAG;

    static HashMap< AutoPtr<IIntentFilterComparison>, AutoPtr<IRemoteViewsFactory> > sRemoteViewFactories;
    static Object sLock;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_REMOTEVIEWSSERVICE_H__
