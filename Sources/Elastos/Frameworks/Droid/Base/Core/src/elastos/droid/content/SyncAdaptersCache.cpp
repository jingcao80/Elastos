
#include "elastos/droid/content/SyncAdaptersCache.h"
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/R.h"
#include "elastos/droid/content/CSyncAdapterType.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::CSyncAdapterType;
using Elastos::Droid::Content::Pm::IXmlSerializerAndParser;
using Elastos::Droid::Content::Pm::EIID_IXmlSerializerAndParser;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {

const String SyncAdaptersCache::TAG("Account");
const String SyncAdaptersCache::SERVICE_INTERFACE("android.content.SyncAdapter");
const String SyncAdaptersCache::SERVICE_META_DATA("android.content.SyncAdapter");
const String SyncAdaptersCache::ATTRIBUTES_NAME("sync-adapter");
const AutoPtr<SyncAdaptersCache::MySerializer> SyncAdaptersCache::sSerializer = new SyncAdaptersCache::MySerializer();

CAR_INTERFACE_IMPL(SyncAdaptersCache::MySerializer, Object, IXmlSerializerAndParser)

ECode SyncAdaptersCache::MySerializer::WriteAsXml(
    /* [in] */ IInterface* item,
    /* [in] */ IXmlSerializer* serializer)
{
    AutoPtr<ISyncAdapterType> adapterType = (ISyncAdapterType*) item;
    String authority;
    String accountType;
    FAIL_RETURN(adapterType->GetAuthority(&authority))
    FAIL_RETURN(adapterType->GetAccountType(&accountType))
    FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("authority"), authority))
    FAIL_RETURN(serializer->WriteAttribute(String(NULL), String("accountType"), accountType))
    return NOERROR;
}

ECode SyncAdaptersCache::MySerializer::CreateFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    String authority, accountType;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("authority"), &authority))
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("accountType"), &accountType))
    AutoPtr<ISyncAdapterType> adapterType = CSyncAdapterType::NewKey(authority, accountType);
    *obj = adapterType;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

CAR_INTERFACE_IMPL(SyncAdaptersCache, RegisteredServicesCache, ISyncAdaptersCache)

SyncAdaptersCache::SyncAdaptersCache()
{}

SyncAdaptersCache::~SyncAdaptersCache()
{}

ECode SyncAdaptersCache::constructor(
    /* [in] */ IContext *context)
{
    String nullStr;
    return RegisteredServicesCache::constructor(context, nullStr, nullStr,
        nullStr, sSerializer.Get());
}

ECode SyncAdaptersCache::ParseServiceAttributes(
    /* [in] */ IResources* res,
    /* [in] */ const String& packageName,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IInterface** syncAdapterType)
{
    VALIDATE_NOT_NULL(syncAdapterType)
    *syncAdapterType = NULL;

    AutoPtr<ArrayOf<Int32> > layout = TO_ATTRS_ARRAYOF(R::styleable::SyncAdapter);

    AutoPtr<ITypedArray> sa;
    FAIL_RETURN(res->ObtainAttributes(attrs, layout, (ITypedArray**)&sa))

    String authority;
    ECode ec = NOERROR;
    String accountType;
    Boolean userVisible = FALSE;
    Boolean supportsUploading = FALSE;
    Boolean isAlwaysSyncable = FALSE;
    Boolean allowParallelSyncs = FALSE;
    String settingsActivity;
    AutoPtr<ISyncAdapterType> adapterType;
    ec = sa->GetString(R::styleable::SyncAdapter_contentAuthority, &authority);
    if (FAILED(ec)) goto EXIT;
    ec = sa->GetString(R::styleable::SyncAdapter_accountType, &accountType);
    if (FAILED(ec)) goto EXIT;

    if (authority.IsNull() || accountType.IsNull()) {
        *syncAdapterType = NULL;
        return NOERROR;
    }

    ec = sa->GetBoolean(R::styleable::SyncAdapter_userVisible, TRUE, &userVisible);
    if (FAILED(ec)) goto EXIT;
    ec = sa->GetBoolean(R::styleable::SyncAdapter_supportsUploading, TRUE, &supportsUploading);
    if (FAILED(ec)) goto EXIT;
    ec = sa->GetBoolean(R::styleable::SyncAdapter_isAlwaysSyncable, FALSE, &isAlwaysSyncable);
    if (FAILED(ec)) goto EXIT;
    ec = sa->GetBoolean(R::styleable::SyncAdapter_allowParallelSyncs, FALSE, &allowParallelSyncs);
    if (FAILED(ec)) goto EXIT;
    ec = sa->GetString(R::styleable::SyncAdapter_settingsActivity, &settingsActivity);
    if (FAILED(ec)) goto EXIT;
    ec = CSyncAdapterType::New((ISyncAdapterType**)&adapterType);
    ((CSyncAdapterType*)adapterType.Get())->constructor(authority, accountType, userVisible, supportsUploading,
            isAlwaysSyncable, allowParallelSyncs, settingsActivity);
    if (FAILED(ec)) goto EXIT;

    *syncAdapterType = adapterType.Get();
    REFCOUNT_ADD(*syncAdapterType);

EXIT:
    FAIL_RETURN(sa->Recycle())
    return ec;
}

}
}
}

