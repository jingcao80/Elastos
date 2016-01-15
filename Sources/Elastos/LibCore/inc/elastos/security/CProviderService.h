
#ifndef __ELASTOS_SECURITY_CPROVIDERSERVICE_H__
#define __ELASTOS_SECURITY_CPROVIDERSERVICE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "coredef.h"
#include "_Elastos_Security_CProviderService.h"
#include "core/Mutex.h"
#include "core/Object.h"
#include "utility/etl/HashMap.h"

using Elastos::Core::Mutex;
using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Security {

CarClass(CProviderService)
    , public Object
    , public IProviderService
{
public:
    CARAPI constructor(
        /* [in] */ IProvider* provider,
        /* [in] */ const String& type,
        /* [in] */ const String& algorithm,
        /* [in] */ const String& className,
        /* [in] */ IList* aliases,
        /* [in] */ IMap* attributes);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Adds an alias.
     *
     * @param alias the alias to add
     */
    CARAPI AddAlias(
        /* [in] */ const String& alias);

    /**
     * Puts a new attribute mapping.
     *
     * @param name the attribute name.
     * @param value the attribute value.
     */
    CARAPI PutAttribute(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /**
     * Returns the type of this {@code Service}. For example {@code
     * KeyPairGenerator}.
     *
     * @return the type of this {@code Service}.
     */
    CARAPI GetType(
        /* [out] */ String* type);

    /**
     * Returns the name of the algorithm implemented by this {@code
     * Service}.
     *
     * @return the name of the algorithm implemented by this {@code
     *         Service}.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String* algorithm);

    /**
     * Returns the {@code Provider} this {@code Service} belongs to.
     *
     * @return the {@code Provider} this {@code Service} belongs to.
     */
    CARAPI GetProvider(
        /* [out] */ IProvider** provider);

    /**
     * Returns the name of the class implementing this {@code Service}.
     *
     * @return the name of the class implementing this {@code Service}.
     */
    CARAPI GetClassName(
        /* [out] */ String* className);

    CARAPI SetClassName(
        /* [in] */ const String& className);

    /**
     * Returns the value of the attribute with the specified {@code name}.
     *
     * @param name
     *            the name of the attribute.
     * @return the value of the attribute, or {@code null} if no attribute
     *         with the given name is set.
     * @throws NullPointerException
     *             if {@code name} is {@code null}.
     */
    CARAPI GetAttribute(
        /* [in] */ const String& name,
        /* [out] */ String* attrib);

    CARAPI GetAliases(
        /* [out] */ IList** aliases);

    /**
     * Creates and returns a new instance of the implementation described by
     * this {@code Service}.
     *
     * @param constructorParameter
     *            the parameter that is used by the constructor, or {@code
     *            null} if the implementation does not declare a constructor
     *            parameter.
     * @return a new instance of the implementation described by this
     *         {@code Service}.
     * @throws NoSuchAlgorithmException
     *             if the instance could not be constructed.
     * @throws InvalidParameterException
     *             if the implementation does not support the specified
     *             {@code constructorParameter}.
     */
    CARAPI NewInstance(
        /* [in] */ IInterface* constructorParameter,
        /* [out] */ IInterface** instance);

    /**
     * Indicates whether this {@code Service} supports the specified
     * constructor parameter.
     *
     * @param parameter
     *            the parameter to test.
     * @return {@code true} if this {@code Service} supports the specified
     *         constructor parameter, {@code false} otherwise.
     */
    CARAPI SupportsParameter(
        /* [in] */ IInterface* parameter,
        /* [out] */ Boolean* isSupported);

    CARAPI GetAttributes(
        /* [out] */ IMap** attributes);

    /**
     * Returns a string containing a concise, human-readable description of
     * this {@code Service}.
     *
     * @return a printable representation for this {@code Service}.
     */
    CARAPI ToString(
        /* [out] */ String* content);

private:
    /** Called to load a class if it's critical that the class exists. */
    static CARAPI LoadClassOrThrow(
        /* [in] */ const String& className,
        /* [in] */ const String& interfaceName,
        /* [in] */ IClassInfo* clsInfo,
        /* [out] */ IInterfaceInfo** itfInfo);

    CARAPI NewInstanceWithParameter(
        /* [in] */ IInterface* constructorParameter,
        /* [in] */ IInterfaceInfo* parameterItfInfo,
        /* [out] */ IInterface** instance);

    CARAPI NewInstanceNoParameter(
        /* [out] */ IInterface** instance);

    /**
     * Initialize the list of supported key classes and formats.
     */
    CARAPI_(void) EnsureSupportedKeysInitialized();

    /**
     * Check if an item is in the array. The array of supported key classes
     * and formats is usually just a length of 1, so a simple array is
     * faster than a Set.
     */
    static CARAPI_(Boolean) IsInArray(
        /* [in] */ const ArrayOf<String>& itemList,
        /* [in] */ const String& target);

    /**
     * Check if an item is in the array. The array of supported key classes
     * and formats is usually just a length of 1, so a simple array is
     * faster than a Set.
     */
    static CARAPI_(Boolean) IsInArray(
        /* [in] */ const ArrayOf<IInterfaceInfo*>& itemList,
        /* [in] */ IClassInfo* target);

    static CARAPI Initialize(
        /* [in] */ IProviderService* service);

    static CARAPI_(AutoPtr<IInterfaceInfo>) ToInterfaceInfo(
        /* [in] */ const String& keyClassName,
        /* [in] */ IClassInfo* keyClass);

private:
    /** Attribute name of supported key classes. */
    static const String ATTR_SUPPORTED_KEY_CLASSES;

    /** Attribute name of supported key formats. */
    static const String ATTR_SUPPORTED_KEY_FORMATS;

    /** Whether this type supports calls to {@link #supportsParameter(Object)}. */
    static HashMap<String, Boolean> sSupportsParameterTypes;

    /** Constructor argument classes for calls to {@link #newInstance(Object)}. */
    static HashMap<String, AutoPtr<IInterfaceInfo> > sConstructorParameterClasses;

    // The provider
    AutoPtr<IProvider> mProvider;

    // The type of this service
    String mType;

    // The algorithm name
    String mAlgorithm;

    // The class implementing this service
    String mClassName;

    // The aliases
    AutoPtr<IList> mAliases;

    // The attributes
    AutoPtr<IMap> mAttributes;

    // Service implementation
    AutoPtr<IClassInfo> mImplementation;

    // For newInstance() optimization
    String mLastClassName;

    /** Indicates whether supportedKeyClasses and supportedKeyFormats. */
    Boolean mSupportedKeysInitialized;

    /** List of classes that this service supports. */
    AutoPtr< ArrayOf<IInterfaceInfo*> > mKeyClasses;

    /** List of key formats this service supports. */
    AutoPtr< ArrayOf<String> > mKeyFormats;

    static Boolean sIsInitialized;
    static Mutex sLock;
};

} // namespace Security
} // namespace Elastos

#endif //__ELASTOS_SECURITY_CPROVIDERSERVICE_H__
