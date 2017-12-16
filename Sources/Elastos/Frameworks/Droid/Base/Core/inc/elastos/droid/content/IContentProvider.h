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

#ifndef __ELASTOS_DROID_CONTENT_ICONTENTPROVIDER_H__
#define __ELASTOS_DROID_CONTENT_ICONTENTPROVIDER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::IBulkCursorDescriptor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Content {

class IContentProviderProxy2
    : public Object
    , public IIContentProvider
{
public:
    IContentProviderProxy2(
        /* [in] */ android::sp<android::IBinder> remote);

    CAR_INTERFACE_DECL();

    CARAPI Query(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ ICursor** cursor);

    // internal use
    CARAPI Query(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IBulkCursorDescriptor** descriptor);

    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    CARAPI Insert(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** insertedUri);

    CARAPI BulkInsert(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<IContentValues*>* initialValues,
        /* [out] */ Int32* number);

    CARAPI Delete(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI Update(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI OpenFile(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    CARAPI OpenAssetFile(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    CARAPI ApplyBatch(
        /* [in] */ const String& callingPkg,
        /* [in] */ IArrayList* operations,
        /* [out, callee] */ ArrayOf<IContentProviderResult*>** providerResults);

    CARAPI Call(
        /* [in] */ const String& callingPkg,
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    CARAPI CreateCancellationSignal(
        /* [out] */ IICancellationSignal** cancellationSignal);

    CARAPI Canonicalize(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    CARAPI Uncanonicalize(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    CARAPI GetStreamTypes(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [out, callee] */ ArrayOf<String>** streamTypes);

    CARAPI OpenTypedAssetFile(
        /* [in] */ const String& callingPkg,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType,
        /* [in] */ IBundle* opts,
        /* [in] */ IICancellationSignal* cancellationSignal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

private:
    static const String TAG;
    static const String DESCRIPTOR;
    static const Int32 CALL_TRANSACTION;

    android::sp<android::IBinder> mRemote;
};

} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_ICONTENTPROVIDER_H__
