
#include "CContentResolverNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CContentResolverNative::TAG("CContentResolverNative");

CContentResolverNative::~CContentResolverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CContentResolverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CContentResolverNative::ReleaseProvider(
    /* [in] */ IIContentProvider* icp,
    /* [out] */ Boolean* isRelease)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::ReleaseUnstableProvider(
    /* [in] */ IIContentProvider* icp,
    /* [out] */ Boolean* isRelease)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::UnstableProviderDied(
    /* [in] */ IIContentProvider* icp)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetType(
    /* [in] */ IUri* uri,
    /* [out] */ String* type)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetStreamTypes(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeTypeFilter,
    /* [out, callee] */ ArrayOf<String>** streamTypes)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
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
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::QueryEx(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& sortOrder,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenInputStream(
    /* [in] */ IUri* uri,
    /* [out] */ IInputStream** inStream)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenOutputStream(
    /* [in] */ IUri* uri,
    /* [out] */ IOutputStream** outStream)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenOutputStreamEx(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IOutputStream** outStream)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IParcelFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mode,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::OpenTypedAssetFileDescriptor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& mimeType,
    /* [in] */ IBundle* opts,
    /* [out] */ IAssetFileDescriptor** fileDescriptor)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::GetResourceId(
    /* [in] */ IUri* uri,
    /* [out] */ IContentResolverOpenResourceIdResult** resourceIdResult)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Insert(
    /* [in] */ IUri* uri,
    /* [in] */ IContentValues* values,
    /* [out] */ IUri** insertedUri)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::ApplyBatch(
    /* [in] */ const String& authority,
    /* [in] */ IObjectContainer* operations,
    /* [out, callee] */ ArrayOf<Elastos::Droid::Content::IContentProviderResult *>** providerResults)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::BulkInsert(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<IContentValues *>* values,
    /* [out] */ Int32* number)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::Delete(
    /* [in] */ IUri* uri,
    /* [in] */ const String& where,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [out] */ Int32* rowsAffected)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
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
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
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
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireExistingProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireProviderEx(
    /* [in] */ const String& name,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableProvider(
    /* [in] */ IUri* uri,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableProviderEx(
    /* [in] */ const String& name,
    /* [out] */ IIContentProvider** contentProvider)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireContentProviderClient(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireContentProviderClientEx(
    /* [in] */ const String& name,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableContentProviderClient(
    /* [in] */ IUri* uri,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::AcquireUnstableContentProviderClientEx(
    /* [in] */ const String& name,
    /* [out] */ IContentProviderClient** client)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendents,
    /* [in] */ IContentObserver* observer)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::RegisterContentObserverEx(
    /* [in] */ IUri* uri,
    /* [in] */ Boolean notifyForDescendents,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Int32 userHandle)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::NotifyChange(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::NotifyChangeEx(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Boolean syncToNetwork)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::NotifyChangeEx2(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer,
    /* [in] */ Boolean syncToNetwork,
    /* [in] */ Int32 userHandle)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::StartSync(
    /* [in] */ IUri* uri,
    /* [in] */ IBundle* extras)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CContentResolverNative::CancelSync(
    /* [in] */ IUri* uri)
{
    LOGGERD(TAG, String("CContentResolverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

}
}
}

