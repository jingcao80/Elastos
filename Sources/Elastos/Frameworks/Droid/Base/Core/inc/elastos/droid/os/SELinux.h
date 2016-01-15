#ifndef __ELASTOS_DROID_OS_SELINUX_H__
#define __ELASTOS_DROID_OS_SELINUX_H__

using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * This class provides access to the centralized jni bindings for
 * SELinux interaction.
 * {@hide}
 */
class SELinux
{
public:
    /**
     * Determine whether SELinux is disabled or enabled.
     * @return a boolean indicating whether SELinux is enabled.
     */
    static CARAPI_(Boolean) IsSELinuxEnabled();

    /**
     * Determine whether SELinux is permissive or enforcing.
     * @return a boolean indicating whether SELinux is enforcing.
     */
    static CARAPI_(Boolean) IsSELinuxEnforced();

    /**
     * Set whether SELinux is permissive or enforcing.
     * @param boolean representing whether to set SELinux to enforcing
     * @return a boolean representing whether the desired mode was set
     */
    static CARAPI_(Boolean) SetSELinuxEnforce(
        /* [in] */ Boolean value);

    /**
     * Sets the security context for newly created file objects.
     * @param context a security context given as a String.
     * @return a boolean indicating whether the operation succeeded.
     */
    static CARAPI_(Boolean) SetFSCreateContext(
        /* [in] */ const String& context);

    /**
     * Change the security context of an existing file object.
     * @param path representing the path of file object to relabel.
     * @param con new security context given as a String.
     * @return a boolean indicating whether the operation succeeded.
     */
    static CARAPI_(Boolean) SetFileContext(
        /* [in] */ const String& path,
        /* [in] */ const String& context);

    /**
     * Get the security context of a file object.
     * @param path the pathname of the file object.
     * @return a security context given as a String.
     */
    static CARAPI_(String) GetFileContext(
        /* [in] */ const String& path);

    /**
     * Get the security context of a peer socket.
     * @param fd FileDescriptor class of the peer socket.
     * @return a String representing the peer socket security context.
     */
    static CARAPI_(String) GetPeerContext(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Gets the security context of the current process.
     * @return a String representing the security context of the current process.
     */
    static CARAPI_(String) GetContext();

    /**
     * Gets the security context of a given process id.
     * Use of this function is discouraged for Binder transactions.
     * Use Binder.getCallingSecctx() instead.
     * @param pid an int representing the process id to check.
     * @return a String representing the security context of the given pid.
     */
    static CARAPI_(String) GetPidContext(
        /* [in] */ Int32 pid);

    /**
     * Gets a list of the SELinux boolean names.
     * @return an array of strings containing the SELinux boolean names.
     */
    static CARAPI_(AutoPtr< ArrayOf<String> >) GetBooleanNames();

    /**
     * Gets the value for the given SELinux boolean name.
     * @param String The name of the SELinux boolean.
     * @return a boolean indicating whether the SELinux boolean is set.
     */
    static CARAPI_(Boolean) GetBooleanValue(
        /* [in] */ const String& name);

    /**
     * Sets the value for the given SELinux boolean name.
     * @param String The name of the SELinux boolean.
     * @param Boolean The new value of the SELinux boolean.
     * @return a boolean indicating whether or not the operation succeeded.
     */
    static CARAPI_(Boolean) SetBooleanValue(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    /**
     * Check permissions between two security contexts.
     * @param scon The source or subject security context.
     * @param tcon The target or object security context.
     * @param tclass The object security class name.
     * @param perm The permission name.
     * @return a boolean indicating whether permission was granted.
     */
    static CARAPI_(Boolean) CheckSELinuxAccess(
        /* [in] */ const String& scon,
        /* [in] */ const String& tcon,
        /* [in] */ const String& tclass,
        /* [in] */ const String& perm);

    /**
     * Restores a file to its default SELinux security context.
     * If the system is not compiled with SELinux, then {@code true}
     * is automatically returned.
     * If SELinux is compiled in, but disabled, then {@code true} is
     * returned.
     *
     * @param pathname The pathname of the file to be relabeled.
     * @return a boolean indicating whether the relabeling succeeded.
     * @exception NullPointerException if the pathname is a null object.
     */
    static CARAPI_(Boolean) Restorecon(
        /* [in] */ const String& pathname);

    /**
     * Restores a file to its default SELinux security context.
     * If the system is not compiled with SELinux, then {@code true}
     * is automatically returned.
     * If SELinux is compiled in, but disabled, then {@code true} is
     * returned.
     *
     * @param file The File object representing the path to be relabeled.
     * @return a boolean indicating whether the relabeling succeeded.
     * @exception NullPointerException if the file is a null object.
     */
    static CARAPI_(Boolean) Restorecon(
        /* [in] */ IFile* file);

    /**
     * Recursively restores all files under the given path to their default
     * SELinux security context. If the system is not compiled with SELinux,
     * then {@code true} is automatically returned. If SELinux is compiled in,
     * but disabled, then {@code true} is returned.
     *
     * @return a boolean indicating whether the relabeling succeeded.
     */
    static CARAPI_(Boolean) RestoreconRecursive(
        /* [in] */ IFile* file);

private:
    /**
     * Restores a file to its default SELinux security context.
     * If the system is not compiled with SELinux, then {@code true}
     * is automatically returned.
     * If SELinux is compiled in, but disabled, then {@code true} is
     * returned.
     *
     * @param pathname The pathname of the file to be relabeled.
     * @return a boolean indicating whether the relabeling succeeded.
     */
    static CARAPI_(Boolean) NativeRestorecon(
        /* [in] */ const String& pathname,
        /* [in] */ Int32 flags);

private:
    static const String TAG;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_SELINUX_H__
