//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/os/SELinux.h"
#ifdef HAVE_SELINUX
#include "selinux/selinux.h"
#include "selinux/android.h"
#endif
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Os {

const String SELinux::TAG("SELinux");

static Boolean sIsSELinuxDisabled = TRUE;

Boolean SELinux::IsSELinuxEnabled()
{
    return !sIsSELinuxDisabled;
}

Boolean SELinux::IsSELinuxEnforced()
{
#ifdef HAVE_SELINUX
    return (security_getenforce() == 1) ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

Boolean SELinux::SetSELinuxEnforce(
    /* [in] */ Boolean value)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return FALSE;

    Int32 enforce = (value) ? 1 : 0;

    return (security_setenforce(enforce) != -1) ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

Boolean SELinux::SetFSCreateContext(
    /* [in] */ const String& context)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return FALSE;

    char * securityContext = NULL;
    const char *constant_securityContext = NULL;

    if (!context.IsNull()) {
        constant_securityContext = context.string();

        // GetStringUTFChars returns const char * yet setfscreatecon needs char *
        securityContext = const_cast<char *>(constant_securityContext);
    }

    int ret;
    if ((ret = setfscreatecon(securityContext)) == -1) goto bail;

    Slogger::V(TAG, "setFSCreateCon: set new security context to '%s' ", context.IsNull() ? "default" : context.string());

bail:
    return (ret == 0) ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

Boolean SELinux::SetFileContext(
    /* [in] */ const String& path,
    /* [in] */ const String& context)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return FALSE;

    if (path.IsNull()) {
        // throw_NullPointerException(env, "Trying to change the security context of a NULL file object.");
        return FALSE;
    }

    if (context.IsNull()) {
      // throw_NullPointerException(env, "Trying to set the security context of a file object with NULL.");
      return FALSE;
    }

    const char *objectPath = path.string();
    const char *constant_con = context.string();

    // GetStringUTFChars returns const char * yet setfilecon needs char *
    char *newCon = const_cast<char *>(constant_con);

    int ret;
    if ((ret = setfilecon(objectPath, newCon)) == -1) goto bail;

    Slogger::V(TAG, "setFileCon: Succesfully set security context '%s' for '%s'", newCon, objectPath);

bail:
    return (ret == 0) ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

String SELinux::GetFileContext(
    /* [in] */ const String& path)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return String(NULL);

    if (path.IsNull()) {
        // throw_NullPointerException(env, "Trying to check security context of a null path.");
        return String(NULL);
    }

    const char *objectPath = path.string();

    security_context_t context = NULL;
    String securityString;

    if (getfilecon(objectPath, &context) == -1) goto bail;

    Slogger::V(TAG, "getFileCon: Successfully retrived context '%s' for file '%s'", context, objectPath);

    securityString = context;

bail:
    if (context != NULL) freecon(context);

    return securityString;
#else
    return String(NULL);
#endif
}

String SELinux::GetPeerContext(
    /* [in] */ IFileDescriptor* fileDescriptor)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return String(NULL);

    if (fileDescriptor == NULL) {
        // throw_NullPointerException(env, "Trying to check security context of a null peer socket.");
        return String(NULL);
    }

    security_context_t context = NULL;
    String securityString;

    Int32 fd;
    fileDescriptor->GetDescriptor(&fd);

    if (getpeercon(fd, &context) == -1) goto bail;

    Slogger::V(TAG, "getPeerCon: Successfully retrived context of peer socket '%s'", context);

    securityString = context;

bail:
    if (context != NULL) freecon(context);

    return securityString;
#else
    return String(NULL);
#endif
}

String SELinux::GetContext()
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return String(NULL);

    security_context_t context = NULL;
    String securityString;

    if (getcon(&context) == -1) goto bail;

    Slogger::V(TAG, "getCon: Successfully retrieved context '%s'", context);

    securityString = context;

bail:
    if (context != NULL) freecon(context);

    return securityString;
#else
    return String(NULL);
#endif
}

