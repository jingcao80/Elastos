
#ifndef __ORG_XML_SAX_HELPERS_CNAMESPACESUPPORT_H__
#define __ORG_XML_SAX_HELPERS_CNAMESPACESUPPORT_H__

#include "_Org_Xml_Sax_Helpers_CNamespaceSupport.h"
#include "Object.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::IEnumeration;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CarClass(CNamespaceSupport)
    , public Object
    , public INamespaceSupport
{
public:
    class Context;

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Reset();

    CARAPI PushContext();

    CARAPI PopContext();

    CARAPI DeclarePrefix(
        /* [in] */ const String& prefix,
        /* [in] */ const String& uri,
        /* [out] */ Boolean* isPrefixLegal);

    CARAPI ProcessName(
        /* [in] */ const String& qName,
        /* [in] */ ArrayOf<String>* parts,
        /* [in] */ Boolean isAttribute,
        /* [out, callee] */ ArrayOf<String>** name);

    CARAPI GetURI(
        /* [in] */ const String& prefix,
        /* [out] */ String* URI);

    CARAPI GetPrefixes(
        /* [out] */ IEnumeration** prefixes);

    CARAPI GetPrefix(
        /* [in] */ const String& uri,
        /* [out] */ String* prefix);

    CARAPI GetPrefixes(
        /* [in] */ const String& uri,
        /* [out] */ IEnumeration** prefixes);

    CARAPI GetDeclaredPrefixes(
        /* [out] */ IEnumeration** prefixes);

    CARAPI SetNamespaceDeclUris(
        /* [in] */ Boolean value);

    CARAPI IsNamespaceDeclUris(
        /* [out] */ Boolean* value);

    CARAPI constructor();

private:
    AutoPtr<ArrayOf<Context*> > mContexts;
    AutoPtr<Context> mCurrentContext;
    Int32 mContextPos;
    Boolean mNamespaceDeclUris;
    static AutoPtr<IEnumeration> EMPTY_ENUMERATION;

public:
    class Context
        : public Object
    {
    public:
        /**
         * Create the root-level Namespace context.
         */
        Context(
            /* [in] */ CNamespaceSupport* parent);

        virtual ~Context();

        /**
         * (Re)set the parent of this Namespace context.
         * The context must either have been freshly constructed,
         * or must have been cleared.
         *
         * @param context The parent Namespace context object.
         */
        CARAPI_(void) SetParent (
            /* [in] */ Context* parent);

        /**
         * Makes associated state become collectible,
         * invalidating this context.
         * {@link #setParent} must be called before
         * this context may be used again.
         */
        CARAPI_(void) Clear ();

        /**
         * Declare a Namespace prefix for this context.
         *
         * @param prefix The prefix to declare.
         * @param uri The associated Namespace URI.
         * @see org.xml.sax.helpers.NamespaceSupport#declarePrefix
         */
        CARAPI DeclarePrefix(
            /* [in] */ const String& prefix,
            /* [in] */ const String& uri);

        /**
         * Process an XML qualified name in this context.
         *
         * @param qName The XML qualified name.
         * @param isAttribute true if this is an attribute name.
         * @return An array of three strings containing the
         *         URI part (or empty string), the local part,
         *         and the raw name, all internalized, or null
         *         if there is an undeclared prefix.
         * @see org.xml.sax.helpers.NamespaceSupport#processName
         */
        CARAPI_(AutoPtr< ArrayOf<String> >) ProcessName (
            /* [in] */ const String& qName,
            /* [in] */ Boolean isAttribute);

        /**
         * Look up the URI associated with a prefix in this context.
         *
         * @param prefix The prefix to look up.
         * @return The associated Namespace URI, or null if none is
         *         declared.
         * @see org.xml.sax.helpers.NamespaceSupport#getURI
         */
        String GetURI (
            /* [in] */ const String& prefix);

        /**
         * Look up one of the prefixes associated with a URI in this context.
         *
         * <p>Since many prefixes may be mapped to the same URI,
         * the return value may be unreliable.</p>
         *
         * @param uri The URI to look up.
         * @return The associated prefix, or null if none is declared.
         * @see org.xml.sax.helpers.NamespaceSupport#getPrefix
         */
        String GetPrefix (
            /* [in] */ const String& uri);

        /**
         * Return an enumeration of prefixes declared in this context.
         *
         * @return An enumeration of prefixes (possibly empty).
         * @see org.xml.sax.helpers.NamespaceSupport#getDeclaredPrefixes
         */
        AutoPtr<IEnumeration> GetDeclaredPrefixes();

        /**
         * Return an enumeration of all prefixes currently in force.
         *
         * <p>The default prefix, if in force, is <em>not</em>
         * returned, and will have to be checked for separately.</p>
         *
         * @return An enumeration of prefixes (never empty).
         * @see org.xml.sax.helpers.NamespaceSupport#getPrefixes
         */
        AutoPtr<IEnumeration> GetPrefixes ();

        ////////////////////////////////////////////////////////////////
        // Internal methods.
        ////////////////////////////////////////////////////////////////

    private:
        /**
         * Copy on write for the internal tables in this context.
         *
         * <p>This class is optimized for the normal case where most
         * elements do not contain Namespace declarations.</p>
         */
        void CopyTables ();

        ////////////////////////////////////////////////////////////////
        // Protected state.
        ////////////////////////////////////////////////////////////////
    public:
        HashMap<String, String> mPrefixTable;
        HashMap<String, String> mUriTable;
        HashMap<String, AutoPtr< ArrayOf<String> > > mElementNameTable;
        HashMap<String, AutoPtr< ArrayOf<String> > > mAttributeNameTable;

        String mDefaultNS;
        Boolean mDeclsOK;

        ////////////////////////////////////////////////////////////////
        // Internal state.
        ////////////////////////////////////////////////////////////////

    private:
        AutoPtr<IWeakReference> mWeakParentHolder;
        AutoPtr<IArrayList> mDeclarations;
        Boolean mDeclSeen;
        Context* mParentContext;
    };
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org


DEFINE_CONVERSION_FOR(Org::Xml::Sax::Helpers::CNamespaceSupport::Context, IInterface)


#endif // __ORG_XML_SAX_HELPERS_CNAMESPACESUPPORT_H__
