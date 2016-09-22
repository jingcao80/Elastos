
#include "KeyStoreBuilder.h"
#include "CIoUtils.h"
#include "CFileInputStream.h"
#include "KeyStore.h"
#include "KeyStoreSpi.h"
#include "CKeyStoreHelper.h"

using Elastos::IO::IIoUtils;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CIoUtils;

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(KeyStoreBuilder, Object, IKeyStoreBuilder)
KeyStoreBuilder::KeyStoreBuilder()
{}

ECode KeyStoreBuilder::NewInstance(
    /* [in] */ IKeyStore* keyStore,
    /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
    /* [out] */ IKeyStoreBuilder** instance)
{
    VALIDATE_NOT_NULL(instance)
    if (keyStore == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (protectionParameter == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!reinterpret_cast<KeyStore*>(keyStore->Probe(EIID_KeyStore))->IsInit()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    (*instance) = new KeyStoreBuilderImpl(keyStore, protectionParameter, NULL, String(), NULL);
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode KeyStoreBuilder::NewInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider* provider,
    /* [in] */ IFile* file,
    /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
    /* [out] */ IKeyStoreBuilder** instance)
{
    VALIDATE_NOT_NULL(instance)
    // check null parameters
    if (type.IsNull() || protectionParameter == NULL || file == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // protection parameter should be PasswordProtection or
    // CallbackHandlerProtection
    if (!IKeyStorePasswordProtection::Probe(protectionParameter)
        && !IKeyStoreCallbackHandlerProtection::Probe(protectionParameter)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // check file parameter
    Boolean tmpTag; file->Exists(&tmpTag);
    if (!tmpTag) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    file->IsFile(&tmpTag);
    if (!tmpTag) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // create new instance
    (*instance) = new KeyStoreBuilderImpl(NULL, protectionParameter, file, type, provider);
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

ECode KeyStoreBuilder::NewInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider* provider,
    /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
    /* [out] */ IKeyStoreBuilder** instance)
{
    VALIDATE_NOT_NULL(instance)
    if (type.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (protectionParameter == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    (*instance) = new KeyStoreBuilderImpl(NULL, protectionParameter, NULL, type, provider);
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

KeyStoreBuilder::KeyStoreBuilderImpl::KeyStoreBuilderImpl(
    /* [in] */ IKeyStore *ks,
    /* [in] */ IKeyStoreProtectionParameter *pp,
    /* [in] */ IFile *file,
    /* [in] */ const String& type,
    /* [in] */ IProvider *provider)
    : mKeyStore(ks)
    , mProtParameter(pp)
    , mFileForLoad(file)
    , mTypeForKeyStore(type)
    , mProviderForKeyStore(provider)
    , mIsGetKeyStore(FALSE)
    , mEc(NOERROR)
{}

ECode KeyStoreBuilder::KeyStoreBuilderImpl::GetKeyStore(
        /* [out] */ IKeyStore** keyStore)
{
    // If KeyStore was created but in final block some exception was
    // thrown
    // then it was stored in lastException variable and will be
    // thrown
    // all subsequent calls of this method.
    AutoLock lock(this);
    VALIDATE_NOT_NULL(keyStore)
    FAIL_RETURN(mEc)
    if (mKeyStore) {
        mIsGetKeyStore = TRUE;
        *keyStore = mKeyStore;
        REFCOUNT_ADD(*keyStore)
        return NOERROR;
    }
    // get KeyStore instance using type or type and provider
    AutoPtr<IKeyStore> ks;
    AutoPtr<IKeyStoreHelper> help;
    CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&help);
    if (mProviderForKeyStore == NULL) {
        help->GetInstance(mTypeForKeyStore, (IKeyStore**)&ks);
    }
    else {
        help->GetInstanceEx2(mTypeForKeyStore, mProviderForKeyStore, (IKeyStore**)&ks);
    }
    // protection parameter should be PasswordProtection
    // or CallbackHandlerProtection
    AutoPtr<ArrayOf<Char32> > passwd;
    if (IKeyStorePasswordProtection::Probe(mProtParameter.Get())) {
        IKeyStorePasswordProtection::Probe(mProtParameter.Get())->GetPassword((ArrayOf<Char32>**)&passwd);
    }
    else if (IKeyStoreCallbackHandlerProtection::Probe(mProtParameter.Get())) {
        KeyStoreSpi::GetPasswordFromCallBack(mProtParameter, (ArrayOf<Char32>**)&passwd);
    }
    else {
        return E_KEY_STORE_EXCEPTION;
    }
    // load KeyStore from file
    if (mFileForLoad != NULL) {
        AutoPtr<IFileInputStream> fis;
        CFileInputStream::New(mFileForLoad, (IFileInputStream**)&fis);
        AutoPtr<IIoUtils> iou;
        FAIL_GOTO(mEc = ks->Load(fis, passwd), ERROR_PROCESS)
        CIoUtils::AcquireSingleton((IIoUtils**)&iou);
ERROR_PROCESS:
        iou->CloseQuietly(fis.Get());
        FAIL_RETURN(mEc)
    }
    else {
        AutoPtr<IKeyStoreLoadStoreParameter> lsp = new KeyStoreTmpLSParameter(mProtParameter);
        FAIL_RETURN(ks->LoadEx(lsp))
    }
    mIsGetKeyStore = TRUE;
    *keyStore = ks;
    REFCOUNT_ADD(*keyStore)
    return NOERROR;

}

ECode KeyStoreBuilder::KeyStoreBuilderImpl::GetProtectionParameter(
/* [in] */ const String& alias,
/* [out] */ IKeyStoreProtectionParameter** protectionParameter)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(protectionParameter)
    if (alias.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!mIsGetKeyStore) {
        return E_NULL_POINTER_EXCEPTION;
    }
    *protectionParameter = mProtParameter;
    REFCOUNT_ADD(*protectionParameter)
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyStoreBuilder::KeyStoreTmpLSParameter, Object, IKeyStoreLoadStoreParameter)
KeyStoreBuilder::KeyStoreTmpLSParameter::KeyStoreTmpLSParameter(
    /* [in] */ IKeyStoreProtectionParameter *protPar)
    : mProtPar(protPar)
{}

ECode KeyStoreBuilder::KeyStoreTmpLSParameter::GetProtectionParameter(
    /* [out] */ IKeyStoreProtectionParameter** protectionParameter)
{
    VALIDATE_NOT_NULL(protectionParameter)
    *protectionParameter = mProtPar;
    REFCOUNT_ADD(*protectionParameter)
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
