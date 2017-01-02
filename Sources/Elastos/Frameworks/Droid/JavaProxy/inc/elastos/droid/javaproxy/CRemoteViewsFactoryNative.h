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

#ifndef __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSFACTORYNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSFACTORYNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CRemoteViewsFactoryNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Widget::IIRemoteViewsFactory;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Widget::IRemoteViews;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CRemoteViewsFactoryNative)
    , public Object
    , public IIRemoteViewsFactory
    , public IBinder
{
public:
    ~CRemoteViewsFactoryNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI OnDataSetChanged();

    CARAPI OnDataSetChangedAsync();

    CARAPI OnDestroy(
        /* [in] */ IIntent* intent);

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

    CARAPI IsCreated(
        /* [out] */ Boolean* res);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

}
}
}

#endif // __ELASTOS_DROID_JAVAPROXY_CREMOTEVIEWSFACTORYNATIVE_H__
