#ifndef __ELASTOS_UTILITY_RESOURCEBUNDLE_H__
#define __ELASTOS_UTILITY_RESOURCEBUNDLE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::IClassLoader;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Utility {

/**
 * {@code ResourceBundle} is an abstract class which is the superclass of classes which
 * provide {@code Locale}-specific resources. A bundle contains a number of named
 * resources, where the names are {@code Strings}. A bundle may have a parent bundle,
 * and when a resource is not found in a bundle, the parent bundle is searched for
 * the resource. If the fallback mechanism reaches the base bundle and still
 * can't find the resource it throws a {@code MissingResourceException}.
 *
 * <ul>
 * <li>All bundles for the same group of resources share a common base bundle.
 * This base bundle acts as the root and is the last fallback in case none of
 * its children was able to respond to a request.</li>
 * <li>The first level contains changes between different languages. Only the
 * differences between a language and the language of the base bundle need to be
 * handled by a language-specific {@code ResourceBundle}.</li>
 * <li>The second level contains changes between different countries that use
 * the same language. Only the differences between a country and the country of
 * the language bundle need to be handled by a country-specific {@code ResourceBundle}.
 * </li>
 * <li>The third level contains changes that don't have a geographic reason
 * (e.g. changes that where made at some point in time like {@code PREEURO} where the
 * currency of come countries changed. The country bundle would return the
 * current currency (Euro) and the {@code PREEURO} variant bundle would return the old
 * currency (e.g. DM for Germany).</li>
 * </ul>
 *
 * <strong>Examples</strong>
 * <ul>
 * <li>BaseName (base bundle)
 * <li>BaseName_de (german language bundle)
 * <li>BaseName_fr (french language bundle)
 * <li>BaseName_de_DE (bundle with Germany specific resources in german)
 * <li>BaseName_de_CH (bundle with Switzerland specific resources in german)
 * <li>BaseName_fr_CH (bundle with Switzerland specific resources in french)
 * <li>BaseName_de_DE_PREEURO (bundle with Germany specific resources in german of
 * the time before the Euro)
 * <li>BaseName_fr_FR_PREEURO (bundle with France specific resources in french of
 * the time before the Euro)
 * </ul>
 *
 * It's also possible to create variants for languages or countries. This can be
 * done by just skipping the country or language abbreviation:
 * BaseName_us__POSIX or BaseName__DE_PREEURO. But it's not allowed to
 * circumvent both language and country: BaseName___VARIANT is illegal.
 *
 * @see Properties
 * @see PropertyResourceBundle
 * @see ListResourceBundle
 * @since 1.1
 */
