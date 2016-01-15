
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CContentProviderHolder.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CContentProviderHolder, Object, IContentProviderHolder, IParcelable)

CAR_OBJECT_IMPL(CContentProviderHolder)

CContentProviderHolder::CContentProviderHolder()
    : mNoReleaseNeeded(FALSE)
{
}

ECode CContentProviderHolder::constructor()
{
    return NOERROR;
}

ECode CContentProviderHolder::constructor(
    /* [in] */ IProviderInfo* info)
{
    mInfo = info;
    return NOERROR;
}

ECode CContentProviderHolder::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mInfo.Get());
    dest->WriteInterfacePtr(mProvider.Get());
    dest->WriteInterfacePtr(mConnection.Get());
    dest->WriteBoolean(mNoReleaseNeeded);
    return NOERROR;
}

ECode CContentProviderHolder::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInterfacePtr((Handle32*)&mInfo);
    source->ReadInterfacePtr((Handle32*)&mProvider);
    // TODO:
//    provider = ContentProviderNative.asInterface(
//        source.readStrongBinder());
    source->ReadInterfacePtr((Handle32*)&mConnection);
    source->ReadBoolean(&mNoReleaseNeeded);
    return NOERROR;
}

ECode CContentProviderHolder::GetProviderInfo(
    /* [out] */ IProviderInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = mInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode CContentProviderHolder::SetProviderInfo(
    /* [in] */ IProviderInfo* info)
{
    mInfo = info;
    return NOERROR;
}

ECode CContentProviderHolder::GetContentProvider(
    /* [out] */ IIContentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CContentProviderHolder::SetContentProvider(
    /* [in] */ IIContentProvider* provider)
{
    mProvider = provider;
    return NOERROR;
}

ECode CContentProviderHolder::GetConnection(
    /* [out] */ IBinder** connection)
{
    VALIDATE_NOT_NULL(connection);
    *connection = mConnection;
    REFCOUNT_ADD(*connection);
    return NOERROR;
}

ECode CContentProviderHolder::SetConnection(
    /* [in] */ IBinder* connection)
{
    mConnection = connection;
    return NOERROR;
}

ECode CContentProviderHolder::GetNoReleaseNeeded(
    /* [out] */ Boolean* noReleaseNeeded)
{
    VALIDATE_NOT_NULL(noReleaseNeeded);
    *noReleaseNeeded = mNoReleaseNeeded;
    return NOERROR;
}

ECode CContentProviderHolder::SetNoReleaseNeeded(
    /* [in] */ Boolean noReleaseNeeded)
{
    mNoReleaseNeeded = noReleaseNeeded;
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

