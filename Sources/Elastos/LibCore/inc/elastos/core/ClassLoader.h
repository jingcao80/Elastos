
#ifndef __ELASTOS_CORE_CLASSLOADER_H__
#define __ELASTOS_CORE_CLASSLOADER_H__

#include "elastos/coredef.h"
#include "elastos/core/Object.h"
#include "elastos/utility/etl/HashMap.h"

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Core {

class ClassLoader
    : public Object
    , public IClassLoader
{
private:
    /**
     * The 'System' ClassLoader - the one that is responsible for loading
     * classes from the classpath. It is not equal to the bootstrap class loader -
     * that one handles the built-in classes.
     *
     * Because of a potential class initialization race between ClassLoader and
     * java.lang.System, reproducible when using JDWP with "suspend=y", we defer
     * creation of the system class loader until first use. We use a static
     * inner class to get synchronization at init time without having to sync on
     * every access.
     *
     * @see #getSystemClassLoader()
     */
    class SystemClassLoader
    {
    public:
        static AutoPtr<IClassLoader> sLoader;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * Returns the system class loader. This is the parent for new
     * {@code ClassLoader} instances and is typically the class loader used to
     * start the application.
     */
    static CARAPI_(AutoPtr<IClassLoader>) GetSystemClassLoader();

    /**
     * Loads the class with the specified name. Invoking this method is
     * equivalent to calling {@code loadClass(className, false)}.
     * <p>
     * <strong>Note:</strong> In the Android reference implementation, the
     * second parameter of {@link #loadClass(String, boolean)} is ignored
     * anyway.
     * </p>
     *
     * @return the {@code Class} object.
     * @param className
     *            the name of the class to look for.
     * @throws ClassNotFoundException
     *             if the class can not be found.
     */
    CARAPI LoadClass(
        /* [in] */ const String& className,
        /* [out] */ IClassInfo** klass);

    static CARAPI_(AutoPtr<IClassLoader>) GetClassLoader(
        /* [in] */ IClassInfo* clsInfo);

protected:
    /**
     * Overridden by subclasses, throws a {@code ClassNotFoundException} by
     * default. This method is called by {@code loadClass} after the parent
     * {@code ClassLoader} has failed to find a loaded class of the same name.
     *
     * @param className
     *            the name of the class to look for.
     * @return the {@code Class} object that is found.
     * @throws ClassNotFoundException
     *             if the class cannot be found.
     */
    virtual CARAPI FindClass(
        /* [in] */ const String& className,
        /* [out] */ IClassInfo** klass);

    /**
     * Returns the class with the specified name if it has already been loaded
     * by the VM or {@code null} if it has not yet been loaded.
     *
     * @param className
     *            the name of the class to look for.
     * @return the {@code Class} object or {@code null} if the requested class
     *         has not been loaded.
     */
    virtual CARAPI_(AutoPtr<IClassInfo>) FindLoadedClass(
        /* [in] */ const String& className);

    /**
     * Loads the class with the specified name, optionally linking it after
     * loading. The following steps are performed:
     * <ol>
     * <li> Call {@link #findLoadedClass(String)} to determine if the requested
     * class has already been loaded.</li>
     * <li>If the class has not yet been loaded: Invoke this method on the
     * parent class loader.</li>
     * <li>If the class has still not been loaded: Call
     * {@link #findClass(String)} to find the class.</li>
     * </ol>
     * <p>
     * <strong>Note:</strong> In the Android reference implementation, the
     * {@code resolve} parameter is ignored; classes are never linked.
     * </p>
     *
     * @return the {@code Class} object.
     * @param className
     *            the name of the class to look for.
     * @param resolve
     *            Indicates if the class should be resolved after loading. This
     *            parameter is ignored on the Android reference implementation;
     *            classes are not resolved.
     * @throws ClassNotFoundException
     *             if the class can not be found.
     */
    virtual CARAPI LoadClass(
        /* [in] */ const String& className,
        /* [in] */ Boolean resolve,
        /* [out] */ IClassInfo** klass);

private:
    /**
     * Create the system class loader. Note this is NOT the bootstrap class
     * loader (which is managed by the VM). We use a null value for the parent
     * to indicate that the bootstrap loader is our parent.
     */
    static CARAPI_(AutoPtr<IClassLoader>) CreateSystemClassLoader();

private:
    /**
     * The parent ClassLoader.
     */
    AutoPtr<ClassLoader> mParent;

    HashMap<String, IClassInfo*> mClassTable;
};

/**
 * Provides an explicit representation of the boot class loader. It sits at the
 * head of the class loader chain and delegates requests to the VM's internal
 * class loading mechanism.
 */
class BootClassLoader : public ClassLoader
{
public:
    static CARAPI_(AutoPtr<BootClassLoader>) GetInstance();

protected:
    CARAPI FindClass(
        /* [in] */ const String& className,
        /* [out] */ IClassInfo** klass);

    CARAPI LoadClass(
        /* [in] */ const String& className,
        /* [in] */ Boolean resolve,
        /* [out] */ IClassInfo** klass);

private:
    static AutoPtr<BootClassLoader> sInstance;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CLASSLOADER_H__
