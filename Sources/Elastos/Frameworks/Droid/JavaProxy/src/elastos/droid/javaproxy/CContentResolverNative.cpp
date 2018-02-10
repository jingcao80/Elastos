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

#include "elastos/droid/javaproxy/CContentResolverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IContentResolver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CContentResolverNative::TAG("CContentResolverNative");

CAR_INTERFACE_IMPL_2(CContentResolverNative, Object, IContentResolver, IBinder)

CAR_OBJECT_IMPL(CContentResolverNative)

CContentResolverNative::~CContentResolverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CContentResolverNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CContentResolverNative::ReleaseProvider(
    /* [in] */ IIContentProvider* icp,
    /* [out] */ Boolean* isRelease)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::ReleaseUnstableProvider(
    /* [in] */ IIContentProvider* icp,
    /* [out] */ Boolean* isRelease)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::UnstableProviderDied(
    /* [in] */ IIContentProvider* icp)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetStreamTypes(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String>** streamTypes)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Query(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Canonicalize(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Uncanonicalize(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenInputStream(
    /* [in] */ IUri* uri,
    /* [out] */ IInputStream** inStream)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenOutputStream(
    /* [in] */ IUri* uri,
    /* [out] */ IOutputStream** outStream)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenOutputStream(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IOutputStream** outStream)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenTypedAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenTypedAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetResourceId(
    /* [in] */ IUri* uri,
    /* [out] */ IContentResolverOpenResourceIdResult** resourceIdResult)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** insertedUri)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::ApplyBatch(
    /* [in] */ const String& authority,
    /* [in] */ IArrayList* operations,
    /* [out, callee] */ ArrayOf<IContentProviderResult *>** providerResults)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::BulkInsert(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues *>* values,
    /* [out] */ Int32* number)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Update(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Call(
    /* [in] */ IUri* uri,
    /* [in] */ const String& method,
    /* [in] */ const String& arg,
    /* [in] */ IBundle* extras,
    /* [out] */ IBundle** bundle)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireExistingProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireProvider(
    /* [in] */ const String& name,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableProvider(
    /* [in] */ const String& name,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireContentProviderClient(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireContentProviderClient(
    /* [in] */ const String& name,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableContentProviderClient(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableContentProviderClient(
    /* [in] */ const String& name,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendents,
    /* [in] */ IContentObserver* observer)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendents,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Int32 userHandle)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Boolean syncToNetwork)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Boolean syncToNetwork,
    /* [in] */ Int32 userHandle)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::TakePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::ReleasePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetPersistedUriPermissions(
    /* [out] */ IList** permissions)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetOutgoingPersistedUriPermissions(
    /* [out] */ IList** permissions)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::StartSync(
    /* [in] */ IUri* uri,
    /* [in] */ IBundle* extras)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::CancelSync(
    /* [in] */ IUri* uri)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetPackageName(
    /* [out] */ String* package)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::ResolveUserId(
    /* [in] */ IUri* uri,
    /* [out] */ Int32* id)
{
    LOGGERD(TAG, "CContentResolverNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CContentResolverNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CContentResolverNative::ToString()");
    return NOERROR;
}

}
}
}

