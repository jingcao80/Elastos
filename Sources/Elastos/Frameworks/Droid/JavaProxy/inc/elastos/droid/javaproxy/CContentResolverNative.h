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

#ifndef __ELASTOS_DROID_JAVAPROXY_CCONTENTRESOLVERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CCONTENTRESOLVERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CContentResolverNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentResolverOpenResourceIdResult;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CContentResolverNative)
    , public Object
    , public IContentResolver
    , public IBinder
{
public:
    ~CContentResolverNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI ReleaseProvider(
        /* [in] */ IIContentProvider* icp,
        /* [out] */ Boolean* isRelease);

    CARAPI ReleaseUnstableProvider(
        /* [in] */ IIContentProvider* icp,
        /* [out] */ Boolean* isRelease);

    CARAPI UnstableProviderDied(
        /* [in] */ IIContentProvider* icp);

    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    CARAPI GetStreamTypes(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [out, callee] */ ArrayOf<String>** streamTypes);

    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor);

    CARAPI Canonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    CARAPI Uncanonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    CARAPI OpenInputStream(
        /* [in] */ IUri* uri,
        /* [out] */ IInputStream** inStream);

    CARAPI OpenOutputStream(
        /* [in] */ IUri* uri,
        /* [out] */ IOutputStream** outStream);

    CARAPI OpenOutputStream(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IOutputStream** outStream);

    CARAPI OpenFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    CARAPI OpenFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    CARAPI OpenAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI OpenAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI OpenTypedAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI OpenTypedAssetFileDescriptor(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI GetResourceId(
        /* [in] */ IUri* uri,
        /* [out] */ IContentResolverOpenResourceIdResult** resourceIdResult);

    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** insertedUri);

    CARAPI ApplyBatch(
        /* [in] */ const String& authority,
        /* [in] */ IArrayList* operations,
        /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults);

    CARAPI BulkInsert(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* values,
        /* [out] */ Int32* number);

    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI Call(
        /* [in] */ IUri* uri,
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    CARAPI AcquireProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireExistingProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireProvider(
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireUnstableProvider(
        /* [in] */ IUri* uri,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireUnstableProvider(
        /* [in] */ const String& name,
        /* [out] */ IIContentProvider** contentProvider);

    CARAPI AcquireContentProviderClient(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderClient** client);

    CARAPI AcquireContentProviderClient(
        /* [in] */ const String& name,
        /* [out] */ IContentProviderClient** client);

    CARAPI AcquireUnstableContentProviderClient(
        /* [in] */ IUri* uri,
        /* [out] */ IContentProviderClient** client);

    CARAPI AcquireUnstableContentProviderClient(
        /* [in] */ const String& name,
        /* [out] */ IContentProviderClient** client);

    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendents,
        /* [in] */ IContentObserver* observer);

    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendents,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Int32 userHandle);

    CARAPI UnregisterContentObserver(
        /* [in] */ IContentObserver* observer);

    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer);

    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Boolean syncToNetwork);

    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IContentObserver* observer,
        /* [in] */ Boolean syncToNetwork,
        /* [in] */ Int32 userHandle);

    CARAPI TakePersistableUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode);

    CARAPI ReleasePersistableUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode);

    CARAPI GetPersistedUriPermissions(
        /* [out] */ IList** permissions);

    CARAPI GetOutgoingPersistedUriPermissions(
        /* [out] */ IList** permissions);

    CARAPI StartSync(
        /* [in] */ IUri* uri,
        /* [in] */ IBundle* extras);

    CARAPI CancelSync(
        /* [in] */ IUri* uri);

    CARAPI GetPackageName(
        /* [out] */ String* package);

    CARAPI ResolveUserId(
        /* [in] */ IUri* uri,
        /* [out] */ Int32* id);

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

#endif // __ELASTOS_DROID_JAVAPROXY_CCONTENTRESOLVERNATIVE_H__