String SELinux::GetPidContext(
    /* [in] */ Int32 pid)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return String(NULL);

    security_context_t context = NULL;
    String securityString;

    pid_t checkPid = (pid_t)pid;

    if (getpidcon(checkPid, &context) == -1) goto bail;

    Slogger::V(TAG, "getPidCon: Successfully retrived context '%s' for pid '%d'", context, checkPid);

    securityString = context;

bail:
    if (context != NULL) freecon(context);

    return securityString;
#else
    return String(NULL);
#endif
}

AutoPtr< ArrayOf<String> > SELinux::GetBooleanNames()
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return NULL;

    char **list;
    int i, len;
    AutoPtr< ArrayOf<String> > stringArray;

    if (security_get_boolean_names(&list, &len) == -1) return NULL;

    stringArray = ArrayOf<String>::Alloc(len);
    for (i = 0; i < len; i++) {
        (*stringArray)[i] = list[i];
        free(list[i]);
    }
    free(list);

    return stringArray;
#else
    return NULL;
#endif
}

Boolean SELinux::GetBooleanValue(
    /* [in] */ const String& name)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return FALSE;

    const char *boolean_name;
    int ret;

    if (name.IsNull()) return FALSE;
    boolean_name = name.string();
    ret = security_get_boolean_active(boolean_name);
    return (ret == 1) ? TRUE : FALSE;
#else
    return FALSE;
#endif
}

Boolean SELinux::SetBooleanValue(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return FALSE;

    const char *boolean_name = NULL;
    int ret;

    if (name.IsNull()) return FALSE;
    boolean_name = name.string();
    ret = security_set_boolean(boolean_name, (value) ? 1 : 0);
    if (ret) return FALSE;

    if (security_commit_booleans() == -1) return FALSE;

    return TRUE;
#else
    return FALSE;
#endif
}

Boolean SELinux::CheckSELinuxAccess(
    /* [in] */ const String& scon,
    /* [in] */ const String& tcon,
    /* [in] */ const String& tclass,
    /* [in] */ const String& perm)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return TRUE;

    int accessGranted = -1;

    const char *const_scon, *const_tcon, *mytclass, *myperm;
    char *myscon, *mytcon;

    if (scon.IsNull() || tcon.IsNull() || tclass.IsNull() || perm.IsNull()) goto bail;

    const_scon = scon.string();
    const_tcon = tcon.string();
    mytclass   = tclass.string();
    myperm     = perm.string();

    // selinux_check_access needs char* for some
    myscon = const_cast<char *>(const_scon);
    mytcon = const_cast<char *>(const_tcon);

    accessGranted = selinux_check_access(myscon, mytcon, mytclass, myperm, NULL);

    Slogger::V(TAG, "selinux_check_access returned %d", accessGranted);

bail:
    return (accessGranted == 0) ? TRUE : FALSE;
#else
    return TRUE;
#endif
}

Boolean SELinux::Restorecon(
    /* [in] */ const String& pathname)
{
    if (pathname.IsNull()) {
        // throw new NullPointerException();
        assert(0);
        return FALSE;
    }
    return NativeRestorecon(pathname, 0);
}

Boolean SELinux::NativeRestorecon(
    /* [in] */ const String& pathname,
    /* [in] */ Int32 flags)
{
#ifdef HAVE_SELINUX
    if (sIsSELinuxDisabled) return TRUE;

    const char *file = const_cast<char *>(pathname.string());
    int ret = selinux_android_restorecon(file, flags);
    return (ret == 0);
#else
    return TRUE;
#endif
}

Boolean SELinux::Restorecon(
    /* [in] */ IFile* file)
{
    // try {
    String path;
    file->GetCanonicalPath(&path);
    return NativeRestorecon(path, 0);
    // } catch (IOException e) {
    //     Slog.e(TAG, "Error getting canonical path. Restorecon failed for " +
    //            file.getPath(), e);
    //     return false;
    // }
}

Boolean SELinux::RestoreconRecursive(
    /* [in] */ IFile* file)
{
    // try {
    String path;
    file->GetCanonicalPath(&path);
    return NativeRestorecon(path, SELINUX_ANDROID_RESTORECON_RECURSE);
    // } catch (IOException e) {
    //     Slog.e(TAG, "Error getting canonical path. Restorecon failed for " +
    //             file.getPath(), e);
    //     return false;
    // }
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
