#include "org/conscrypt/KeyManagerFactoryImpl.h"
#include "org/conscrypt/KeyManagerImpl.h"
#include <libcore/utility/EmptyArray.h>

using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
// TODO: Need CKeyStoreHelper
// using Elastos::Security::CKeyStoreHelper;
using Elastos::Security::IKeyStore;
using Elastos::Security::IKeyStoreHelper;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastosx::Net::Ssl::EIID_IKeyManagerFactorySpi;
using Libcore::Utility::EmptyArray;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL_2(KeyManagerFactoryImpl, Object,
        IKeyManagerFactoryImpl, IKeyManagerFactorySpi)

ECode KeyManagerFactoryImpl::EngineInit(
    /* [in] */ IKeyStore* ks,
    /* [in] */ ArrayOf<Char32>* password)
{
    if (ks != NULL) {
        mKeyStore = ks;
        if (password != NULL) {
            mPwd = ArrayOf<Char32>::Alloc(password->GetLength());
            for (Int32 i = 0; i < password->GetLength(); ++i) {
                (*mPwd)[i] = (*password)[i];
            }
        }
        else {
            mPwd = EmptyArray::CHAR32;
        }
    }
    else {
        AutoPtr<IKeyStoreHelper> ksHelper;
// TODO: Need CKeyStoreHelper
        // CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&ksHelper);
        String type;
        ksHelper->GetDefaultType(&type);
        ksHelper->GetInstance(type, (IKeyStore**)&mKeyStore);

        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        String keyStoreName;
        system->GetProperty(String("javax.net.ssl.keyStore"), &keyStoreName);
        String keyStorePwd;
        if (keyStoreName.IsNull() || keyStoreName.EqualsIgnoreCase("NONE")
                || keyStoreName.IsEmpty()) {
            // try {
            mKeyStore->Load(NULL, NULL);
            // } catch (IOException e) {
            //     throw new KeyStoreException(e);
            // } catch (CertificateException e) {
            //     throw new KeyStoreException(e);
            // }
        } else {
            system->GetProperty(String("javax.net.ssl.keyStorePassword"),
                    &keyStorePwd);

            if (keyStorePwd == NULL) {
                mPwd = EmptyArray::CHAR32;
            }
            else {
                mPwd = keyStorePwd.GetChars();
            }
            // try {
            AutoPtr<IFile> file;
            CFile::New(keyStoreName, (IFile**)&file);
            AutoPtr<IFileInputStream> fis;
            CFileInputStream::New(file, (IFileInputStream**)&fis);
            mKeyStore->Load(IInputStream::Probe(fis), mPwd);
            // } catch (FileNotFoundException e) {
            //     throw new KeyStoreException(e);
            // } catch (IOException e) {
            //     throw new KeyStoreException(e);
            // } catch (CertificateException e) {
            //     throw new KeyStoreException(e);
            // }
        }
    }
    return NOERROR;
}

ECode KeyManagerFactoryImpl::EngineInit(
    /* [in] */ IManagerFactoryParameters* spec)
{
    // throw new InvalidAlgorithmParameterException(
    //         "ManagerFactoryParameters not supported");
    return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
}

ECode KeyManagerFactoryImpl::EngineGetKeyManagers(
    /* [out, callee] */ ArrayOf<IKeyManager*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mKeyStore == NULL) {
        // throw new IllegalStateException("KeyManagerFactory is not initialized");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<KeyManagerImpl> kmi = new KeyManagerImpl();
    kmi->constructor(mKeyStore, mPwd);
    AutoPtr<ArrayOf<IKeyManager*> > array = ArrayOf<IKeyManager*>::Alloc(1);
    array->Set(0, IKeyManager::Probe(kmi));
    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
