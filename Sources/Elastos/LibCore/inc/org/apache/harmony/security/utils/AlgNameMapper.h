
#ifndef __ORG_APACHE_HARMONY_SECURITY_UTILS_ALGNAMEMAPPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_UTILS_ALGNAMEMAPPER_H__

#include "elastos/coredef.h"
#include "Elastos.CoreLibrary.Apache.h"
#include "Object.h"
#include "elastos/utility/etl/HashMap.h"

using Elastos::Core::Object;
using Elastos::Security::IProvider;
using Elastos::Utility::Etl::HashMap;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

/**
 * Provides Algorithm Name to OID and OID to Algorithm Name mappings. Some known
 * mappings are hardcoded. Tries to obtain additional mappings from installed
 * providers during initialization.
 */
class AlgNameMapper
{
public:
    /**
     * Returns OID for algName
     *
     * @param algName algorithm name to be mapped
     * @return OID as String
     */
    static CARAPI_(String) Map2OID(
        /* [in] */ const String& algName);

    /**
     * Returns algName for OID
     *
     * @param oid OID to be mapped
     * @return algorithm name
     */
    static CARAPI_(String) Map2AlgName(
        /* [in] */ const String& oid);

    /**
     * Returns Algorithm name for given algorithm alias
     *
     * @param algName - alias
     * @return algorithm name
     */
    static CARAPI_(String) GetStandardName(
        /* [in] */ const String& algName);

    /**
     * Checks if parameter represents OID
     *
     * @param alias alias to be checked
     * @return 'true' if parameter represents OID
     */
    static CARAPI_(Boolean) IsOID(
        /* [in] */ const String& alias);

    /**
     * Removes leading "OID." from oid String passed
     *
     * @param oid string that may contain leading "OID."
     * @return string passed without leading "OID."
     */
    static CARAPI_(String) Normalize(
        /* [in] */ const String& oid);

    static CARAPI_(void) SetSource(
        /* [in] */ IAlgNameMapperSource* source);

private:
    // No instances
    AlgNameMapper();

    static CARAPI_(void) CheckCacheVersion();

    // Searches given provider for mappings like
    // Alg.Alias.<service>.<OID-INTS-DOT-SEPARATED>=<alg-name>
    //  or
    // Alg.Alias.<service>.OID.<OID-INTS-DOT-SEPARATED>=<alg-name>
    // Puts mappings found into appropriate internal maps
    static CARAPI_(void) SelectEntries(
        /* [in] */ IProvider* p);

    static CARAPI_(Boolean) InitStatic();

private:
    static AutoPtr<IAlgNameMapperSource> sSource;

    static volatile Int32 sCacheVersion;

    // Will search OID mappings for these services
    static const String sServiceName[];

    // These mappings CAN NOT be overridden
    // by the ones from available providers
    // during maps initialization
    // (source: http://asn1.elibel.tm.fr):
    static String sKnownAlgMappings[19][2];

    // Maps alg name to OID
    static HashMap<String, String> sAlg2OidMap;
    // Maps OID to alg name
    static HashMap<String, String> sOid2AlgMap;
    // Maps aliases to alg names
    static HashMap<String, String> sAlgAliasesMap;

    static Object sLock;

    static Boolean sInit;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_UTILS_ALGNAMEMAPPER_H__