class ResourceBundle
    : public Object
    , public IResourceBundle
{
public:
    /**
     * ResourceBundle.Control is a static utility class defines ResourceBundle
     * load access methods, its default access order is as the same as before.
     * However users can implement their own control.
     *
     * @since 1.6
     */
    class Control
        : public IResourceBundleControl
        , public Object
    {
    protected:
        /**
         * default constructor
         *
         */
        Control();

    public:

        CAR_INTERFACE_DECL()
        /**
         * Returns a list of candidate locales according to {@code baseName} in
         * {@code locale}.
         */
        CARAPI GetCandidateLocales(
            /* [in] */ const String& baseName,
            /* [in] */ ILocale* locale,
            /* [out] */ IList** obj);

        /**
         * Returns a list of strings of formats according to {@code baseName}.
         */
        CARAPI GetFormats(
            /* [in] */ const String& baseName,
            /* [out] */ IList** obj);

        /**
         * Returns the fallback locale for {@code baseName} in {@code locale}.
         */
        CARAPI GetFallbackLocale(
            /* [in] */ const String& baseName,
            /* [in] */ ILocale* locale,
            /* [out] */ ILocale** loc);

        /**
         * Returns a new ResourceBundle.
         *
         * @param baseName
         *            the base name to use
         * @param locale
         *            the given locale
         * @param format
         *            the format, default is "java.class" or "java.properties"
         * @param loader
         *            the classloader to use
         * @param reload
         *            whether to reload the resource
         * @return a new ResourceBundle according to the give parameters
         * @throws IllegalAccessException
         *             if we can not access resources
         * @throws InstantiationException
         *             if we can not instantiate a resource class
         * @throws IOException
         *             if other I/O exception happens
         */
        CARAPI NewBundle(
            /* [in] */ const String& baseName,
            /* [in] */ ILocale* locale,
            /* [in] */ const String& format,
            /* [in] */ IClassLoader* loader,
            /* [in] */ Boolean reload,
            /* [out] */ IResourceBundle** resbun);

        /**
         * Returns the time to live of the ResourceBundle {@code baseName} in {@code locale},
         * default is TTL_NO_EXPIRATION_CONTROL.
         */
        CARAPI GetTimeToLive(
            /* [in] */ const String& baseName,
            /* [in] */ ILocale* locale,
            /* [out] */ Int64* value);

        /**
         * Returns true if the ResourceBundle needs to reload.
         *
         * @param baseName
         *            the base name of the ResourceBundle
         * @param locale
         *            the locale of the ResourceBundle
         * @param format
         *            the format to load
         * @param loader
         *            the ClassLoader to load resource
         * @param bundle
         *            the ResourceBundle
         * @param loadTime
         *            the expired time
         * @return if the ResourceBundle needs to reload
         */
        CARAPI NeedsReload(
            /* [in] */ const String& baseName,
            /* [in] */ ILocale* locale,
            /* [in] */ const String& format,
            /* [in] */ IClassLoader* loader,
            /* [in] */ IResourceBundle* bundle,
            /* [in] */ Int64 loadTime,
            /* [out] */ Boolean* value);

        /**
         * a utility method to answer the name of a resource bundle according to
         * the given base name and locale
         *
         * @param baseName
         *            the given base name
         * @param locale
         *            the locale to use
         * @return the name of a resource bundle according to the given base
         *         name and locale
         */
        CARAPI ToBundleName(
            /* [in] */ const String& baseName,
            /* [in] */ ILocale* locale,
            /* [out] */ String* str);

        /**
         * a utility method to answer the name of a resource according to the
         * given bundleName and suffix
         *
         * @param bundleName
         *            the given bundle name
         * @param suffix
         *            the suffix
         * @return the name of a resource according to the given bundleName and
         *         suffix
         */
        CARAPI ToResourceName(
            /* [in] */ const String& bundleName,
            /* [in] */ const String& suffix,
            /* [out] */ String* str);

        /**
         * Returns a control according to {@code formats}.
         */
        static AutoPtr<Control> GetControl(
            /* [in] */ IList* formats);

        /**
         * Returns a control according to {@code formats} whose fallback
         * locale is null.
         */
        static AutoPtr<Control> GetNoFallbackControl(
            /* [in] */ IList* formats);

    private:
        static CARAPI_(Boolean) isInit();

    public:
        static AutoPtr<IList> sListDefault;

        static AutoPtr<IList> sListClass;

        static AutoPtr<IList> sListProperties;

        static String JAVACLASS;

        static String JAVAPROPERTIES;

        /**
         * a list defines default format
         */
        static const AutoPtr<IList> FORMAT_DEFAULT;

        /**
         * a list defines java class format
         */
        static const AutoPtr<IList> FORMAT_CLASS;

        /**
         * a list defines property format
         */
        static const AutoPtr<IList> FORMAT_PROPERTIES;

        /**
         * a constant that indicates cache will not be used.
         */
        static const Int64 TTL_DONT_CACHE;

        /**
         * a constant that indicates cache will not be expired.
         */
        static const Int64 TTL_NO_EXPIRATION_CONTROL;

        AutoPtr<IList> mFormat;

    private:
        static const AutoPtr<Control> FORMAT_PROPERTIES_CONTROL;

        static const AutoPtr<Control> FORMAT_CLASS_CONTROL;

        static const AutoPtr<Control> FORMAT_DEFAULT_CONTROL;

        static Boolean isinitflag;
    };
private:
    class NoFallbackControl : public Control
    {
    public:
        NoFallbackControl(
            /* [in] */ const String& format);

        NoFallbackControl(
            /* [in] */ IList* list);

        // @Override
        CARAPI GetFallbackLocale(
            /* [in] */ const String& baseName,
            /* [in] */ ILocale* locale,
            /* [out] */ ILocale** outlocale);

    public:
        static const AutoPtr<Control> NOFALLBACK_FORMAT_PROPERTIES_CONTROL; // = new NoFallbackControl(JAVAPROPERTIES);

        static const AutoPtr<Control> NOFALLBACK_FORMAT_CLASS_CONTROL; // = new NoFallbackControl(JAVACLASS);

        static const AutoPtr<Control> NOFALLBACK_FORMAT_DEFAULT_CONTROL; // = new NoFallbackControl(listDefault);
    };

    class SimpleControl : public Control
    {
    public:
        SimpleControl(
            /* [in] */ const String& format);
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of this class.
     */
    ResourceBundle();

    /**
     * Finds the named resource bundle for the default {@code Locale} and the caller's
     * {@code ClassLoader}.
     *
     * @param bundleName
     *            the name of the {@code ResourceBundle}.
     * @return the requested {@code ResourceBundle}.
     * @throws MissingResourceException
     *                if the {@code ResourceBundle} cannot be found.
     */
    static CARAPI GetBundle(
        /* [in] */ const String& bundleName,
        /* [out] */ IResourceBundle** outrb);

    /**
     * Finds the named {@code ResourceBundle} for the specified {@code Locale} and the caller
     * {@code ClassLoader}.
     *
     * @param bundleName
     *            the name of the {@code ResourceBundle}.
     * @param locale
     *            the {@code Locale}.
     * @return the requested resource bundle.
     * @throws MissingResourceException
     *                if the resource bundle cannot be found.
     */
    static CARAPI GetBundle(
        /* [in] */ const String& bundleName,
        /* [in] */ ILocale* locale,
        /* [out] */ IResourceBundle** outrb);

    /**
     * Finds the named resource bundle for the specified {@code Locale} and {@code ClassLoader}.
     *
     * The passed base name and {@code Locale} are used to create resource bundle names.
     * The first name is created by concatenating the base name with the result
     * of {@link Locale#toString()}. From this name all parent bundle names are
     * derived. Then the same thing is done for the default {@code Locale}. This results
     * in a list of possible bundle names.
     *
     * <strong>Example</strong> For the basename "BaseName", the {@code Locale} of the
     * German part of Switzerland (de_CH) and the default {@code Locale} en_US the list
     * would look something like this:
     *
     * <ol>
     * <li>BaseName_de_CH</li>
     * <li>BaseName_de</li>
     * <li>Basename_en_US</li>
     * <li>Basename_en</li>
     * <li>BaseName</li>
     * </ol>
     *
     * This list also shows the order in which the bundles will be searched for a requested
     * resource in the German part of Switzerland (de_CH).
     *
     * As a first step, this method tries to instantiate
     * a {@code ResourceBundle} with the names provided.
     * If such a class can be instantiated and initialized, it is returned and
     * all the parent bundles are instantiated too. If no such class can be
     * found this method tries to load a {@code .properties} file with the names by
     * replacing dots in the base name with a slash and by appending
     * "{@code .properties}" at the end of the string. If such a resource can be found
     * by calling {@link ClassLoader#getResource(String)} it is used to
     * initialize a {@link PropertyResourceBundle}. If this succeeds, it will
     * also load the parents of this {@code ResourceBundle}.
     *
     * For compatibility with older code, the bundle name isn't required to be
     * a fully qualified class name. It's also possible to directly pass
     * the path to a properties file (without a file extension).
     *
     * @param bundleName
     *            the name of the {@code ResourceBundle}.
     * @param locale
     *            the {@code Locale}.
     * @param loader
     *            the {@code ClassLoader} to use.
     * @return the requested {@code ResourceBundle}.
     * @throws MissingResourceException
     *                if the {@code ResourceBundle} cannot be found.
     */
    static CARAPI GetBundle(
        /* [in] */ const String& bundleName,
        /* [in] */ ILocale* locale,
        /* [in] */ IClassLoader* loader,
        /* [out] */ IResourceBundle** outrb);

    static CARAPI MissingResourceException(
        /* [in] */ const String& className,
        /* [in] */ const String& key);

    /**
     * Finds the named resource bundle for the specified base name and control.
     *
     * @param baseName
     *            the base name of a resource bundle
     * @param control
     *            the control that control the access sequence
     * @return the named resource bundle
     *
     * @since 1.6
     */
    static CARAPI GetBundle(
        /* [in] */ const String& baseName,
        /* [in] */ IResourceBundleControl* control,
        /* [out] */ IResourceBundle** outrb);

    /**
     * Finds the named resource bundle for the specified base name and control.
     *
     * @param baseName
     *            the base name of a resource bundle
     * @param targetLocale
     *            the target locale of the resource bundle
     * @param control
     *            the control that control the access sequence
     * @return the named resource bundle
     *
     * @since 1.6
     */
    static CARAPI GetBundle(
        /* [in] */ const String& baseName,
        /* [in] */ ILocale* targetLocale,
        /* [in] */ IResourceBundleControl* control,
        /* [out] */ IResourceBundle** outrb);


    /**
     * Finds the named resource bundle for the specified base name and control.
     *
     * @param baseName
     *            the base name of a resource bundle
     * @param targetLocale
     *            the target locale of the resource bundle
     * @param loader
     *            the class loader to load resource
     * @param control
     *            the control that control the access sequence
     * @return the named resource bundle
     *
     * @since 1.6
     */
    static CARAPI GetBundle(
        /* [in] */ const String& baseName,
        /* [in] */ ILocale* targetLocale,
        /* [in] */ IClassLoader* loader,
        /* [in] */ IResourceBundleControl* control,
        /* [out] */ IResourceBundle** outrb);

    /**
     * Returns the names of the resources contained in this {@code ResourceBundle}.
     *
     * @return an {@code Enumeration} of the resource names.
     */
    virtual CARAPI GetKeys(
        /* [out] */ IEnumeration** outenum) = 0;

    /**
     * Gets the {@code Locale} of this {@code ResourceBundle}. In case a bundle was not
     * found for the requested {@code Locale}, this will return the actual {@code Locale} of
     * this resource bundle that was found after doing a fallback.
     *
     * @return the {@code Locale} of this {@code ResourceBundle}.
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** outlocale);

    /**
     * Returns the named resource from this {@code ResourceBundle}. If the resource
     * cannot be found in this bundle, it falls back to the parent bundle (if
     * it's not null) by calling the {@link #handleGetObject} method. If the resource still
     * can't be found it throws a {@code MissingResourceException}.
     *
     * @param key
     *            the name of the resource.
     * @return the resource object.
     * @throws MissingResourceException
     *                if the resource is not found.
     */
    CARAPI GetObject(
        /* [in] */ const String& key,
        /* [out] */ IInterface** outface);

    /**
     * Returns the named string resource from this {@code ResourceBundle}.
     *
     * @param key
     *            the name of the resource.
     * @return the resource string.
     * @throws MissingResourceException
     *                if the resource is not found.
     * @throws ClassCastException
     *                if the resource found is not a string.
     * @see #getObject(String)
     */
    CARAPI GetString(
        /* [in] */ const String& key,
        /* [out] */ String* outstr);

    /**
     * Returns the named resource from this {@code ResourceBundle}.
     *
     * @param key
     *            the name of the resource.
     * @return the resource string array.
     * @throws MissingResourceException
     *                if the resource is not found.
     * @throws ClassCastException
     *                if the resource found is not an array of strings.
     * @see #getObject(String)
     */
    CARAPI GetStringArray(
        /* [in] */ const String& key,
        /* [out, callee] */ ArrayOf<String>** arrstr);

    CARAPI ContainsKey(
        /* [in] */ const String& key,
        /* [out] */ Boolean* value);

    CARAPI GetKeySet(
        /* [out] */ ISet** outset);

protected:
    /**
     * Returns the named resource from this {@code ResourceBundle}, or null if the
     * resource is not found.
     *
     * @param key
     *            the name of the resource.
     * @return the resource object.
     */
    virtual CARAPI HandleGetObject(
        /* [in] */ const String& key,
        /* [out] */ IInterface** outface) = 0;

    /**
     * Sets the parent resource bundle of this {@code ResourceBundle}. The parent is
     * searched for resources which are not found in this {@code ResourceBundle}.
     *
     * @param bundle
     *            the parent {@code ResourceBundle}.
     */
    CARAPI SetParent(
        /* [in] */ IResourceBundle* bundle);

    static CARAPI ClearCache();

    static CARAPI ClearCache(
        /* [in] */ IClassLoader* loader);

    virtual CARAPI HandleKeySet(
        /* [out] */ ISet** outset);

private:
    static AutoPtr<IClassLoader> GetLoader();

    static AutoPtr<IResourceBundle> ProcessGetBundle(
        /* [in] */ const String& baseName,
        /* [in] */ ILocale* targetLocale,
        /* [in] */ IClassLoader* loader,
        /* [in] */ IResourceBundleControl* control,
        /* [in] */ Boolean expired,
        /* [in] */ IResourceBundle* result);

    static AutoPtr<IResourceBundle> HandleGetBundle(
        /* [in] */ Boolean loadBase,
        /* [in] */ const String& base,
        /* [in] */ ILocale* locale,
        /* [in] */ IClassLoader* loader);

    static AutoPtr<IDictionary> GetLoaderCache(
        /* [in] */ IInterface* cacheKey);


    /**
     * Returns a locale with the most-specific field removed, or null if this
     * locale had an empty language, country and variant.
     */
    static AutoPtr<ILocale> Strip(
        /* [in] */ ILocale* locale);

    CARAPI SetLocale(
        /* [in] */ ILocale* locale);

protected:
    AutoPtr<IResourceBundle> mParent;

private:
    static CARAPI_(Boolean) ishostinit();

private:
    static Boolean ishostflag;

    static const String UNDER_SCORE;

    static const String EMPTY_STRING;

    static const ResourceBundle* MISSING;

    static const ResourceBundle* MISSINGBASE;

    static const AutoPtr<IMap> sCache;

    static AutoPtr<ILocale> sCacheLocale;

    static Object mLock;

    /**
     * The parent of this {@code ResourceBundle} that is used if this bundle doesn't
     * include the requested resource.
     */

    AutoPtr<ILocale> mLocale;

    Int64 mLastLoadTime;
};

class MissingBundle : public ResourceBundle
{
public:
    // @Override
    ECode GetKeys(
        /* [out] */ IEnumeration** outenum)
    {
        *outenum = NULL;
        return NOERROR;
    }

    // @Override
    ECode HandleGetObject(
        /* [in] */ const String& name,
        /* [out] */ IInterface** outface)
    {
        *outface = NULL;
        return NOERROR;
    }
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_RESOURCEBUNDLE_H__
