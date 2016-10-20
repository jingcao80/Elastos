
#include "coredef.h"
#include "CProviderService.h"
#include "core/ClassLoader.h"
#include "core/CString.h"
#include "core/StringBuilder.h"
#include "core/StringUtils.h"
#include "utility/CArrayList.h"
#include "utility/CCollections.h"
#include "utility/CHashMap.h"
#include "elastos/core/CPathClassLoader.h"
#include "utility/logging/Logger.h"

using Elastos::Core::ClassLoader;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IClassLoader;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CPathClassLoader;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Security {

const String CProviderService::ATTR_SUPPORTED_KEY_CLASSES("SupportedKeyClasses");
const String CProviderService::ATTR_SUPPORTED_KEY_FORMATS("SupportedKeyFormats");
HashMap<String, Boolean> CProviderService::sSupportsParameterTypes;
HashMap<String, AutoPtr<IInterfaceInfo> > CProviderService::sConstructorParameterClasses;
Boolean CProviderService::sIsInitialized = FALSE;
Mutex CProviderService::sLock;

ECode CProviderService::Initialize(
    /* [in] */ IProviderService* service)
{
    Mutex::AutoLock lock(sLock);

    if (sIsInitialized) return NOERROR;

    // Does not support parameter
    sSupportsParameterTypes[String("AlgorithmParameterGenerator")] = FALSE;
    sSupportsParameterTypes[String("AlgorithmParameters")] = FALSE;
    sSupportsParameterTypes[String("CertificateFactory")] = FALSE;
    sSupportsParameterTypes[String("CertPathBuilder")] = FALSE;
    sSupportsParameterTypes[String("CertPathValidator")] = FALSE;
    sSupportsParameterTypes[String("CertStore")] = FALSE;
    sSupportsParameterTypes[String("KeyFactory")] = FALSE;
    sSupportsParameterTypes[String("KeyGenerator")] = FALSE;
    sSupportsParameterTypes[String("KeyManagerFactory")] = FALSE;
    sSupportsParameterTypes[String("KeyPairGenerator")] = FALSE;
    sSupportsParameterTypes[String("KeyStore")] = FALSE;
    sSupportsParameterTypes[String("MessageDigest")] = FALSE;
    sSupportsParameterTypes[String("SecretKeyFactory")] = FALSE;
    sSupportsParameterTypes[String("SecureRandom")] = FALSE;
    sSupportsParameterTypes[String("SSLContext")] = FALSE;
    sSupportsParameterTypes[String("TrustManagerFactory")] = FALSE;

    // Supports parameter
    sSupportsParameterTypes[String("Cipher")] = TRUE;
    sSupportsParameterTypes[String("KeyAgreement")] = TRUE;
    sSupportsParameterTypes[String("Mac")] = TRUE;
    sSupportsParameterTypes[String("Signature")] = TRUE;

    //assert(0 && "TODO: java.security.cert.CertStoreParameters");
    // Types that take a parameter to newInstance
    AutoPtr<IClassInfo> clsInfo;
    CObject::ReflectClassInfo(service->Probe(EIID_IInterface), (IClassInfo**)&clsInfo);
    AutoPtr<IInterfaceInfo> itfInfo;
    ECode ec = LoadClassOrThrow(String("Elastos.Security.Cert.CLDAPCertStoreParameters"),//TODO
            String("Elastos.Security.Cert.ICertStoreParameters"),
            clsInfo, (IInterfaceInfo**)&itfInfo);
    if (FAILED(ec)) {
        Logger::E("leliang", "====== failed === ec:0x%x", ec);
        assert(SUCCEEDED(ec));
        return ec;
    }
    sConstructorParameterClasses[String("CertStore")] = itfInfo;

    // Types that do not take any kind of parameter
    sConstructorParameterClasses[String("AlgorithmParameterGenerator")] = NULL;
    sConstructorParameterClasses[String("AlgorithmParameters")] = NULL;
    sConstructorParameterClasses[String("CertificateFactory")] = NULL;
    sConstructorParameterClasses[String("CertPathBuilder")] = NULL;
    sConstructorParameterClasses[String("CertPathValidator")] = NULL;
    sConstructorParameterClasses[String("KeyFactory")] = NULL;
    sConstructorParameterClasses[String("KeyGenerator")] = NULL;
    sConstructorParameterClasses[String("KeyManagerFactory")] = NULL;
    sConstructorParameterClasses[String("KeyPairGenerator")] = NULL;
    sConstructorParameterClasses[String("KeyStore")] = NULL;
    sConstructorParameterClasses[String("MessageDigest")] = NULL;
    sConstructorParameterClasses[String("SecretKeyFactory")] = NULL;
    sConstructorParameterClasses[String("SecureRandom")] = NULL;
    sConstructorParameterClasses[String("SSLContext")] = NULL;
    sConstructorParameterClasses[String("TrustManagerFactory")] = NULL;
    sConstructorParameterClasses[String("Cipher")] = NULL;
    sConstructorParameterClasses[String("KeyAgreement")] = NULL;
    sConstructorParameterClasses[String("Mac")] = NULL;
    sConstructorParameterClasses[String("Signature")] = NULL;

    sIsInitialized = TRUE;

    return NOERROR;
}

ECode CProviderService::constructor(
    /* [in] */ IProvider* provider,
    /* [in] */ const String& type,
    /* [in] */ const String& algorithm,
    /* [in] */ const String& className,
    /* [in] */ IList* aliases,
    /* [in] */ IMap* attributes)
{
    FAIL_RETURN(Initialize(this));

    if (provider == NULL) {
        Logger::E("CProviderService", "provider == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (type.IsNull()) {
        Logger::E("CProviderService", "type == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (algorithm.IsNull()) {
        Logger::E("CProviderService", "algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (className.IsNull()) {
        Logger::E("CProviderService", "className == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mProvider = provider;
    mType = type;
    mAlgorithm = algorithm;
    mClassName = className;
    Int32 size;
    if ((aliases != NULL) && (aliases->GetSize(&size), size == 0)) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        AutoPtr<IList> emptyList;
        collections->GetEmptyList((IList**)&emptyList);
        mAliases = emptyList;
    }
    else {
        mAliases = aliases;
    }
    if ((attributes != NULL) && (attributes->GetSize(&size), size == 0)) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        AutoPtr<IMap> emptyMap;
        collections->GetEmptyMap((IMap**)&emptyMap);
        mAttributes = emptyMap;
    }
    else {
        mAttributes = attributes;
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(CProviderService, Object, IProviderService)

CAR_OBJECT_IMPL(CProviderService)

ECode CProviderService::LoadClassOrThrow(
    /* [in] */ const String& className,
    /* [in] */ const String& interfaceName,
    /* [in] */ IClassInfo* clsInfo,
    /* [out] */ IInterfaceInfo** itfInfo)
{
    //TODO AutoPtr<IClassLoader> loader = ClassLoader::GetClassLoader(clsInfo);
    //begin leliang
    String classPath("/system/lib/Elastos.CoreLibrary.eco");
    AutoPtr<IClassLoader> loader;
    CPathClassLoader::New(classPath, NULL, (IClassLoader**)&loader);
    //end leliang
    Logger::E("leliang", "CProviderService::LoadClassOrThrow loader:%p", loader.Get());
    AutoPtr<IClassInfo> theClsInfo;
    ECode ec = loader->LoadClass(className, (IClassInfo**)&theClsInfo);
    if (FAILED(ec)) {
        Logger::E("leliang", "CProviderService::LoadClassOrThrow ec:0x%x, className:%s", ec, className.string());
        return ec;
    }

    //FAIL_RETURN(loader->LoadClass(className, (IClassInfo**)&theClsInfo));
    return theClsInfo->GetInterfaceInfo(interfaceName, itfInfo);
}

ECode CProviderService::AddAlias(
    /* [in] */ const String& alias)
{
    Int32 size;
    if ((mAliases == NULL) || (mAliases->GetSize(&size), size == 0)) {
        mAliases = NULL;
        CArrayList::New((IList**)&mAliases);
    }
    AutoPtr<ICharSequence> strObj;
    CString::New(alias, (ICharSequence**)&strObj);
    mAliases->Add(strObj);
    return NOERROR;
}

ECode CProviderService::PutAttribute(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    Int32 size;
    if ((mAttributes == NULL) || (mAttributes->GetSize(&size), size == 0)) {
        mAttributes = NULL;
        CHashMap::New((IMap**)&mAttributes);
    }
    AutoPtr<ICharSequence> keyObj, valueObj;
    CString::New(name, (ICharSequence**)&keyObj);
    CString::New(value, (ICharSequence**)&valueObj);
    mAttributes->Put(keyObj, valueObj);
    return NOERROR;
}

ECode CProviderService::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CProviderService::GetAlgorithm(
    /* [out] */ String* algorithm)
{
    VALIDATE_NOT_NULL(algorithm);
    *algorithm = mAlgorithm;
    return NOERROR;
}

ECode CProviderService::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CProviderService::GetClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className);
    *className = mClassName;
    return NOERROR;
}

ECode CProviderService::GetAttribute(
    /* [in] */ const String& name,
    /* [out] */ String* attrib)
{
    VALIDATE_NOT_NULL(attrib);
    if (name.IsNull()) {
        Logger::E("CProviderService", "name == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mAttributes == NULL) {
        *attrib = NULL;
        return NOERROR;
    }
    AutoPtr<ICharSequence> keyObj;
    CString::New(name, (ICharSequence**)&keyObj);
    AutoPtr<IInterface> valueObj;
    mAttributes->Get(keyObj, (IInterface**)&valueObj);
    return ICharSequence::Probe(valueObj)->ToString(attrib);
}

ECode CProviderService::GetAliases(
    /* [out] */ IList** aliases)
{
    VALIDATE_NOT_NULL(aliases);
    if (mAliases == NULL){
        CArrayList::New((IList**)&mAliases);
    }
    *aliases = mAliases;
    REFCOUNT_ADD(*aliases);
    return NOERROR;
}

ECode CProviderService::NewInstance(
    /* [in] */ IInterface* constructorParameter,
    /* [out] */ IInterface** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;
    if (mImplementation == NULL || !mClassName.Equals(mLastClassName)) {
        AutoPtr<IClassInfo> clsInfo;
        CObject::ReflectClassInfo(mProvider, (IClassInfo**)&clsInfo);
        AutoPtr<IClassLoader> cl = ClassLoader::GetClassLoader(clsInfo);
        if (cl == NULL) {
            cl = ClassLoader::GetSystemClassLoader();
        }
        mImplementation = NULL;
        ECode ec = cl->LoadClass(mClassName, (IClassInfo**)&mImplementation);
        if (FAILED(ec)) {
            Logger::E("CProviderService", "%s %s implementation not found", mType.string(), mAlgorithm.string());
            Logger::E("CProviderService", "mClassName: %s", mClassName.string());
            assert(0);
            return E_NO_SUCH_ALGORITHM_EXCEPTION;
        }
        mLastClassName = mClassName;
    }

    // We don't know whether this takes a parameter or not.
    if (sConstructorParameterClasses.Find(mType) == sConstructorParameterClasses.End()) {
        if (constructorParameter == NULL) {
            return NewInstanceNoParameter(instance);
        }
        else {
            AutoPtr<IInterfaceInfo> paramItfInfo;
            CObject::ReflectInterfaceInfo(constructorParameter, (IInterfaceInfo**)&paramItfInfo);
            return NewInstanceWithParameter(constructorParameter, paramItfInfo, instance);
        }
    }

    // A known type, but it's not required to have a parameter even if a
    // class is specified.
    if (constructorParameter == NULL) {
        return NewInstanceNoParameter(instance);
    }

    // Make sure the provided constructor class is valid.
    AutoPtr<IInterfaceInfo> expectedInterface;
    HashMap<String, AutoPtr<IInterfaceInfo> >::Iterator it = sConstructorParameterClasses.Find(mType);
    if (it != sConstructorParameterClasses.End()) {
        expectedInterface = it->mSecond;
    }
    if (expectedInterface == NULL) {
        Logger::E("CProviderService", "Constructor parameter not supported for %s", mType.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IClassInfo> paramClsInfo;
    CObject::ReflectClassInfo(constructorParameter, (IClassInfo**)&paramClsInfo);
    Boolean isAssignableFrom;
    paramClsInfo->HasInterfaceInfo(expectedInterface, &isAssignableFrom);
    if (!isAssignableFrom) {
        String name1, name2;
        expectedInterface->GetName(&name1);
        paramClsInfo->GetName(&name2);
        Logger::E("CProviderService", "Expecting constructor parameter of type %s but was %s",
                name1.string(), name2.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NewInstanceWithParameter(constructorParameter, expectedInterface, instance);
}

ECode CProviderService::NewInstanceWithParameter(
    /* [in] */ IInterface* constructorParameter,
    /* [in] */ IInterfaceInfo* parameterItfInfo,
    /* [out] */ IInterface** instance)
{
    Int32 count;
    mImplementation->GetConstructorCount(&count);
    AutoPtr< ArrayOf<IConstructorInfo*> > consInfos = ArrayOf<IConstructorInfo*>::Alloc(count);
    mImplementation->GetAllConstructorInfos(consInfos);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IConstructorInfo> consInfo = (*consInfos)[i];
        Int32 paramCount;
        consInfo->GetParamCount(&paramCount);
        if (paramCount != 1) continue;
        AutoPtr<IParamInfo> paramInfo;
        consInfo->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
        AutoPtr<IDataTypeInfo> typeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&typeInfo);
        if (IInterfaceInfo::Probe(typeInfo) == parameterItfInfo) continue;

        AutoPtr<IArgumentList> args;
        consInfo->CreateArgumentList((IArgumentList**)&args);
        args->SetInputArgumentOfObjectPtr(0, constructorParameter);

        ECode ec = consInfo->CreateObject(args, instance);
        if (FAILED(ec)) goto ERROR;
        return NOERROR;
    }
ERROR:
    Logger::E("CProviderService", "%s  %s implementation not found",
            mType.string(), mAlgorithm.string());
    return E_NO_SUCH_ALGORITHM_EXCEPTION;
}

ECode CProviderService::NewInstanceNoParameter(
    /* [out] */ IInterface** instance)
{
    ECode ec = mImplementation->CreateObject(instance);
    if (FAILED(ec)) {
        Logger::E("CProviderService", "%s  %s implementation not found",
                mType.string(), mAlgorithm.string());
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    return NOERROR;
}

ECode CProviderService::SupportsParameter(
    /* [in] */ IInterface* parameter,
    /* [out] */ Boolean* isSupported)
{
    VALIDATE_NOT_NULL(isSupported);
    *isSupported = FALSE;
    HashMap<String, Boolean>::Iterator it = sSupportsParameterTypes.Find(mType);
    if (it == sSupportsParameterTypes.End()) {
        *isSupported = TRUE;
        return NOERROR;
    }
    Boolean supportsParameter = it->mSecond;
    if (!supportsParameter) {
        Logger::E("CProviderService", "Cannot use a parameter with %s", mType.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    /*
     * Only Key parameters are allowed, but allow null since there might
     * not be any listed classes or formats for this instance.
     */
    if (parameter != NULL && IKey::Probe(parameter) == NULL) {
        Logger::E("CProviderService", "Parameter should be of type Key");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    EnsureSupportedKeysInitialized();

    // No restriction specified by Provider registration.
    if (mKeyClasses == NULL && mKeyFormats == NULL) {
        *isSupported = TRUE;
        return NOERROR;
    }

    // Restriction specified by registration, so null is not acceptable.
    if (parameter == NULL) {
        *isSupported = FALSE;
        return NOERROR;
    }

    AutoPtr<IKey> keyParam = IKey::Probe(parameter);
    AutoPtr<IClassInfo> keyParamClsInfo;
    CObject::ReflectClassInfo(keyParam, (IClassInfo**)&keyParamClsInfo);
    if (mKeyClasses != NULL && IsInArray(*mKeyClasses, keyParamClsInfo)) {
        *isSupported = TRUE;
        return NOERROR;
    }
    String format;
    keyParam->GetFormat(&format);
    if (mKeyFormats != NULL && IsInArray(*mKeyFormats, format)) {
        *isSupported = TRUE;
        return NOERROR;
    }

    *isSupported = FALSE;
    return NOERROR;
}

void CProviderService::EnsureSupportedKeysInitialized()
{
    if (mSupportedKeysInitialized) {
        return;
    }

    String supportedClassesString;
    GetAttribute(ATTR_SUPPORTED_KEY_CLASSES, &supportedClassesString);
    if (!supportedClassesString.IsNull()) {
        AutoPtr< ArrayOf<String> > keyClassNames;
        StringUtils::Split(supportedClassesString, String("\\|"), (ArrayOf<String>**)&keyClassNames);
        AutoPtr<IArrayList> supportedClassList;
        CArrayList::New(keyClassNames->GetLength(), (IArrayList**)&supportedClassList);
        AutoPtr<IProvider> provider;
        GetProvider((IProvider**)&provider);
        AutoPtr<IClassInfo> clsInfo;
        CObject::ReflectClassInfo(provider, (IClassInfo**)&clsInfo);
        AutoPtr<IClassLoader> classLoader = ClassLoader::GetClassLoader(clsInfo);
        for (Int32 i = 0; i < keyClassNames->GetLength(); i++) {
            String keyClassName = (*keyClassNames)[i];
            AutoPtr<IClassInfo> keyClass;
            ECode ec = classLoader->LoadClass(keyClassName, (IClassInfo**)&keyClass);
            if (FAILED(ec)) continue;
            AutoPtr<IInterfaceInfo> keyItfInfo;
            keyClass->GetInterfaceInfo(String("LElastos/Security/IKey;"), (IInterfaceInfo**)&keyItfInfo);
            if (keyItfInfo != NULL) {
                supportedClassList->Add(ToInterfaceInfo(keyClassName, keyClass));
            }
        }
        supportedClassList->ToArray((ArrayOf<IInterface*>**)&mKeyClasses);
    }

    String supportedFormatString;
    GetAttribute(ATTR_SUPPORTED_KEY_FORMATS, &supportedFormatString);
    if (!supportedFormatString.IsNull()) {
        StringUtils::Split(supportedFormatString, String("\\|"), (ArrayOf<String>**)&mKeyFormats);
    }

    mSupportedKeysInitialized = TRUE;
}

Boolean CProviderService::IsInArray(
    /* [in] */ const ArrayOf<String>& itemList,
    /* [in] */ const String& target)
{
    if (target.IsNull()) {
        return FALSE;
    }
    for (Int32 i = 0; i < itemList.GetLength(); i++) {
        String item = itemList[i];
        if (target.Equals(item)) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CProviderService::IsInArray(
    /* [in] */ const ArrayOf<IInterfaceInfo*>& itemList,
    /* [in] */ IClassInfo* target)
{
    if (target == NULL) {
        return FALSE;
    }
    for (Int32 i = 0; i < itemList.GetLength(); i++) {
        AutoPtr<IInterfaceInfo> item = itemList[i];
        Boolean isAssignableFrom;
        target->HasInterfaceInfo(item, &isAssignableFrom);
        if (isAssignableFrom) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode CProviderService::ToString(
    /* [out] */ String* content)
{
    VALIDATE_NOT_NULL(content);

    StringBuilder sb;
    sb += "Provider ";
    String name;
    mProvider->GetName(&name);
    sb += name;
    sb += " Service ";
    sb += mType;
    sb += ".";
    sb += mAlgorithm;
    sb += " ";
    sb += mClassName;
    if (mAliases != NULL) {
        sb += "\nAliases ";
        String str;
        IObject::Probe(mAliases)->ToString(&str);
        sb += str;
    }
    if (mAttributes != NULL) {
        sb += "\nAttributes ";
        String str;
        IObject::Probe(mAttributes)->ToString(&str);
        sb += str;
    }

    *content = sb.ToString();
    return NOERROR;
}

ECode CProviderService::GetAttributes(
    /* [out] */ IMap** attributes)
{
    VALIDATE_NOT_NULL(attributes);
    *attributes = mAttributes;
    REFCOUNT_ADD(*attributes);
    return NOERROR;
}

ECode CProviderService::SetClassName(
    /* [in] */ const String& className)
{
    mClassName = className;
    return NOERROR;
}

AutoPtr<IInterfaceInfo> CProviderService::ToInterfaceInfo(
    /* [in] */ const String& keyClassName,
    /* [in] */ IClassInfo* keyClass)
{
    String keyInterfaceName;
    Int32 index = keyClassName.LastIndexOf(".");
    if (index <= 0) {
        keyInterfaceName = String("I") + keyClassName.Substring(1);
    }
    else {
        StringBuilder sb;
        sb += keyClassName.Substring(0, index);
        sb += "I";
        sb += keyClassName.Substring(index + 1);
        keyInterfaceName = sb.ToString();
    }
    AutoPtr<IInterfaceInfo> keyInterface;
    keyClass->GetInterfaceInfo(keyInterfaceName, (IInterfaceInfo**)&keyInterface);
    return keyInterface;
}

}
}
