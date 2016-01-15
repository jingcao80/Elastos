
#include "elastos/droid/ext/frameworkdef.h"
#include "accounts/AccountAuthenticatorCache.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Content::Pm::EIID_IXmlSerializerAndParser;
using Elastos::Droid::Accounts::IAuthenticatorDescriptionHelper;
using Elastos::Droid::Accounts::CAuthenticatorDescriptionHelper;
using Elastos::Droid::Accounts::CAuthenticatorDescription;
using Elastos::Droid::Accounts::IAccountManager;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CAR_INTERFACE_IMPL(AccountAuthenticatorCache::MySerializer, IXmlSerializerAndParser);

ECode AccountAuthenticatorCache::MySerializer::WriteAsXml(
    /* [in] */ IInterface* item,
    /* [in] */ IXmlSerializer* serializer)
{
    AutoPtr<IAuthenticatorDescription> desc = (IAuthenticatorDescription*)item->Probe(
            Elastos::Droid::Accounts::EIID_IAuthenticatorDescription);
    String type;
    desc->GetType(&type);
    return serializer->WriteAttribute(String(NULL), String("type"), type);
}

ECode AccountAuthenticatorCache::MySerializer::CreateFromXml(
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    String s;
    parser->GetAttributeValue(String(NULL), String("type"), &s);
    AutoPtr<IAuthenticatorDescriptionHelper> helper;
    ASSERT_SUCCEEDED(CAuthenticatorDescriptionHelper::AcquireSingleton(
            (IAuthenticatorDescriptionHelper**)&helper));
    AutoPtr<IAuthenticatorDescription> desc;
    FAIL_RETURN(helper->NewKey(s, (IAuthenticatorDescription**)&desc));
    *obj = (IInterface*)desc.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}


const String AccountAuthenticatorCache::TAG("Account");
AutoPtr<AccountAuthenticatorCache::MySerializer> AccountAuthenticatorCache::sSerializer = new MySerializer();

AccountAuthenticatorCache::AccountAuthenticatorCache(
    /* [in] */ IContext* context)
    : RegisteredServicesCache(context, IAccountManager::ACTION_AUTHENTICATOR_INTENT,
            IAccountManager::AUTHENTICATOR_META_DATA_NAME,
            IAccountManager::AUTHENTICATOR_ATTRIBUTES_NAME,
            (IXmlSerializerAndParser*)sSerializer)
{}

PInterface AccountAuthenticatorCache::Probe(
    /* [in]  */ REIID riid)
{
    return NULL;
}

UInt32 AccountAuthenticatorCache::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 AccountAuthenticatorCache::Release()
{
    return ElRefBase::Release();
}

ECode AccountAuthenticatorCache::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

ECode AccountAuthenticatorCache::ParseServiceAttributes(
    /* [in] */ IResources* res,
    /* [in] */ const String& packageName,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IInterface** attributes)
{
    VALIDATE_NOT_NULL(attributes);
    *attributes = NULL;

    Int32 size = ArraySize(R::styleable::AccountAuthenticator);
    AutoPtr< ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::AccountAuthenticator, size);
    AutoPtr<ITypedArray> sa;
    ASSERT_SUCCEEDED(res->ObtainAttributes(attrs, layout, (ITypedArray**)&sa));
    // try {
    String accountType;
    ECode ec = sa->GetString(R::styleable::AccountAuthenticator_accountType, &accountType);
    if (FAILED(ec)) {
        sa->Recycle();
        return ec;
    }
    Int32 labelId, iconId, smallIconId, prefId;
    ec = sa->GetResourceId(
            R::styleable::AccountAuthenticator_label, 0, &labelId);
    if (FAILED(ec)) {
        sa->Recycle();
        return ec;
    }
    ec = sa->GetResourceId(
            R::styleable::AccountAuthenticator_icon, 0, &iconId);
    if (FAILED(ec)) {
        sa->Recycle();
        return ec;
    }
    ec = sa->GetResourceId(
            R::styleable::AccountAuthenticator_smallIcon, 0, &smallIconId);
    if (FAILED(ec)) {
        sa->Recycle();
        return ec;
    }
    ec = sa->GetResourceId(
            R::styleable::AccountAuthenticator_accountPreferences, 0, &prefId);
    if (FAILED(ec)) {
        sa->Recycle();
        return ec;
    }
    Boolean customTokens;
    sa->GetBoolean(
            R::styleable::AccountAuthenticator_customTokens, FALSE, &customTokens);
    if (FAILED(ec)) {
        sa->Recycle();
        return ec;
    }
    AutoPtr<ICharSequence> csq;
    CString::New(accountType, (ICharSequence**)&csq);
    if (TextUtils::IsEmpty(csq)) {
        return NOERROR;
    }
    AutoPtr<IAuthenticatorDescription> desc;
    ec = CAuthenticatorDescription::New(accountType, packageName, labelId, iconId,
            smallIconId, prefId, customTokens, (IAuthenticatorDescription**)&desc);
    *attributes = desc;
    REFCOUNT_ADD(*attributes);
    sa->Recycle();
    return ec;
    // } finally {
    //     sa.recycle();
    // }
}

AutoPtr<IRegisteredServicesCacheServiceInfo> AccountAuthenticatorCache::GetServiceInfo(
    /* [in] */ IAuthenticatorDescription* type,
    /* [in] */ Int32 userId)
{
    return RegisteredServicesCache::GetServiceInfo(type, userId);
}

AutoPtr< List<AutoPtr<IRegisteredServicesCacheServiceInfo> > > AccountAuthenticatorCache::GetAllServices(
    /* [in] */ Int32 userId)
{
    return RegisteredServicesCache::GetAllServices(userId);
}

void AccountAuthenticatorCache::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* fout,
    /* [in] */ const ArrayOf<String>& args,
    /* [in] */ Int32 userId)
{
    return RegisteredServicesCache::Dump(fd, fout,
            const_cast<ArrayOf<String>*>(&args), userId);
}

void AccountAuthenticatorCache::SetListener(
    /* [in] */ IRegisteredServicesCacheListener* listener,
    /* [in] */ IHandler* handler)
{
    return RegisteredServicesCache::SetListener(listener, handler);
}

void AccountAuthenticatorCache::InvalidateCache(
    /* [in] */ Int32 userId)
{
    return RegisteredServicesCache::InvalidateCache(userId);
}

}  //namespace Accounts
}  //namespace Server
}  //namespace Droid
}  //namespace Elastos
