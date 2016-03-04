
#ifndef __ELASTOS_CORE_CSYSTEM_H__
#define __ELASTOS_CORE_CSYSTEM_H__

#include "_Elastos_Core_CSystem.h"
#include "Singleton.h"
#include "elastos/utility/etl/HashMap.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IPrintStream;
using Elastos::IO::Channels::IChannel;
using Elastos::Utility::IProperties;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Core {

CarClass(CSystem)
    , public Singleton
    , public ISystem
{
public:
    CARAPI constructor();

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Sets the standard input stream to the given user defined input stream.
     *
     * @param newIn
     *            the user defined input stream to set as the standard input
     *            stream.
     */
    CARAPI SetIn(
        /* [in] */ IInputStream* newIn);

    CARAPI GetIn(
        /* [out] */ IInputStream** input);

    /**
     * Sets the standard output stream to the given user defined output stream.
     *
     * @param newOut
     *            the user defined output stream to set as the standard output
     *            stream.
     */
    CARAPI SetOut(
        /* [in] */ IPrintStream* newOut);

    CARAPI GetOut(
        /* [out] */ IPrintStream** output);

    /**
     * Sets the standard error output stream to the given user defined output
     * stream.
     *
     * @param newErr
     *            the user defined output stream to set as the standard error
     *            output stream.
     */
    CARAPI SetErr(
        /* [in] */ IPrintStream* newIn);

    CARAPI GetErr(
        /* [out] */ IPrintStream** err);

    /**
     * Returns the current system time in milliseconds since January 1, 1970
     * 00:00:00 UTC. This method shouldn't be used for measuring timeouts or
     * other elapsed time measurements, as changing the system time can affect
     * the results.
     *
     * @return the local system time in milliseconds.
     */
    CARAPI GetCurrentTimeMillis(
        /* [out] */ Int64* value);

    /**
     * Returns the current timestamp of the most precise timer available on the
     * local system. This timestamp can only be used to measure an elapsed
     * period by comparing it against another timestamp. It cannot be used as a
     * very exact system time expression.
     *
     * @return the current timestamp in nanoseconds.
     */
    CARAPI GetNanoTime(
        /* [out] */ Int64* value);

    /**
     * Causes the VM to stop running and the program to exit. If
     * {@link #runFinalizersOnExit(boolean)} has been previously invoked with a
     * {@code true} argument, then all objects will be properly
     * garbage-collected and finalized first.
     *
     * @param code
     *            the return code.
     */
    CARAPI Exit(
        /* [in] */ Int32 code);

    /**
     * Indicates to the VM that it would be a good time to run the
     * garbage collector. Note that this is a hint only. There is no guarantee
     * that the garbage collector will actually be run.
     */
    CARAPI GC();

    /**
     * Returns an integer hash code for the parameter. The hash code returned is
     * the same one that would be returned by the method {@code
     * java.lang.Object.hashCode()}, whether or not the object's class has
     * overridden hashCode(). The hash code for {@code null} is {@code 0}.
     *
     * @param anObject
     *            the object to calculate the hash code.
     * @return the hash code for the given object.
     * @see java.lang.Object#hashCode
     */
    CARAPI IdentityHashCode(
        /* [in] */ IInterface* anObject,
        /* [out] */ Int32* hash);

    /**
     * Returns the system's line separator. On Android, this is {@code "\n"}. The value
     * comes from the value of the {@code line.separator} system property when the VM
     * starts. Later changes to the property will not affect the value returned by this
     * method.
     * @since 1.7
     * @hide 1.7 - fix documentation references to "line.separator" in Formatter.
     */
    CARAPI GetLineSeparator(
        /* [out] */ String* value);

    /**
     * Returns the value of the environment variable with the given name {@code
     * var}.
     *
     * @param name
     *            the name of the environment variable.
     * @return the value of the specified environment variable or {@code null}
     *         if no variable exists with the given name.
     */
    CARAPI GetEnv(
        /* [in] */ const String& name,
        /* [out] */ String* value);

    CARAPI GetEnv(
        /* [in] */ const String& name,
        /* [in] */ const String& defaultValue,
        /* [out] */ String* value);

    /**
     * Returns an unmodifiable map of all available environment variables.
     *
     * @return the map representing all environment variables.
     */
    CARAPI GetEnvs(
        /* [out] */ IMap** map);

    /**
     * Returns the inherited channel from the creator of the current virtual
     * machine.
     *
     * @return the inherited {@link Channel} or {@code null} if none exists.
     * @throws IOException
     *             if an I/O error occurred.
     * @see SelectorProvider
     * @see SelectorProvider#inheritedChannel()
     */
    CARAPI InheritedChannel(
        /* [out] */ IChannel** channel);

    /**
     * Returns the system properties. Note that this is not a copy, so that
     * changes made to the returned Properties object will be reflected in
     * subsequent calls to getProperty and getProperties.
     *
     * @return the system properties.
     */
    CARAPI GetProperties(
        /* [out] */ IProperties** prop);

    /**
     * Returns the value of a particular system property or {@code null} if no
     * such property exists.
     *
     * <p>The following properties are always provided by the Dalvik VM:
     * <p><table BORDER="1" WIDTH="100%" CELLPADDING="3" CELLSPACING="0" SUMMARY="">
     * <tr BGCOLOR="#CCCCFF" CLASS="TableHeadingColor">
     *     <td><b>Name</b></td>        <td><b>Meaning</b></td>                    <td><b>Example</b></td></tr>
     * <tr><td>file.separator</td>     <td>{@link java.io.File#separator}</td>    <td>{@code /}</td></tr>
     *
     * <tr><td>java.class.path</td>    <td>System class path</td>                 <td>{@code .}</td></tr>
     * <tr><td>java.class.version</td> <td>(Not useful on Android)</td>           <td>{@code 50.0}</td></tr>
     * <tr><td>java.compiler</td>      <td>(Not useful on Android)</td>           <td>Empty</td></tr>
     * <tr><td>java.ext.dirs</td>      <td>(Not useful on Android)</td>           <td>Empty</td></tr>
     * <tr><td>java.home</td>          <td>Location of the VM on the file system</td> <td>{@code /system}</td></tr>
     * <tr><td>java.io.tmpdir</td>     <td>See {@link java.io.File#createTempFile}</td> <td>{@code /sdcard}</td></tr>
     * <tr><td>java.library.path</td>  <td>Search path for JNI libraries</td>     <td>{@code /system/lib}</td></tr>
     * <tr><td>java.vendor</td>        <td>Human-readable VM vendor</td>          <td>{@code The Android Project}</td></tr>
     * <tr><td>java.vendor.url</td>    <td>URL for VM vendor's web site</td>      <td>{@code http://www.android.com/}</td></tr>
     * <tr><td>java.version</td>       <td>(Not useful on Android)</td>           <td>{@code 0}</td></tr>
     *
     * <tr><td>java.specification.version</td>    <td>VM libraries version</td>        <td>{@code 0.9}</td></tr>
     * <tr><td>java.specification.vendor</td>     <td>VM libraries vendor</td>         <td>{@code The Android Project}</td></tr>
     * <tr><td>java.specification.name</td>       <td>VM libraries name</td>           <td>{@code Dalvik Core Library}</td></tr>
     * <tr><td>java.vm.version</td>               <td>VM implementation version</td>   <td>{@code 1.2.0}</td></tr>
     * <tr><td>java.vm.vendor</td>                <td>VM implementation vendor</td>    <td>{@code The Android Project}</td></tr>
     * <tr><td>java.vm.name</td>                  <td>VM implementation name</td>      <td>{@code Dalvik}</td></tr>
     * <tr><td>java.vm.specification.version</td> <td>VM specification version</td>    <td>{@code 0.9}</td></tr>
     * <tr><td>java.vm.specification.vendor</td>  <td>VM specification vendor</td>     <td>{@code The Android Project}</td></tr>
     * <tr><td>java.vm.specification.name</td>    <td>VM specification name</td>       <td>{@code Dalvik Virtual Machine Specification}</td></tr>
     *
     * <tr><td>line.separator</td>     <td>The system line separator</td>         <td>{@code \n}</td></tr>
     *
     * <tr><td>os.arch</td>            <td>OS architecture</td>                   <td>{@code armv7l}</td></tr>
     * <tr><td>os.name</td>            <td>OS (kernel) name</td>                  <td>{@code Linux}</td></tr>
     * <tr><td>os.version</td>         <td>OS (kernel) version</td>               <td>{@code 2.6.32.9-g103d848}</td></tr>
     *
     * <tr><td>path.separator</td>     <td>See {@link java.io.File#pathSeparator}</td> <td>{@code :}</td></tr>
     *
     * <tr><td>user.dir</td>           <td>Base of non-absolute paths</td>        <td>{@code /}</td></tr>
     * <tr><td>user.home</td>          <td>(Not useful on Android)</td>           <td>Empty</td></tr>
     * <tr><td>user.name</td>          <td>(Not useful on Android)</td>           <td>Empty</td></tr>
     *
     * </table>
     *
     * <p>It is a mistake to try to override any of these. Doing so will have unpredictable results.
     *
     * @param propertyName
     *            the name of the system property to look up.
     * @return the value of the specified system property or {@code null} if the
     *         property doesn't exist.
     */
    CARAPI GetProperty(
        /* [in] */ const String& propertyName,
        /* [out] */ String* value);

    /**
     * Returns the value of a particular system property. The {@code
     * defaultValue} will be returned if no such property has been found.
     *
     * @param prop
     *            the name of the system property to look up.
     * @param defaultValue
     *            the return value if the system property with the given name
     *            does not exist.
     * @return the value of the specified system property or the {@code
     *         defaultValue} if the property does not exist.
     */
    CARAPI GetProperty(
        /* [in] */ const String& prop,
        /* [in] */ const String& defaultValue,
        /* [out */ String* value);

    /**
     * Sets the value of a particular system property.
     *
     * @param prop
     *            the name of the system property to be changed.
     * @param value
     *            the value to associate with the given property {@code prop}.
     * @return the old value of the property or {@code null} if the property
     *         didn't exist.
     */
    CARAPI SetProperty(
        /* [in] */ const String& prop,
        /* [in] */ const String& value,
        /* [out] */ String* oldValue);

    /**
     * Removes a specific system property.
     *
     * @param key
     *            the name of the system property to be removed.
     * @return the property value or {@code null} if the property didn't exist.
     * @throws NullPointerException
     *             if the argument {@code key} is {@code null}.
     * @throws IllegalArgumentException
     *             if the argument {@code key} is empty.
     */
    CARAPI ClearProperty(
        /* [in] */ const String& prop,
        /* [out] */ String* value);

    /**
     * Loads and links the dynamic library that is identified through the
     * specified path. This method is similar to {@link #loadLibrary(String)},
     * but it accepts a full path specification whereas {@code loadLibrary} just
     * accepts the name of the library to load.
     *
     * @param pathName
     *            the path of the file to be loaded.
     */
    CARAPI Load(
        /* [in] */ const String& pathName);

    /**
     * Loads and links the library with the specified name. The mapping of the
     * specified library name to the full path for loading the library is
     * implementation-dependent.
     *
     * @param libName
     *            the name of the library to load.
     * @throws UnsatisfiedLinkError
     *             if the library could not be loaded.
     */
    CARAPI LoadLibrary(
        /* [in] */ const String& libName);

    /**
     * Returns the platform specific file name format for the shared library
     * named by the argument.
     *
     * @param userLibName
     *            the name of the library to look up.
     * @return the platform specific filename for the library.
     */
    CARAPI MapLibraryName(
        /* [in] */ const String& userLibName,
        /* [out] */ String* filename);

    /**
     * Provides a hint to the VM that it would be useful to attempt
     * to perform any outstanding object finalization.
     */
    CARAPI RunFinalization();

    /**
     * Ensures that, when the VM is about to exit, all objects are
     * finalized. Note that all finalization which occurs when the system is
     * exiting is performed after all running threads have been terminated.
     *
     * @param flag
     *            the flag determines if finalization on exit is enabled.
     * @deprecated this method is unsafe.
     */
    CARAPI RunFinalizersOnExit(
        /* [in] */ Boolean flag);

    /**
     * Sets all system properties. This does not take a copy; the passed-in object is used
     * directly. Passing null causes the VM to reinitialize the properties to how they were
     * when the VM was started.
     */
    CARAPI SetProperties(
        /* [in] */ IProperties* properties);

    /**
     * Throws {@code SecurityException}.
     *
     * <p>Security managers do <i>not</i> provide a secure environment for
     * executing untrusted code and are unsupported on Android. Untrusted code
     * cannot be safely isolated within a single VM on Android, so this method
     * <i>always</i> throws a {@code SecurityException}.
     *
     * @param sm a security manager
     * @throws SecurityException always
     */
    // static void setSecurityManager(SecurityManager sm)

    /**
     * Returns null. Android does not use {@code SecurityManager}. This method
     * is only provided for source compatibility.
     *
     * @return null
     */
    // public static SecurityManager getSecurityManager() {

    // From dalvik/VMRuntime.java
    //
    /**
     * Returns the runtime instruction set corresponding to a given ABI. Multiple
     * compatible ABIs might map to the same instruction set. For example
     * {@code armeabi-v7a} and {@code armeabi} might map to the instruction set {@code arm}.
     *
     * This influences the compilation of the applications classes.
     */
    CARAPI GetInstructionSet(
        /* [in] */ const String& abi,
        /* [out] */ String* instructionSet);

    CARAPI Is64BitInstructionSet(
        /* [in] */ const String& instructionSet,
        /* [out] */ Boolean* result);

    CARAPI Is64BitAbi(
        /* [in] */ const String& abi,
        /* [out] */ Boolean* result);

    CARAPI Is64Bit(
        /* [out] */ Boolean* result);

private:

    CARAPI InitSystemProperties();

    /*
     * Returns an environment variable. No security checks are performed.
     * @param var the name of the environment variable
     * @return the value of the specified environment variable
     */
    static CARAPI_(String) GetEnvByName(
        /* [in] */ const String& name);

    /**
     * Returns an array of "key=value" strings containing information not otherwise
     * easily available, such as #defined library versions.
     */
    static CARAPI_(AutoPtr<ArrayOf<String> >) SpecialProperties();

    /**
     * Adds each element of 'assignments' to 'p', treating each element as an
     * assignment in the form "key=value".
     */
    CARAPI ParsePropertyAssignments(
        /* [in] */ IProperties* p,
        /* [in] */ ArrayOf<String>* assignments);

private:
    static String sLineSeparator;
    static AutoPtr<IProperties> sSystemProperties;

    static AutoPtr<IInputStream> mIn;
    static AutoPtr<IPrintStream> mOut;
    static AutoPtr<IPrintStream> mErr;

    static const AutoPtr<HashMap<String, String> > ABI_TO_INSTRUCTION_SET_MAP;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_SYSTEM_H__
