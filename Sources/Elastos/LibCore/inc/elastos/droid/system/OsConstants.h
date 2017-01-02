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

#ifndef __ELASTOS_DROID_SYSTEM_OSCONSTANTS_H__
#define __ELASTOS_DROID_SYSTEM_OSCONSTANTS_H__

#include "Elastos.CoreLibrary.h"

namespace Elastos {
namespace Droid {
namespace System {

class ECO_PUBLIC OsConstants
{
public:
    const static Int32 _AF_INET;
    const static Int32 _AF_INET6;
    const static Int32 _AF_UNIX;
    const static Int32 _AF_UNSPEC;
    const static Int32 _AI_ADDRCONFIG;
    const static Int32 _AI_ALL;
    const static Int32 _AI_CANONNAME;
    const static Int32 _AI_NUMERICHOST;
    const static Int32 _AI_NUMERICSERV;
    const static Int32 _AI_PASSIVE;
    const static Int32 _AI_V4MAPPED;
    const static Int32 _CAP_AUDIT_CONTROL;
    const static Int32 _CAP_AUDIT_WRITE;
    const static Int32 _CAP_BLOCK_SUSPEND;
    const static Int32 _CAP_CHOWN;
    const static Int32 _CAP_DAC_OVERRIDE;
    const static Int32 _CAP_DAC_READ_SEARCH;
    const static Int32 _CAP_FOWNER;
    const static Int32 _CAP_FSETID;
    const static Int32 _CAP_IPC_LOCK;
    const static Int32 _CAP_IPC_OWNER;
    const static Int32 _CAP_KILL;
    const static Int32 _CAP_LAST_CAP;
    const static Int32 _CAP_LEASE;
    const static Int32 _CAP_LINUX_IMMUTABLE;
    const static Int32 _CAP_MAC_ADMIN;
    const static Int32 _CAP_MAC_OVERRIDE;
    const static Int32 _CAP_MKNOD;
    const static Int32 _CAP_NET_ADMIN;
    const static Int32 _CAP_NET_BIND_SERVICE;
    const static Int32 _CAP_NET_BROADCAST;
    const static Int32 _CAP_NET_RAW;
    const static Int32 _CAP_SETFCAP;
    const static Int32 _CAP_SETGID;
    const static Int32 _CAP_SETPCAP;
    const static Int32 _CAP_SETUID;
    const static Int32 _CAP_SYS_ADMIN;
    const static Int32 _CAP_SYS_BOOT;
    const static Int32 _CAP_SYS_CHROOT;
    const static Int32 _CAP_SYSLOG;
    const static Int32 _CAP_SYS_MODULE;
    const static Int32 _CAP_SYS_NICE;
    const static Int32 _CAP_SYS_PACCT;
    const static Int32 _CAP_SYS_PTRACE;
    const static Int32 _CAP_SYS_RAWIO;
    const static Int32 _CAP_SYS_RESOURCE;
    const static Int32 _CAP_SYS_TIME;
    const static Int32 _CAP_SYS_TTY_CONFIG;
    const static Int32 _CAP_WAKE_ALARM;
    const static Int32 _E2BIG;
    const static Int32 _EACCES;
    const static Int32 _EADDRINUSE;
    const static Int32 _EADDRNOTAVAIL;
    const static Int32 _EAFNOSUPPORT;
    const static Int32 _EAGAIN;
    const static Int32 _EAI_AGAIN;
    const static Int32 _EAI_BADFLAGS;
    const static Int32 _EAI_FAIL;
    const static Int32 _EAI_FAMILY;
    const static Int32 _EAI_MEMORY;
    const static Int32 _EAI_NODATA;
    const static Int32 _EAI_NONAME;
    const static Int32 _EAI_OVERFLOW;
    const static Int32 _EAI_SERVICE;
    const static Int32 _EAI_SOCKTYPE;
    const static Int32 _EAI_SYSTEM;
    const static Int32 _EALREADY;
    const static Int32 _EBADF;
    const static Int32 _EBADMSG;
    const static Int32 _EBUSY;
    const static Int32 _ECANCELED;
    const static Int32 _ECHILD;
    const static Int32 _ECONNABORTED;
    const static Int32 _ECONNREFUSED;
    const static Int32 _ECONNRESET;
    const static Int32 _EDEADLK;
    const static Int32 _EDESTADDRREQ;
    const static Int32 _EDOM;
    const static Int32 _EDQUOT;
    const static Int32 _EEXIST;
    const static Int32 _EFAULT;
    const static Int32 _EFBIG;
    const static Int32 _EHOSTUNREACH;
    const static Int32 _EIDRM;
    const static Int32 _EILSEQ;
    const static Int32 _EINPROGRESS;
    const static Int32 _EINTR;
    const static Int32 _EINVAL;
    const static Int32 _EIO;
    const static Int32 _EISCONN;
    const static Int32 _EISDIR;
    const static Int32 _ELOOP;
    const static Int32 _EMFILE;
    const static Int32 _EMLINK;
    const static Int32 _EMSGSIZE;
    const static Int32 _EMULTIHOP;
    const static Int32 _ENAMETOOLONG;
    const static Int32 _ENETDOWN;
    const static Int32 _ENETRESET;
    const static Int32 _ENETUNREACH;
    const static Int32 _ENFILE;
    const static Int32 _ENOBUFS;
    const static Int32 _ENODATA;
    const static Int32 _ENODEV;
    const static Int32 _ENOENT;
    const static Int32 _ENOEXEC;
    const static Int32 _ENOLCK;
    const static Int32 _ENOLINK;
    const static Int32 _ENOMEM;
    const static Int32 _ENOMSG;
    const static Int32 _ENOPROTOOPT;
    const static Int32 _ENOSPC;
    const static Int32 _ENOSR;
    const static Int32 _ENOSTR;
    const static Int32 _ENOSYS;
    const static Int32 _ENOTCONN;
    const static Int32 _ENOTDIR;
    const static Int32 _ENOTEMPTY;
    const static Int32 _ENOTSOCK;
    const static Int32 _ENOTSUP;
    const static Int32 _ENOTTY;
    const static Int32 _ENXIO;
    const static Int32 _EOPNOTSUPP;
    const static Int32 _EOVERFLOW;
    const static Int32 _EPERM;
    const static Int32 _EPIPE;
    const static Int32 _EPROTO;
    const static Int32 _EPROTONOSUPPORT;
    const static Int32 _EPROTOTYPE;
    const static Int32 _ERANGE;
    const static Int32 _EROFS;
    const static Int32 _ESPIPE;
    const static Int32 _ESRCH;
    const static Int32 _ESTALE;
    const static Int32 _ETIME;
    const static Int32 _ETIMEDOUT;
    const static Int32 _ETXTBSY;
    // On Linux, EWOULDBLOCK == EAGAIN. Use EAGAIN instead, to reduce confusion.
    const static Int32 _EXDEV;
    const static Int32 _EXIT_FAILURE;
    const static Int32 _EXIT_SUCCESS;
    const static Int32 _FD_CLOEXEC;
    const static Int32 _FIONREAD;
    const static Int32 _F_DUPFD;
    const static Int32 _F_GETFD;
    const static Int32 _F_GETFL;
    const static Int32 _F_GETLK;
    const static Int32 _F_GETLK64;
    const static Int32 _F_GETOWN;
    const static Int32 _F_OK;
    const static Int32 _F_RDLCK;
    const static Int32 _F_SETFD;
    const static Int32 _F_SETFL;
    const static Int32 _F_SETLK;
    const static Int32 _F_SETLK64;
    const static Int32 _F_SETLKW;
    const static Int32 _F_SETLKW64;
    const static Int32 _F_SETOWN;
    const static Int32 _F_UNLCK;
    const static Int32 _F_WRLCK;
    const static Int32 _IFA_F_DADFAILED;
    const static Int32 _IFA_F_DEPRECATED;
    const static Int32 _IFA_F_HOMEADDRESS;
    const static Int32 _IFA_F_NODAD;
    const static Int32 _IFA_F_OPTIMISTIC;
    const static Int32 _IFA_F_PERMANENT;
    const static Int32 _IFA_F_SECONDARY;
    const static Int32 _IFA_F_TEMPORARY;
    const static Int32 _IFA_F_TENTATIVE;
    const static Int32 _IFF_ALLMULTI;
    const static Int32 _IFF_AUTOMEDIA;
    const static Int32 _IFF_BROADCAST;
    const static Int32 _IFF_DEBUG;
    const static Int32 _IFF_DYNAMIC;
    const static Int32 _IFF_LOOPBACK;
    const static Int32 _IFF_MASTER;
    const static Int32 _IFF_MULTICAST;
    const static Int32 _IFF_NOARP;
    const static Int32 _IFF_NOTRAILERS;
    const static Int32 _IFF_POINTOPOINT;
    const static Int32 _IFF_PORTSEL;
    const static Int32 _IFF_PROMISC;
    const static Int32 _IFF_RUNNING;
    const static Int32 _IFF_SLAVE;
    const static Int32 _IFF_UP;
    const static Int32 _IPPROTO_ICMP;
    const static Int32 _IPPROTO_ICMPV6;
    const static Int32 _IPPROTO_IP;
    const static Int32 _IPPROTO_IPV6;
    const static Int32 _IPPROTO_RAW;
    const static Int32 _IPPROTO_TCP;
    const static Int32 _IPPROTO_UDP;
    const static Int32 _IPV6_CHECKSUM;
    const static Int32 _IPV6_MULTICAST_HOPS;
    const static Int32 _IPV6_MULTICAST_IF;
    const static Int32 _IPV6_MULTICAST_LOOP;
    const static Int32 _IPV6_RECVDSTOPTS;
    const static Int32 _IPV6_RECVHOPLIMIT;
    const static Int32 _IPV6_RECVHOPOPTS;
    const static Int32 _IPV6_RECVPKTINFO;
    const static Int32 _IPV6_RECVRTHDR;
    const static Int32 _IPV6_RECVTCLASS;
    const static Int32 _IPV6_TCLASS;
    const static Int32 _IPV6_UNICAST_HOPS;
    const static Int32 _IPV6_V6ONLY;
    const static Int32 _IP_MULTICAST_IF;
    const static Int32 _IP_MULTICAST_LOOP;
    const static Int32 _IP_MULTICAST_TTL;
    const static Int32 _IP_TOS;
    const static Int32 _IP_TTL;
    const static Int32 _MAP_FIXED;
    const static Int32 _MAP_PRIVATE;
    const static Int32 _MAP_SHARED;
    const static Int32 _MCAST_JOIN_GROUP;
    const static Int32 _MCAST_LEAVE_GROUP;
    const static Int32 _MCAST_JOIN_SOURCE_GROUP;
    const static Int32 _MCAST_LEAVE_SOURCE_GROUP;
    const static Int32 _MCAST_BLOCK_SOURCE;
    const static Int32 _MCAST_UNBLOCK_SOURCE;
    const static Int32 _MCL_CURRENT;
    const static Int32 _MCL_FUTURE;
    const static Int32 _MSG_CTRUNC;
    const static Int32 _MSG_DONTROUTE;
    const static Int32 _MSG_EOR;
    const static Int32 _MSG_OOB;
    const static Int32 _MSG_PEEK;
    const static Int32 _MSG_TRUNC;
    const static Int32 _MSG_WAITALL;
    const static Int32 _MS_ASYNC;
    const static Int32 _MS_INVALIDATE;
    const static Int32 _MS_SYNC;
    const static Int32 _NI_DGRAM;
    const static Int32 _NI_NAMEREQD;
    const static Int32 _NI_NOFQDN;
    const static Int32 _NI_NUMERICHOST;
    const static Int32 _NI_NUMERICSERV;
    const static Int32 _O_ACCMODE;
    const static Int32 _O_APPEND;
    const static Int32 _O_CREAT;
    const static Int32 _O_EXCL;
    const static Int32 _O_NOCTTY;
    const static Int32 _O_NOFOLLOW;
    const static Int32 _O_NONBLOCK;
    const static Int32 _O_RDONLY;
    const static Int32 _O_RDWR;
    const static Int32 _O_SYNC;
    const static Int32 _O_TRUNC;
    const static Int32 _O_WRONLY;
    const static Int32 _POLLERR;
    const static Int32 _POLLHUP;
    const static Int32 _POLLIN;
    const static Int32 _POLLNVAL;
    const static Int32 _POLLOUT;
    const static Int32 _POLLPRI;
    const static Int32 _POLLRDBAND;
    const static Int32 _POLLRDNORM;
    const static Int32 _POLLWRBAND;
    const static Int32 _POLLWRNORM;
    const static Int32 _PR_GET_DUMPABLE;
    const static Int32 _PR_SET_DUMPABLE;
    const static Int32 _PR_SET_NO_NEW_PRIVS;
    const static Int32 _PROT_EXEC;
    const static Int32 _PROT_NONE;
    const static Int32 _PROT_READ;
    const static Int32 _PROT_WRITE;
    const static Int32 _R_OK;
    const static Int32 _RT_SCOPE_HOST;
    const static Int32 _RT_SCOPE_LINK;
    const static Int32 _RT_SCOPE_NOWHERE;
    const static Int32 _RT_SCOPE_SITE;
    const static Int32 _RT_SCOPE_UNIVERSE;
    const static Int32 _SEEK_CUR;
    const static Int32 _SEEK_END;
    const static Int32 _SEEK_SET;
    const static Int32 _SHUT_RD;
    const static Int32 _SHUT_RDWR;
    const static Int32 _SHUT_WR;
    const static Int32 _SIGABRT;
    const static Int32 _SIGALRM;
    const static Int32 _SIGBUS;
    const static Int32 _SIGCHLD;
    const static Int32 _SIGCONT;
    const static Int32 _SIGFPE;
    const static Int32 _SIGHUP;
    const static Int32 _SIGILL;
    const static Int32 _SIGINT;
    const static Int32 _SIGIO;
    const static Int32 _SIGKILL;
    const static Int32 _SIGPIPE;
    const static Int32 _SIGPROF;
    const static Int32 _SIGPWR;
    const static Int32 _SIGQUIT;
    const static Int32 _SIGRTMAX;
    const static Int32 _SIGRTMIN;
    const static Int32 _SIGSEGV;
    const static Int32 _SIGSTKFLT;
    const static Int32 _SIGSTOP;
    const static Int32 _SIGSYS;
    const static Int32 _SIGTERM;
    const static Int32 _SIGTRAP;
    const static Int32 _SIGTSTP;
    const static Int32 _SIGTTIN;
    const static Int32 _SIGTTOU;
    const static Int32 _SIGURG;
    const static Int32 _SIGUSR1;
    const static Int32 _SIGUSR2;
    const static Int32 _SIGVTALRM;
    const static Int32 _SIGWINCH;
    const static Int32 _SIGXCPU;
    const static Int32 _SIGXFSZ;
    const static Int32 _SIOCGIFADDR;
    const static Int32 _SIOCGIFBRDADDR;
    const static Int32 _SIOCGIFDSTADDR;
    const static Int32 _SIOCGIFNETMASK;
    const static Int32 _SOCK_DGRAM;
    const static Int32 _SOCK_RAW;
    const static Int32 _SOCK_SEQPACKET;
    const static Int32 _SOCK_STREAM;
    const static Int32 _SOL_SOCKET;
    const static Int32 _SO_BINDTODEVICE;
    const static Int32 _SO_BROADCAST;
    const static Int32 _SO_DEBUG;
    const static Int32 _SO_DONTROUTE;
    const static Int32 _SO_ERROR;
    const static Int32 _SO_KEEPALIVE;
    const static Int32 _SO_LINGER;
    const static Int32 _SO_OOBINLINE;
    const static Int32 _SO_PASSCRED;
    const static Int32 _SO_PEERCRED;
    const static Int32 _SO_RCVBUF;
    const static Int32 _SO_RCVLOWAT;
    const static Int32 _SO_RCVTIMEO;
    const static Int32 _SO_REUSEADDR;
    const static Int32 _SO_SNDBUF;
    const static Int32 _SO_SNDLOWAT;
    const static Int32 _SO_SNDTIMEO;
    const static Int32 _SO_TYPE;
    const static Int32 _STDERR_FILENO;
    const static Int32 _STDIN_FILENO;
    const static Int32 _STDOUT_FILENO;
    const static Int32 _S_IFBLK;
    const static Int32 _S_IFCHR;
    const static Int32 _S_IFDIR;
    const static Int32 _S_IFIFO;
    const static Int32 _S_IFLNK;
    const static Int32 _S_IFMT;
    const static Int32 _S_IFREG;
    const static Int32 _S_IFSOCK;
    const static Int32 _S_IRGRP;
    const static Int32 _S_IROTH;
    const static Int32 _S_IRUSR;
    const static Int32 _S_IRWXG;
    const static Int32 _S_IRWXO;
    const static Int32 _S_IRWXU;
    const static Int32 _S_ISGID;
    const static Int32 _S_ISUID;
    const static Int32 _S_ISVTX;
    const static Int32 _S_IWGRP;
    const static Int32 _S_IWOTH;
    const static Int32 _S_IWUSR;
    const static Int32 _S_IXGRP;
    const static Int32 _S_IXOTH;
    const static Int32 _S_IXUSR;
    const static Int32 _TCP_NODELAY;
    const static Int32 _WCONTINUED;
    const static Int32 _WEXITED;
    const static Int32 _WNOHANG;
    const static Int32 _WNOWAIT;
    const static Int32 _WSTOPPED;
    const static Int32 _WUNTRACED;
    const static Int32 _W_OK;
    const static Int32 _X_OK;
    const static Int32 __SC_2_CHAR_TERM;
    const static Int32 __SC_2_C_BIND;
    const static Int32 __SC_2_C_DEV;
    const static Int32 __SC_2_C_VERSION;
    const static Int32 __SC_2_FORT_DEV;
    const static Int32 __SC_2_FORT_RUN;
    const static Int32 __SC_2_LOCALEDEF;
    const static Int32 __SC_2_SW_DEV;
    const static Int32 __SC_2_UPE;
    const static Int32 __SC_2_VERSION;
    const static Int32 __SC_AIO_LISTIO_MAX;
    const static Int32 __SC_AIO_MAX;
    const static Int32 __SC_AIO_PRIO_DELTA_MAX;
    const static Int32 __SC_ARG_MAX;
    const static Int32 __SC_ASYNCHRONOUS_IO;
    const static Int32 __SC_ATEXIT_MAX;
    const static Int32 __SC_AVPHYS_PAGES;
    const static Int32 __SC_BC_BASE_MAX;
    const static Int32 __SC_BC_DIM_MAX;
    const static Int32 __SC_BC_SCALE_MAX;
    const static Int32 __SC_BC_STRING_MAX;
    const static Int32 __SC_CHILD_MAX;
    const static Int32 __SC_CLK_TCK;
    const static Int32 __SC_COLL_WEIGHTS_MAX;
    const static Int32 __SC_DELAYTIMER_MAX;
    const static Int32 __SC_EXPR_NEST_MAX;
    const static Int32 __SC_FSYNC;
    const static Int32 __SC_GETGR_R_SIZE_MAX;
    const static Int32 __SC_GETPW_R_SIZE_MAX;
    const static Int32 __SC_IOV_MAX;
    const static Int32 __SC_JOB_CONTROL;
    const static Int32 __SC_LINE_MAX;
    const static Int32 __SC_LOGIN_NAME_MAX;
    const static Int32 __SC_MAPPED_FILES;
    const static Int32 __SC_MEMLOCK;
    const static Int32 __SC_MEMLOCK_RANGE;
    const static Int32 __SC_MEMORY_PROTECTION;
    const static Int32 __SC_MESSAGE_PASSING;
    const static Int32 __SC_MQ_OPEN_MAX;
    const static Int32 __SC_MQ_PRIO_MAX;
    const static Int32 __SC_NGROUPS_MAX;
    const static Int32 __SC_NPROCESSORS_CONF;
    const static Int32 __SC_NPROCESSORS_ONLN;
    const static Int32 __SC_OPEN_MAX;
    const static Int32 __SC_PAGESIZE;
    const static Int32 __SC_PAGE_SIZE;
    const static Int32 __SC_PASS_MAX;
    const static Int32 __SC_PHYS_PAGES;
    const static Int32 __SC_PRIORITIZED_IO;
    const static Int32 __SC_PRIORITY_SCHEDULING;
    const static Int32 __SC_REALTIME_SIGNALS;
    const static Int32 __SC_RE_DUP_MAX;
    const static Int32 __SC_RTSIG_MAX;
    const static Int32 __SC_SAVED_IDS;
    const static Int32 __SC_SEMAPHORES;
    const static Int32 __SC_SEM_NSEMS_MAX;
    const static Int32 __SC_SEM_VALUE_MAX;
    const static Int32 __SC_SHARED_MEMORY_OBJECTS;
    const static Int32 __SC_SIGQUEUE_MAX;
    const static Int32 __SC_STREAM_MAX;
    const static Int32 __SC_SYNCHRONIZED_IO;
    const static Int32 __SC_THREADS;
    const static Int32 __SC_THREAD_ATTR_STACKADDR;
    const static Int32 __SC_THREAD_ATTR_STACKSIZE;
    const static Int32 __SC_THREAD_DESTRUCTOR_ITERATIONS;
    const static Int32 __SC_THREAD_KEYS_MAX;
    const static Int32 __SC_THREAD_PRIORITY_SCHEDULING;
    const static Int32 __SC_THREAD_PRIO_INHERIT;
    const static Int32 __SC_THREAD_PRIO_PROTECT;
    const static Int32 __SC_THREAD_SAFE_FUNCTIONS;
    const static Int32 __SC_THREAD_STACK_MIN;
    const static Int32 __SC_THREAD_THREADS_MAX;
    const static Int32 __SC_TIMERS;
    const static Int32 __SC_TIMER_MAX;
    const static Int32 __SC_TTY_NAME_MAX;
    const static Int32 __SC_TZNAME_MAX;
    const static Int32 __SC_VERSION;
    const static Int32 __SC_XBS5_ILP32_OFF32;
    const static Int32 __SC_XBS5_ILP32_OFFBIG;
    const static Int32 __SC_XBS5_LP64_OFF64;
    const static Int32 __SC_XBS5_LPBIG_OFFBIG;
    const static Int32 __SC_XOPEN_CRYPT;
    const static Int32 __SC_XOPEN_ENH_I18N;
    const static Int32 __SC_XOPEN_LEGACY;
    const static Int32 __SC_XOPEN_REALTIME;
    const static Int32 __SC_XOPEN_REALTIME_THREADS;
    const static Int32 __SC_XOPEN_SHM;
    const static Int32 __SC_XOPEN_UNIX;
    const static Int32 __SC_XOPEN_VERSION;
    const static Int32 __SC_XOPEN_XCU_VERSION;

public:
        /**
     * Tests whether the given mode is a block device.
     */
    static CARAPI_(Boolean) S_IsBLK(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the given mode is a character device.
     */
    static CARAPI_(Boolean) S_IsCHR(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the given mode is a directory.
     */
    static CARAPI_(Boolean) S_IsDIR(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the given mode is a FIFO.
     */
    static CARAPI_(Boolean) S_IsFIFO(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the given mode is a regular file.
     */
    static CARAPI_(Boolean) S_IsREG(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the given mode is a symbolic link.
     */
    static CARAPI_(Boolean) S_IsLNK(
        /* [in] */ Int32 mode);
    /**
     * Tests whether the given mode is a socket.
     */
    static CARAPI_(Boolean) S_IsSOCK(
        /* [in] */ Int32 mode);

    /**
     * Extracts the exit status of a child. Only valid if WIFEXITED returns true.
     */
    static CARAPI_(Int32) WExitStatus(
        /* [in] */ Int32 mode);
    /**
     * Tests whether the child dumped core. Only valid if WIFSIGNALED returns true.
     */
    static CARAPI_(Boolean) WCoreDump(
        /* [in] */ Int32 mode);

    /**
     * Returns the signal that caused the child to exit. Only valid if WIFSIGNALED returns true.
     */
    static CARAPI_(Int32) WTermSig(
        /* [in] */ Int32 mode);

    /**
     * Returns the signal that cause the child to stop. Only valid if WIFSTOPPED returns true.
     */
    static CARAPI_(Int32) WStopSig(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the child exited normally.
     */
    static CARAPI_(Boolean) WIfExited(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the child was stopped (not terminated) by a signal.
     */
    static CARAPI_(Boolean) WIfStopped(
        /* [in] */ Int32 mode);

    /**
     * Tests whether the child was terminated by a signal.
     */
    static CARAPI_(Boolean) WIfSignaled(
        /* [in] */ Int32 mode);

    /**
     * Returns the string name of a getaddrinfo(3) error value.
     * For example, "EAI_AGAIN".
     */
    static CARAPI_(String) GaiName(
        /* [in] */ Int32 error);
        /**
     * Returns the string name of an errno value.
     * For example, "EACCES". See {@link Os#strerror} for human-readable errno descriptions.
     */
    static CARAPI_(String) GetErrnoName(
        /* [in] */ Int32 error);
};

}// namespace System
}// namespace Droid
}// namespace Elastos

#endif