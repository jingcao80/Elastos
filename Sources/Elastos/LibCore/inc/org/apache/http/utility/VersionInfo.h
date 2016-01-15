
#ifndef __ORG_APACHE_HTTP_UTILITY_VERSIONINFO_H_
#define __ORG_APACHE_HTTP_UTILITY_VERSIONINFO_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::IClassLoader;
using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

/**
 * A request interceptor that adds a User-Agent header.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
class VersionInfo
    : public Object
    , public IVersionInfo
{
public:
    CAR_INTERFACE_DECL()

    virtual ~VersionInfo() {}

    /**
     * Obtains the package name.
     * The package name identifies the module or informal unit.
     *
     * @return  the package name, never <code>null</code>
     */
    CARAPI GetPackage(
        /* [out] */ String* name);

    /**
     * Obtains the name of the versioned module or informal unit.
     * This data is read from the version information for the package.
     *
     * @return  the module name, never <code>null</code>
     */
    CARAPI GetModule(
        /* [out] */ String* name);

    /**
     * Obtains the release of the versioned module or informal unit.
     * This data is read from the version information for the package.
     *
     * @return  the release version, never <code>null</code>
     */
    CARAPI GetRelease(
        /* [out] */ String* name);

    /**
     * Obtains the timestamp of the versioned module or informal unit.
     * This data is read from the version information for the package.
     *
     * @return  the timestamp, never <code>null</code>
     */
    CARAPI GetTimestamp(
        /* [out] */ String* name);

    /**
     * Obtains the classloader used to read the version information.
     * This is just the <code>toString</code> output of the classloader,
     * since the version information should not keep a reference to
     * the classloader itself. That could prevent garbage collection.
     *
     * @return  the classloader description, never <code>null</code>
     */
    CARAPI GetClassloader(
        /* [out] */ String* name);

    /**
     * Provides the version information in human-readable format.
     *
     * @return  a string holding this version information
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Loads version information for a list of packages.
     *
     * @param pckgs     the packages for which to load version info
     * @param clsldr    the classloader to load from, or
     *                  <code>null</code> for the thread context classloader
     *
     * @return  the version information for all packages found,
     *          never <code>null</code>
     */
    static CARAPI LoadVersionInfo(
        /* [in] */ ArrayOf<String>* pckgs,
        /* [in] */ IClassLoader* clsldr,
        /* [out] */ ArrayOf<IVersionInfo*>** infos);

    /**
     * Loads version information for a package.
     *
     * @param pckg      the package for which to load version information,
     *                  for example "org.apache.http".
     *                  The package name should NOT end with a dot.
     * @param clsldr    the classloader to load from, or
     *                  <code>null</code> for the thread context classloader
     *
     * @return  the version information for the argument package, or
     *          <code>null</code> if not available
     */
    static CARAPI LoadVersionInfo(
        /* [in] */ const String& pckg,
        /* [in] */ IClassLoader* clsldr,
        /* [out] */ IVersionInfo** info);

protected:
    VersionInfo(
        /* [in] */ const String& pckg,
        /* [in] */ const String& module,
        /* [in] */ const String& release,
        /* [in] */ const String& time,
        /* [in] */ const String& clsldr);

    /**
     * Instantiates version information.
     *
     * @param pckg      the package
     * @param module    the module, or <code>null</code>
     * @param release   the release, or <code>null</code>
     * @param time      the build time, or <code>null</code>
     * @param clsldr    the class loader, or <code>null</code>
     */
    CARAPI Init(
        /* [in] */ const String& pckg,
        /* [in] */ const String& module,
        /* [in] */ const String& release,
        /* [in] */ const String& time,
        /* [in] */ const String& clsldr);

    /**
     * Instantiates version information from properties.
     *
     * @param pckg      the package for the version information
     * @param info      the map from string keys to string values,
     *                  for example {@link java.util.Properties}
     * @param clsldr    the classloader, or <code>null</code>
     *
     * @return  the version information
     */
    static CARAPI FromMap(
        /* [in] */ const String& pckg,
        /* [in] */ IMap* info,
        /* [in] */ IClassLoader* clsldr,
        /* [out] */ IVersionInfo** versionInfo);

private:
    /** The package that contains the version information. */
    String mInfoPackage;

    /** The module from the version info. */
    String mInfoModule;

    /** The release from the version info. */
    String mInfoRelease;

    /** The timestamp from the version info. */
    String mInfoTimestamp;

    /** The classloader from which the version info was obtained. */
    String mInfoClassloader;

};

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_UTILITY_VERSIONINFO_H_
