
#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CNAME_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CNAME_H__

#include "_Elastos_Utility_Jar_CName.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CName)
    , public Object
    , public IName
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CName();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Boolean validateName);

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI Equals(
        /* [in] */ IInterface * pObject,
        /* [out] */ Boolean * pResult);

    CARAPI GetHashCode(
        /* [out] */ Int32 * pHashCode);

    CARAPI ToString(
        /* [out] */ String * pName);

public:
    /** The class path (a main attribute). */
    static const AutoPtr<IName> CLASS_PATH;// = new Name("Class-Path");

    /** The version of the manifest file (a main attribute). */
    static const AutoPtr<IName> MANIFEST_VERSION;// = new Name("Manifest-Version");

    /** The main class's name (for stand-alone applications). */
    static const AutoPtr<IName> MAIN_CLASS;// = new Name("Main-Class");

    /** Defines the signature version of the JAR file. */
    static const AutoPtr<IName> SIGNATURE_VERSION;// = new Name("Signature-Version");

    /** The {@code Content-Type} manifest attribute. */
    static const AutoPtr<IName> CONTENT_TYPE;// = new Name("Content-Type");

    /**
     * The {@code Sealed} manifest attribute which may have the value
     * {@code true} for sealed archives.
     */
    static const AutoPtr<IName> SEALED;// = new Name("Sealed");

    /**
     * The {@code Implementation-Title} attribute whose value is a string
     * that defines the title of the extension implementation.
     */
    static const AutoPtr<IName> IMPLEMENTATION_TITLE;// = new Name("Implementation-Title");

    /**
     * The {@code Implementation-Version} attribute defining the version of
     * the extension implementation.
     */
    static const AutoPtr<IName> IMPLEMENTATION_VERSION;// = new Name("Implementation-Version");

    /**
     * The {@code Implementation-Vendor} attribute defining the organization
     * that maintains the extension implementation.
     */
    static const AutoPtr<IName> IMPLEMENTATION_VENDOR;// = new Name("Implementation-Vendor");

    /**
     * The {@code Specification-Title} attribute defining the title of the
     * extension specification.
     */
    static const AutoPtr<IName> SPECIFICATION_TITLE;// = new Name("Specification-Title");

    /**
     * The {@code Specification-Version} attribute defining the version of
     * the extension specification.
     */
    static const AutoPtr<IName> SPECIFICATION_VERSION;// = new Name("Specification-Version");

    /**
     * The {@code Specification-Vendor} attribute defining the organization
     * that maintains the extension specification.
     */
    static const AutoPtr<IName> SPECIFICATION_VENDOR;// = new Name("Specification-Vendor");

    /**
     * The {@code Extension-List} attribute defining the extensions that are
     * needed by the applet.
     */
    static const AutoPtr<IName> EXTENSION_LIST;// = new Name("Extension-List");

    /**
     * The {@code Extension-Name} attribute which defines the unique name of
     * the extension.
     */
    static const AutoPtr<IName> EXTENSION_NAME;// = new Name("Extension-Name");

    /**
     * The {@code Extension-Installation} attribute.
     */
    static const AutoPtr<IName> EXTENSION_INSTALLATION;// = new Name("Extension-Installation");

    /**
     * The {@code Implementation-Vendor-Id} attribute specifies the vendor
     * of an extension implementation if the applet requires an
     * implementation from a specific vendor.
     */
    static const AutoPtr<IName> IMPLEMENTATION_VENDOR_ID;// = new Name("Implementation-Vendor-Id");

    /**
     * The {@code Implementation-URL} attribute specifying a URL that can be
     * used to obtain the most recent version of the extension if the
     * required version is not already installed.
     */
    static const AutoPtr<IName> IMPLEMENTATION_URL;// = new Name("Implementation-URL");

    static const AutoPtr<IName> NAME;// = new Name("Name");

private:
    String mName;
    Int32 mHashCode;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CNAME_H__
