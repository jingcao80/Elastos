
#include "OsConstants.h"
// #include "Portability.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#ifndef __APPLE__
#include <sys/prctl.h>
#endif
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// After the others because these are not necessarily self-contained in glibc.
#ifndef __APPLE__
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#endif

#include <net/if.h> // After <sys/socket.h> to work around a Mac header file bug.

#if defined(__BIONIC__)
#include <linux/capability.h>
#endif

namespace Elastos {
namespace Droid {
namespace System {

const Int32 OsConstants::_AF_INET = AF_INET;
const Int32 OsConstants::_AF_INET6 = AF_INET6;
const Int32 OsConstants::_AF_UNIX = AF_UNIX;
const Int32 OsConstants::_AF_UNSPEC = AF_UNSPEC;
const Int32 OsConstants::_AI_ADDRCONFIG = AI_ADDRCONFIG;
const Int32 OsConstants::_AI_ALL = AI_ALL;
const Int32 OsConstants::_AI_CANONNAME = AI_CANONNAME;
const Int32 OsConstants::_AI_NUMERICHOST = AI_NUMERICHOST;
#if defined(AI_NUMERICSERV)
const Int32 OsConstants::_AI_NUMERICSERV = AI_NUMERICSERV;
#else
const Int32 OsConstants::_AI_NUMERICSERV = 0;
#endif
const Int32 OsConstants::_AI_PASSIVE = AI_PASSIVE;
const Int32 OsConstants::_AI_V4MAPPED = AI_V4MAPPED;
#if defined(CAP_LAST_CAP)
const Int32 OsConstants::_CAP_AUDIT_CONTROL = CAP_AUDIT_CONTROL;
const Int32 OsConstants::_CAP_AUDIT_WRITE = CAP_AUDIT_WRITE;
const Int32 OsConstants::_CAP_BLOCK_SUSPEND = CAP_BLOCK_SUSPEND;
const Int32 OsConstants::_CAP_CHOWN = CAP_CHOWN;
const Int32 OsConstants::_CAP_DAC_OVERRIDE = CAP_DAC_OVERRIDE;
const Int32 OsConstants::_CAP_DAC_READ_SEARCH = CAP_DAC_READ_SEARCH;
const Int32 OsConstants::_CAP_FOWNER = CAP_FOWNER;
const Int32 OsConstants::_CAP_FSETID = CAP_FSETID;
const Int32 OsConstants::_CAP_IPC_LOCK = CAP_IPC_LOCK;
const Int32 OsConstants::_CAP_IPC_OWNER = CAP_IPC_OWNER;
const Int32 OsConstants::_CAP_KILL = CAP_KILL;
const Int32 OsConstants::_CAP_LAST_CAP = CAP_LAST_CAP;
const Int32 OsConstants::_CAP_LEASE = CAP_LEASE;
const Int32 OsConstants::_CAP_LINUX_IMMUTABLE = CAP_LINUX_IMMUTABLE;
const Int32 OsConstants::_CAP_MAC_ADMIN = CAP_MAC_ADMIN;
const Int32 OsConstants::_CAP_MAC_OVERRIDE = CAP_MAC_OVERRIDE;
const Int32 OsConstants::_CAP_MKNOD = CAP_MKNOD;
const Int32 OsConstants::_CAP_NET_ADMIN = CAP_NET_ADMIN;
const Int32 OsConstants::_CAP_NET_BIND_SERVICE = CAP_NET_BIND_SERVICE;
const Int32 OsConstants::_CAP_NET_BROADCAST = CAP_NET_BROADCAST;
const Int32 OsConstants::_CAP_NET_RAW = CAP_NET_RAW;
const Int32 OsConstants::_CAP_SETFCAP = CAP_SETFCAP;
const Int32 OsConstants::_CAP_SETGID = CAP_SETGID;
const Int32 OsConstants::_CAP_SETPCAP = CAP_SETPCAP;
const Int32 OsConstants::_CAP_SETUID = CAP_SETUID;
const Int32 OsConstants::_CAP_SYS_ADMIN = CAP_SYS_ADMIN;
const Int32 OsConstants::_CAP_SYS_BOOT = CAP_SYS_BOOT;
const Int32 OsConstants::_CAP_SYS_CHROOT = CAP_SYS_CHROOT;
const Int32 OsConstants::_CAP_SYSLOG = CAP_SYSLOG;
const Int32 OsConstants::_CAP_SYS_MODULE = CAP_SYS_MODULE;
const Int32 OsConstants::_CAP_SYS_NICE = CAP_SYS_NICE;
const Int32 OsConstants::_CAP_SYS_PACCT = CAP_SYS_PACCT;
const Int32 OsConstants::_CAP_SYS_PTRACE = CAP_SYS_PTRACE;
const Int32 OsConstants::_CAP_SYS_RAWIO = CAP_SYS_RAWIO;
const Int32 OsConstants::_CAP_SYS_RESOURCE = CAP_SYS_RESOURCE;
const Int32 OsConstants::_CAP_SYS_TIME = CAP_SYS_TIME;
const Int32 OsConstants::_CAP_SYS_TTY_CONFIG = CAP_SYS_TTY_CONFIG;
const Int32 OsConstants::_CAP_WAKE_ALARM = CAP_WAKE_ALARM;
const Int32 OsConstants::_E2BIG = E2BIG;
#else
const Int32 OsConstants::_CAP_AUDIT_CONTROL = 0;
const Int32 OsConstants::_CAP_AUDIT_WRITE = 0;
const Int32 OsConstants::_CAP_BLOCK_SUSPEND = 0;
const Int32 OsConstants::_CAP_CHOWN = 0;
const Int32 OsConstants::_CAP_DAC_OVERRIDE = 0;
const Int32 OsConstants::_CAP_DAC_READ_SEARCH = 0;
const Int32 OsConstants::_CAP_FOWNER = 0;
const Int32 OsConstants::_CAP_FSETID = 0;
const Int32 OsConstants::_CAP_IPC_LOCK = 0;
const Int32 OsConstants::_CAP_IPC_OWNER = 0;
const Int32 OsConstants::_CAP_KILL = 0;
const Int32 OsConstants::_CAP_LAST_CAP = 0;
const Int32 OsConstants::_CAP_LEASE = 0;
const Int32 OsConstants::_CAP_LINUX_IMMUTABLE = 0;
const Int32 OsConstants::_CAP_MAC_ADMIN = 0;
const Int32 OsConstants::_CAP_MAC_OVERRIDE = 0;
const Int32 OsConstants::_CAP_MKNOD = 0;
const Int32 OsConstants::_CAP_NET_ADMIN = 0;
const Int32 OsConstants::_CAP_NET_BIND_SERVICE = 0;
const Int32 OsConstants::_CAP_NET_BROADCAST = 0;
const Int32 OsConstants::_CAP_NET_RAW = 0;
const Int32 OsConstants::_CAP_SETFCAP = 0;
const Int32 OsConstants::_CAP_SETGID = 0;
const Int32 OsConstants::_CAP_SETPCAP = 0;
const Int32 OsConstants::_CAP_SETUID = 0;
const Int32 OsConstants::_CAP_SYS_ADMIN = 0;
const Int32 OsConstants::_CAP_SYS_BOOT = 0;
const Int32 OsConstants::_CAP_SYS_CHROOT = 0;
const Int32 OsConstants::_CAP_SYSLOG = 0;
const Int32 OsConstants::_CAP_SYS_MODULE = 0;
const Int32 OsConstants::_CAP_SYS_NICE = 0;
const Int32 OsConstants::_CAP_SYS_PACCT = 0;
const Int32 OsConstants::_CAP_SYS_PTRACE = 0;
const Int32 OsConstants::_CAP_SYS_RAWIO = 0;
const Int32 OsConstants::_CAP_SYS_RESOURCE = 0;
const Int32 OsConstants::_CAP_SYS_TIME = 0;
const Int32 OsConstants::_CAP_SYS_TTY_CONFIG = 0;
const Int32 OsConstants::_CAP_WAKE_ALARM = 0;
const Int32 OsConstants::_E2BIG = 0;
#endif
const Int32 OsConstants::_EACCES = EACCES;
const Int32 OsConstants::_EADDRINUSE = EADDRINUSE;
const Int32 OsConstants::_EADDRNOTAVAIL = EADDRNOTAVAIL;
const Int32 OsConstants::_EAFNOSUPPORT = EAFNOSUPPORT;
const Int32 OsConstants::_EAGAIN = EAGAIN;
const Int32 OsConstants::_EAI_AGAIN = EAI_AGAIN;
const Int32 OsConstants::_EAI_BADFLAGS = EAI_BADFLAGS;
const Int32 OsConstants::_EAI_FAIL = EAI_FAIL;
const Int32 OsConstants::_EAI_FAMILY = EAI_FAMILY;
const Int32 OsConstants::_EAI_MEMORY = EAI_MEMORY;
const Int32 OsConstants::_EAI_NODATA = EAI_NODATA;
const Int32 OsConstants::_EAI_NONAME = EAI_NONAME;
#if defined(EAI_OVERFLOW)
const Int32 OsConstants::_EAI_OVERFLOW = EAI_OVERFLOW;
#else
const Int32 OsConstants::_EAI_OVERFLOW = 0;
#endif
const Int32 OsConstants::_EAI_SERVICE = EAI_SERVICE;
const Int32 OsConstants::_EAI_SOCKTYPE = EAI_SOCKTYPE;
const Int32 OsConstants::_EAI_SYSTEM = EAI_SYSTEM;
const Int32 OsConstants::_EALREADY = EALREADY;
const Int32 OsConstants::_EBADF = EBADF;
const Int32 OsConstants::_EBADMSG = EBADMSG;
const Int32 OsConstants::_EBUSY = EBUSY;
const Int32 OsConstants::_ECANCELED = ECANCELED;
const Int32 OsConstants::_ECHILD = ECHILD;
const Int32 OsConstants::_ECONNABORTED = ECONNABORTED;
const Int32 OsConstants::_ECONNREFUSED = ECONNREFUSED;
const Int32 OsConstants::_ECONNRESET = ECONNRESET;
const Int32 OsConstants::_EDEADLK = EDEADLK;
const Int32 OsConstants::_EDESTADDRREQ = EDESTADDRREQ;
const Int32 OsConstants::_EDOM = EDOM;
const Int32 OsConstants::_EDQUOT = EDQUOT;
const Int32 OsConstants::_EEXIST = EEXIST;
const Int32 OsConstants::_EFAULT = EFAULT;
const Int32 OsConstants::_EFBIG = EFBIG;
const Int32 OsConstants::_EHOSTUNREACH = EHOSTUNREACH;
const Int32 OsConstants::_EIDRM = EIDRM;
const Int32 OsConstants::_EILSEQ = EILSEQ;
const Int32 OsConstants::_EINPROGRESS = EINPROGRESS;
const Int32 OsConstants::_EINTR = EINTR;
const Int32 OsConstants::_EINVAL = EINVAL;
const Int32 OsConstants::_EIO = EIO;
const Int32 OsConstants::_EISCONN = EISCONN;
const Int32 OsConstants::_EISDIR = EISDIR;
const Int32 OsConstants::_ELOOP = ELOOP;
const Int32 OsConstants::_EMFILE = EMFILE;
const Int32 OsConstants::_EMLINK = EMLINK;
const Int32 OsConstants::_EMSGSIZE = EMSGSIZE;
const Int32 OsConstants::_EMULTIHOP = EMULTIHOP;
const Int32 OsConstants::_ENAMETOOLONG = ENAMETOOLONG;
const Int32 OsConstants::_ENETDOWN = ENETDOWN;
const Int32 OsConstants::_ENETRESET = ENETRESET;
const Int32 OsConstants::_ENETUNREACH = ENETUNREACH;
const Int32 OsConstants::_ENFILE = ENFILE;
const Int32 OsConstants::_ENOBUFS = ENOBUFS;
const Int32 OsConstants::_ENODATA = ENODATA;
const Int32 OsConstants::_ENODEV = ENODEV;
const Int32 OsConstants::_ENOENT = ENOENT;
const Int32 OsConstants::_ENOEXEC = ENOEXEC;
const Int32 OsConstants::_ENOLCK = ENOLCK;
const Int32 OsConstants::_ENOLINK = ENOLINK;
const Int32 OsConstants::_ENOMEM = ENOMEM;
const Int32 OsConstants::_ENOMSG = ENOMSG;
const Int32 OsConstants::_ENOPROTOOPT = ENOPROTOOPT;
const Int32 OsConstants::_ENOSPC = ENOSPC;
const Int32 OsConstants::_ENOSR = ENOSR;
const Int32 OsConstants::_ENOSTR = ENOSTR;
const Int32 OsConstants::_ENOSYS = ENOSYS;
const Int32 OsConstants::_ENOTCONN = ENOTCONN;
const Int32 OsConstants::_ENOTDIR = ENOTDIR;
const Int32 OsConstants::_ENOTEMPTY = ENOTEMPTY;
const Int32 OsConstants::_ENOTSOCK = ENOTSOCK;
const Int32 OsConstants::_ENOTSUP = ENOTSUP;
const Int32 OsConstants::_ENOTTY = ENOTTY;
const Int32 OsConstants::_ENXIO = ENXIO;
const Int32 OsConstants::_EOPNOTSUPP = EOPNOTSUPP;
const Int32 OsConstants::_EOVERFLOW = EOVERFLOW;
const Int32 OsConstants::_EPERM = EPERM;
const Int32 OsConstants::_EPIPE = EPIPE;
const Int32 OsConstants::_EPROTO = EPROTO;
const Int32 OsConstants::_EPROTONOSUPPORT = EPROTONOSUPPORT;
const Int32 OsConstants::_EPROTOTYPE = EPROTOTYPE;
const Int32 OsConstants::_ERANGE = ERANGE;
const Int32 OsConstants::_EROFS = EROFS;
const Int32 OsConstants::_ESPIPE = ESPIPE;
const Int32 OsConstants::_ESRCH = ESRCH;
const Int32 OsConstants::_ESTALE = ESTALE;
const Int32 OsConstants::_ETIME = ETIME;
const Int32 OsConstants::_ETIMEDOUT = ETIMEDOUT;
const Int32 OsConstants::_ETXTBSY = ETXTBSY;
#if EWOULDBLOCK != EAGAIN
#error EWOULDBLOCK != EAGAIN
#endif
    // On Linux, EWOULDBLOCK == EAGAIN. Use EAGAIN instead, to reduce confusion.
const Int32 OsConstants::_EXDEV = EXDEV;
const Int32 OsConstants::_EXIT_FAILURE = EXIT_FAILURE;
const Int32 OsConstants::_EXIT_SUCCESS = EXIT_SUCCESS;
const Int32 OsConstants::_FD_CLOEXEC = FD_CLOEXEC;
const Int32 OsConstants::_FIONREAD = FIONREAD;
const Int32 OsConstants::_F_DUPFD = F_DUPFD;
const Int32 OsConstants::_F_GETFD = F_GETFD;
const Int32 OsConstants::_F_GETFL = F_GETFL;
const Int32 OsConstants::_F_GETLK = F_GETLK;
#if defined(F_GETLK64)
const Int32 OsConstants::_F_GETLK64 = F_GETLK64;
#else
const Int32 OsConstants::_F_GETLK64 = 0;
#endif
const Int32 OsConstants::_F_GETOWN = F_GETOWN;
const Int32 OsConstants::_F_OK = F_OK;
const Int32 OsConstants::_F_RDLCK = F_RDLCK;
const Int32 OsConstants::_F_SETFD = F_SETFD;
const Int32 OsConstants::_F_SETFL = F_SETFL;
const Int32 OsConstants::_F_SETLK = F_SETLK;
#if defined(F_SETLK64)
const Int32 OsConstants::_F_SETLK64 = F_SETLK64;
#else
const Int32 OsConstants::_F_SETLK64 = 0;
#endif
const Int32 OsConstants::_F_SETLKW = F_SETLKW;
#if defined(F_SETLKW64)
const Int32 OsConstants::_F_SETLKW64 = F_SETLKW64;
#else
const Int32 OsConstants::_F_SETLKW64 = 0;
#endif
const Int32 OsConstants::_F_SETOWN = F_SETOWN;
const Int32 OsConstants::_F_UNLCK = F_UNLCK;
const Int32 OsConstants::_F_WRLCK = F_WRLCK;
#if defined(IFA_F_DADFAILED)
const Int32 OsConstants::_IFA_F_DADFAILED = IFA_F_DADFAILED;
#else
const Int32 OsConstants::_IFA_F_DADFAILED = 0;
#endif
#if defined(IFA_F_DEPRECATED)
const Int32 OsConstants::_IFA_F_DEPRECATED = IFA_F_DEPRECATED;
#else
const Int32 OsConstants::_IFA_F_DEPRECATED = 0;
#endif
#if defined(IFA_F_HOMEADDRESS)
const Int32 OsConstants::_IFA_F_HOMEADDRESS = IFA_F_HOMEADDRESS;
#else
const Int32 OsConstants::_IFA_F_HOMEADDRESS = 0;
#endif
#if defined(IFA_F_NODAD)
const Int32 OsConstants::_IFA_F_NODAD = IFA_F_NODAD;
#else
const Int32 OsConstants::_IFA_F_NODAD = 0;
#endif
#if defined(IFA_F_OPTIMISTIC)
const Int32 OsConstants::_IFA_F_OPTIMISTIC = IFA_F_OPTIMISTIC;
#else
const Int32 OsConstants::_IFA_F_OPTIMISTIC = 0;
#endif
#if defined(IFA_F_PERMANENT)
const Int32 OsConstants::_IFA_F_PERMANENT = IFA_F_PERMANENT;
#else
const Int32 OsConstants::_IFA_F_PERMANENT = 0;
#endif
#if defined(IFA_F_SECONDARY)
const Int32 OsConstants::_IFA_F_SECONDARY = IFA_F_SECONDARY;
#else
const Int32 OsConstants::_IFA_F_SECONDARY = 0;
#endif
#if defined(IFA_F_TEMPORARY)
const Int32 OsConstants::_IFA_F_TEMPORARY = IFA_F_TEMPORARY;
#else
const Int32 OsConstants::_IFA_F_TEMPORARY = 0;
#endif
#if defined(IFA_F_TENTATIVE)
const Int32 OsConstants::_IFA_F_TENTATIVE = IFA_F_TENTATIVE;
#else
const Int32 OsConstants::_IFA_F_TENTATIVE = 0;
#endif
const Int32 OsConstants::_IFF_ALLMULTI = IFF_ALLMULTI;
#if defined(IFF_AUTOMEDIA)
const Int32 OsConstants::_IFF_AUTOMEDIA = IFF_AUTOMEDIA;
#else
const Int32 OsConstants::_IFF_AUTOMEDIA = 0;
#endif
const Int32 OsConstants::_IFF_BROADCAST = IFF_BROADCAST;
const Int32 OsConstants::_IFF_DEBUG = IFF_DEBUG;
#if defined(IFF_DYNAMIC)
const Int32 OsConstants::_IFF_DYNAMIC = IFF_DYNAMIC;
#else
const Int32 OsConstants::_IFF_DYNAMIC = 0;
#endif
const Int32 OsConstants::_IFF_LOOPBACK = IFF_LOOPBACK;
#if defined(IFF_MASTER)
const Int32 OsConstants::_IFF_MASTER = IFF_MASTER;
#else
const Int32 OsConstants::_IFF_MASTER = 0;
#endif
const Int32 OsConstants::_IFF_MULTICAST = IFF_MULTICAST;
const Int32 OsConstants::_IFF_NOARP = IFF_NOARP;
const Int32 OsConstants::_IFF_NOTRAILERS = IFF_NOTRAILERS;
const Int32 OsConstants::_IFF_POINTOPOINT = IFF_POINTOPOINT;
#if defined(IFF_PORTSEL)
const Int32 OsConstants::_IFF_PORTSEL = IFF_PORTSEL;
#else
const Int32 OsConstants::_IFF_PORTSEL = 0;
#endif
const Int32 OsConstants::_IFF_PROMISC = IFF_PROMISC;
const Int32 OsConstants::_IFF_RUNNING = IFF_RUNNING;
#if defined(IFF_SLAVE)
const Int32 OsConstants::_IFF_SLAVE = IFF_SLAVE;
#else
const Int32 OsConstants::_IFF_SLAVE = 0;
#endif
const Int32 OsConstants::_IFF_UP = IFF_UP;
const Int32 OsConstants::_IPPROTO_ICMP = IPPROTO_ICMP;
const Int32 OsConstants::_IPPROTO_ICMPV6 = IPPROTO_ICMPV6;
const Int32 OsConstants::_IPPROTO_IP = IPPROTO_IP;
const Int32 OsConstants::_IPPROTO_IPV6 = IPPROTO_IPV6;
const Int32 OsConstants::_IPPROTO_RAW = IPPROTO_RAW;
const Int32 OsConstants::_IPPROTO_TCP = IPPROTO_TCP;
const Int32 OsConstants::_IPPROTO_UDP = IPPROTO_UDP;
const Int32 OsConstants::_IPV6_CHECKSUM = IPV6_CHECKSUM;
const Int32 OsConstants::_IPV6_MULTICAST_HOPS = IPV6_MULTICAST_HOPS;
const Int32 OsConstants::_IPV6_MULTICAST_IF = IPV6_MULTICAST_IF;
const Int32 OsConstants::_IPV6_MULTICAST_LOOP = IPV6_MULTICAST_LOOP;
#if defined(IPV6_RECVDSTOPTS)
const Int32 OsConstants::_IPV6_RECVDSTOPTS = IPV6_RECVDSTOPTS;
#else
const Int32 OsConstants::_IPV6_RECVDSTOPTS = 0;
#endif
#if defined(IPV6_RECVHOPLIMIT)
const Int32 OsConstants::_IPV6_RECVHOPLIMIT = IPV6_RECVHOPLIMIT;
#else
const Int32 OsConstants::_IPV6_RECVHOPLIMIT = 0;
#endif
#if defined(IPV6_RECVHOPOPTS)
const Int32 OsConstants::_IPV6_RECVHOPOPTS = IPV6_RECVHOPOPTS;
#else
const Int32 OsConstants::_IPV6_RECVHOPOPTS = 0;
#endif
#if defined(IPV6_RECVPKTINFO)
const Int32 OsConstants::_IPV6_RECVPKTINFO = IPV6_RECVPKTINFO;
#else
const Int32 OsConstants::_IPV6_RECVPKTINFO = 0;
#endif
#if defined(IPV6_RECVRTHDR)
const Int32 OsConstants::_IPV6_RECVRTHDR = IPV6_RECVRTHDR;
#else
const Int32 OsConstants::_IPV6_RECVRTHDR = 0;
#endif
#if defined(IPV6_RECVTCLASS)
const Int32 OsConstants::_IPV6_RECVTCLASS = IPV6_RECVTCLASS;
#else
const Int32 OsConstants::_IPV6_RECVTCLASS = 0;
#endif
#if defined(IPV6_TCLASS)
const Int32 OsConstants::_IPV6_TCLASS = IPV6_TCLASS;
#else
const Int32 OsConstants::_IPV6_TCLASS = 0;
#endif
const Int32 OsConstants::_IPV6_UNICAST_HOPS = IPV6_UNICAST_HOPS;
const Int32 OsConstants::_IPV6_V6ONLY = IPV6_V6ONLY;
const Int32 OsConstants::_IP_MULTICAST_IF = IP_MULTICAST_IF;
const Int32 OsConstants::_IP_MULTICAST_LOOP = IP_MULTICAST_LOOP;
const Int32 OsConstants::_IP_MULTICAST_TTL = IP_MULTICAST_TTL;
const Int32 OsConstants::_IP_TOS = IP_TOS;
const Int32 OsConstants::_IP_TTL = IP_TTL;
const Int32 OsConstants::_MAP_FIXED = MAP_FIXED;
const Int32 OsConstants::_MAP_PRIVATE = MAP_PRIVATE;
const Int32 OsConstants::_MAP_SHARED = MAP_SHARED;
#if defined(MCAST_JOIN_GROUP)
const Int32 OsConstants::_MCAST_JOIN_GROUP = MCAST_JOIN_GROUP;
#else
const Int32 OsConstants::_MCAST_JOIN_GROUP = 0;
#endif
#if defined(MCAST_LEAVE_GROUP)
const Int32 OsConstants::_MCAST_LEAVE_GROUP = MCAST_LEAVE_GROUP;
#else
const Int32 OsConstants::_MCAST_LEAVE_GROUP = 0;
#endif
#if defined(MCAST_JOIN_SOURCE_GROUP)
const Int32 OsConstants::_MCAST_JOIN_SOURCE_GROUP = MCAST_JOIN_SOURCE_GROUP;
#else
const Int32 OsConstants::_MCAST_JOIN_SOURCE_GROUP = 0;
#endif
#if defined(MCAST_LEAVE_SOURCE_GROUP)
const Int32 OsConstants::_MCAST_LEAVE_SOURCE_GROUP = MCAST_LEAVE_SOURCE_GROUP;
#else
const Int32 OsConstants::_MCAST_LEAVE_SOURCE_GROUP = 0;
#endif
#if defined(MCAST_BLOCK_SOURCE)
const Int32 OsConstants::_MCAST_BLOCK_SOURCE = MCAST_BLOCK_SOURCE;
#else
const Int32 OsConstants::_MCAST_BLOCK_SOURCE = 0;
#endif
#if defined(MCAST_UNBLOCK_SOURCE)
const Int32 OsConstants::_MCAST_UNBLOCK_SOURCE = MCAST_UNBLOCK_SOURCE;
#else
const Int32 OsConstants::_MCAST_UNBLOCK_SOURCE = 0;
#endif
const Int32 OsConstants::_MCL_CURRENT = MCL_CURRENT;
const Int32 OsConstants::_MCL_FUTURE = MCL_FUTURE;
const Int32 OsConstants::_MSG_CTRUNC = MSG_CTRUNC;
const Int32 OsConstants::_MSG_DONTROUTE = MSG_DONTROUTE;
const Int32 OsConstants::_MSG_EOR = MSG_EOR;
const Int32 OsConstants::_MSG_OOB = MSG_OOB;
const Int32 OsConstants::_MSG_PEEK = MSG_PEEK;
const Int32 OsConstants::_MSG_TRUNC = MSG_TRUNC;
const Int32 OsConstants::_MSG_WAITALL = MSG_WAITALL;
const Int32 OsConstants::_MS_ASYNC = MS_ASYNC;
const Int32 OsConstants::_MS_INVALIDATE = MS_INVALIDATE;
const Int32 OsConstants::_MS_SYNC = MS_SYNC;
const Int32 OsConstants::_NI_DGRAM = NI_DGRAM;
const Int32 OsConstants::_NI_NAMEREQD = NI_NAMEREQD;
const Int32 OsConstants::_NI_NOFQDN = NI_NOFQDN;
const Int32 OsConstants::_NI_NUMERICHOST = NI_NUMERICHOST;
const Int32 OsConstants::_NI_NUMERICSERV = NI_NUMERICSERV;
const Int32 OsConstants::_O_ACCMODE = O_ACCMODE;
const Int32 OsConstants::_O_APPEND = O_APPEND;
const Int32 OsConstants::_O_CREAT = O_CREAT;
const Int32 OsConstants::_O_EXCL = O_EXCL;
const Int32 OsConstants::_O_NOCTTY = O_NOCTTY;
const Int32 OsConstants::_O_NOFOLLOW = O_NOFOLLOW;
const Int32 OsConstants::_O_NONBLOCK = O_NONBLOCK;
const Int32 OsConstants::_O_RDONLY = O_RDONLY;
const Int32 OsConstants::_O_RDWR = O_RDWR;
const Int32 OsConstants::_O_SYNC = O_SYNC;
const Int32 OsConstants::_O_TRUNC = O_TRUNC;
const Int32 OsConstants::_O_WRONLY = O_WRONLY;
const Int32 OsConstants::_POLLERR = POLLERR;
const Int32 OsConstants::_POLLHUP = POLLHUP;
const Int32 OsConstants::_POLLIN = POLLIN;
const Int32 OsConstants::_POLLNVAL = POLLNVAL;
const Int32 OsConstants::_POLLOUT = POLLOUT;
const Int32 OsConstants::_POLLPRI = POLLPRI;
const Int32 OsConstants::_POLLRDBAND = POLLRDBAND;
const Int32 OsConstants::_POLLRDNORM = POLLRDNORM;
const Int32 OsConstants::_POLLWRBAND = POLLWRBAND;
const Int32 OsConstants::_POLLWRNORM = POLLWRNORM;
#if defined(PR_GET_DUMPABLE)
const Int32 OsConstants::_PR_GET_DUMPABLE = PR_GET_DUMPABLE;
#else
const Int32 OsConstants::_PR_GET_DUMPABLE = 0;
#endif
#if defined(PR_SET_DUMPABLE)
const Int32 OsConstants::_PR_SET_DUMPABLE = PR_SET_DUMPABLE;
#else
const Int32 OsConstants::_PR_SET_DUMPABLE = 0;
#endif
#if defined(PR_SET_NO_NEW_PRIVS)
const Int32 OsConstants::_PR_SET_NO_NEW_PRIVS = PR_SET_NO_NEW_PRIVS;
#else
const Int32 OsConstants::_PR_SET_NO_NEW_PRIVS = 0;
#endif
const Int32 OsConstants::_PROT_EXEC = PROT_EXEC;
const Int32 OsConstants::_PROT_NONE = PROT_NONE;
const Int32 OsConstants::_PROT_READ = PROT_READ;
const Int32 OsConstants::_PROT_WRITE = PROT_WRITE;
const Int32 OsConstants::_R_OK = R_OK;
#if !defined(__APPLE__)
const Int32 OsConstants::_RT_SCOPE_HOST = RT_SCOPE_HOST;
const Int32 OsConstants::_RT_SCOPE_LINK = RT_SCOPE_LINK;
const Int32 OsConstants::_RT_SCOPE_NOWHERE = RT_SCOPE_NOWHERE;
const Int32 OsConstants::_RT_SCOPE_SITE = RT_SCOPE_SITE;
const Int32 OsConstants::_RT_SCOPE_UNIVERSE = RT_SCOPE_UNIVERSE;
#else
const Int32 OsConstants::_RT_SCOPE_HOST = 0;
const Int32 OsConstants::_RT_SCOPE_LINK = 0;
const Int32 OsConstants::_RT_SCOPE_NOWHERE = 0;
const Int32 OsConstants::_RT_SCOPE_SITE = 0;
const Int32 OsConstants::_RT_SCOPE_UNIVERSE = 0;
#endif
const Int32 OsConstants::_SEEK_CUR = SEEK_CUR;
const Int32 OsConstants::_SEEK_END = SEEK_END;
const Int32 OsConstants::_SEEK_SET = SEEK_SET;
const Int32 OsConstants::_SHUT_RD = SHUT_RD;
const Int32 OsConstants::_SHUT_RDWR = SHUT_RDWR;
const Int32 OsConstants::_SHUT_WR = SHUT_WR;
const Int32 OsConstants::_SIGABRT = SIGABRT;
const Int32 OsConstants::_SIGALRM = SIGALRM;
const Int32 OsConstants::_SIGBUS = SIGBUS;
const Int32 OsConstants::_SIGCHLD = SIGCHLD;
const Int32 OsConstants::_SIGCONT = SIGCONT;
const Int32 OsConstants::_SIGFPE = SIGFPE;
const Int32 OsConstants::_SIGHUP = SIGHUP;
const Int32 OsConstants::_SIGILL = SIGILL;
const Int32 OsConstants::_SIGINT = SIGINT;
const Int32 OsConstants::_SIGIO = SIGIO;
const Int32 OsConstants::_SIGKILL = SIGKILL;
const Int32 OsConstants::_SIGPIPE = SIGPIPE;
const Int32 OsConstants::_SIGPROF = SIGPROF;
#if defined(SIGPWR)
const Int32 OsConstants::_SIGPWR = SIGPWR;
#else
const Int32 OsConstants::_SIGPWR = 0;
#endif
const Int32 OsConstants::_SIGQUIT = SIGQUIT;
#if defined(SIGRTMAX)
const Int32 OsConstants::_SIGRTMAX = SIGRTMAX;
#else
const Int32 OsConstants::_SIGRTMAX = 0;
#endif
#if defined(SIGRTMIN)
const Int32 OsConstants::_SIGRTMIN = SIGRTMIN;
#else
const Int32 OsConstants::_SIGRTMIN = 0;
#endif
const Int32 OsConstants::_SIGSEGV = SIGSEGV;
#if defined(SIGSTKFLT)
const Int32 OsConstants::_SIGSTKFLT = SIGSTKFLT;
#else
const Int32 OsConstants::_SIGSTKFLT = 0;
#endif
const Int32 OsConstants::_SIGSTOP = SIGSTOP;
const Int32 OsConstants::_SIGSYS = SIGSYS;
const Int32 OsConstants::_SIGTERM = SIGTERM;
const Int32 OsConstants::_SIGTRAP = SIGTRAP;
const Int32 OsConstants::_SIGTSTP = SIGTSTP;
const Int32 OsConstants::_SIGTTIN = SIGTTIN;
const Int32 OsConstants::_SIGTTOU = SIGTTOU;
const Int32 OsConstants::_SIGURG = SIGURG;
const Int32 OsConstants::_SIGUSR1 = SIGUSR1;
const Int32 OsConstants::_SIGUSR2 = SIGUSR2;
const Int32 OsConstants::_SIGVTALRM = SIGVTALRM;
const Int32 OsConstants::_SIGWINCH = SIGWINCH;
const Int32 OsConstants::_SIGXCPU = SIGXCPU;
const Int32 OsConstants::_SIGXFSZ = SIGXFSZ;
const Int32 OsConstants::_SIOCGIFADDR = SIOCGIFADDR;
const Int32 OsConstants::_SIOCGIFBRDADDR = SIOCGIFBRDADDR;
const Int32 OsConstants::_SIOCGIFDSTADDR = SIOCGIFDSTADDR;
const Int32 OsConstants::_SIOCGIFNETMASK = SIOCGIFNETMASK;
const Int32 OsConstants::_SOCK_DGRAM = SOCK_DGRAM;
const Int32 OsConstants::_SOCK_RAW = SOCK_RAW;
const Int32 OsConstants::_SOCK_SEQPACKET = SOCK_SEQPACKET;
const Int32 OsConstants::_SOCK_STREAM = SOCK_STREAM;
const Int32 OsConstants::_SOL_SOCKET = SOL_SOCKET;
#if defined(SO_BINDTODEVICE)
const Int32 OsConstants::_SO_BINDTODEVICE = SO_BINDTODEVICE;
#else
const Int32 OsConstants::_SO_BINDTODEVICE = 0;
#endif
const Int32 OsConstants::_SO_BROADCAST = SO_BROADCAST;
const Int32 OsConstants::_SO_DEBUG = SO_DEBUG;
const Int32 OsConstants::_SO_DONTROUTE = SO_DONTROUTE;
const Int32 OsConstants::_SO_ERROR = SO_ERROR;
const Int32 OsConstants::_SO_KEEPALIVE = SO_KEEPALIVE;
const Int32 OsConstants::_SO_LINGER = SO_LINGER;
const Int32 OsConstants::_SO_OOBINLINE = SO_OOBINLINE;
#if defined(SO_PASSCRED)
const Int32 OsConstants::_SO_PASSCRED = SO_PASSCRED;
#else
const Int32 OsConstants::_SO_PASSCRED = 0;
#endif
#if defined(SO_PEERCRED)
const Int32 OsConstants::_SO_PEERCRED = SO_PEERCRED;
#else
const Int32 OsConstants::_SO_PEERCRED = 0;
#endif
const Int32 OsConstants::_SO_RCVBUF = SO_RCVBUF;
const Int32 OsConstants::_SO_RCVLOWAT = SO_RCVLOWAT;
const Int32 OsConstants::_SO_RCVTIMEO = SO_RCVTIMEO;
const Int32 OsConstants::_SO_REUSEADDR = SO_REUSEADDR;
const Int32 OsConstants::_SO_SNDBUF = SO_SNDBUF;
const Int32 OsConstants::_SO_SNDLOWAT = SO_SNDLOWAT;
const Int32 OsConstants::_SO_SNDTIMEO = SO_SNDTIMEO;
const Int32 OsConstants::_SO_TYPE = SO_TYPE;
const Int32 OsConstants::_STDERR_FILENO = STDERR_FILENO;
const Int32 OsConstants::_STDIN_FILENO = STDIN_FILENO;
const Int32 OsConstants::_STDOUT_FILENO = STDOUT_FILENO;
const Int32 OsConstants::_S_IFBLK = S_IFBLK;
const Int32 OsConstants::_S_IFCHR = S_IFCHR;
const Int32 OsConstants::_S_IFDIR = S_IFDIR;
const Int32 OsConstants::_S_IFIFO = S_IFIFO;
const Int32 OsConstants::_S_IFLNK = S_IFLNK;
const Int32 OsConstants::_S_IFMT = S_IFMT;
const Int32 OsConstants::_S_IFREG = S_IFREG;
const Int32 OsConstants::_S_IFSOCK = S_IFSOCK;
const Int32 OsConstants::_S_IRGRP = S_IRGRP;
const Int32 OsConstants::_S_IROTH = S_IROTH;
const Int32 OsConstants::_S_IRUSR = S_IRUSR;
const Int32 OsConstants::_S_IRWXG = S_IRWXG;
const Int32 OsConstants::_S_IRWXO = S_IRWXO;
const Int32 OsConstants::_S_IRWXU = S_IRWXU;
const Int32 OsConstants::_S_ISGID = S_ISGID;
const Int32 OsConstants::_S_ISUID = S_ISUID;
const Int32 OsConstants::_S_ISVTX = S_ISVTX;
const Int32 OsConstants::_S_IWGRP = S_IWGRP;
const Int32 OsConstants::_S_IWOTH = S_IWOTH;
const Int32 OsConstants::_S_IWUSR = S_IWUSR;
const Int32 OsConstants::_S_IXGRP = S_IXGRP;
const Int32 OsConstants::_S_IXOTH = S_IXOTH;
const Int32 OsConstants::_S_IXUSR = S_IXUSR;
const Int32 OsConstants::_TCP_NODELAY = TCP_NODELAY;
const Int32 OsConstants::_WCONTINUED = WCONTINUED;
const Int32 OsConstants::_WEXITED = WEXITED;
const Int32 OsConstants::_WNOHANG = WNOHANG;
const Int32 OsConstants::_WNOWAIT = WNOWAIT;
const Int32 OsConstants::_WSTOPPED = WSTOPPED;
const Int32 OsConstants::_WUNTRACED = WUNTRACED;
const Int32 OsConstants::_W_OK = W_OK;
const Int32 OsConstants::_X_OK = X_OK;
const Int32 OsConstants::__SC_2_CHAR_TERM = _SC_2_CHAR_TERM;
const Int32 OsConstants::__SC_2_C_BIND = _SC_2_C_BIND;
const Int32 OsConstants::__SC_2_C_DEV = _SC_2_C_DEV;
#if defined(_SC_2_C_VERSION)
const Int32 OsConstants::__SC_2_C_VERSION = _SC_2_C_VERSION;
#else
const Int32 OsConstants::__SC_2_C_VERSION = 0;
#endif
const Int32 OsConstants::__SC_2_FORT_DEV = _SC_2_FORT_DEV;
const Int32 OsConstants::__SC_2_FORT_RUN = _SC_2_FORT_RUN;
const Int32 OsConstants::__SC_2_LOCALEDEF = _SC_2_LOCALEDEF;
const Int32 OsConstants::__SC_2_SW_DEV = _SC_2_SW_DEV;
const Int32 OsConstants::__SC_2_UPE = _SC_2_UPE;
const Int32 OsConstants::__SC_2_VERSION = _SC_2_VERSION;
const Int32 OsConstants::__SC_AIO_LISTIO_MAX = _SC_AIO_LISTIO_MAX;
const Int32 OsConstants::__SC_AIO_MAX = _SC_AIO_MAX;
const Int32 OsConstants::__SC_AIO_PRIO_DELTA_MAX = _SC_AIO_PRIO_DELTA_MAX;
const Int32 OsConstants::__SC_ARG_MAX = _SC_ARG_MAX;
const Int32 OsConstants::__SC_ASYNCHRONOUS_IO = _SC_ASYNCHRONOUS_IO;
const Int32 OsConstants::__SC_ATEXIT_MAX = _SC_ATEXIT_MAX;
#if defined(_SC_AVPHYS_PAGES)
const Int32 OsConstants::__SC_AVPHYS_PAGES = _SC_AVPHYS_PAGES;
#else
const Int32 OsConstants::__SC_AVPHYS_PAGES = 0;
#endif
const Int32 OsConstants::__SC_BC_BASE_MAX = _SC_BC_BASE_MAX;
const Int32 OsConstants::__SC_BC_DIM_MAX = _SC_BC_DIM_MAX;
const Int32 OsConstants::__SC_BC_SCALE_MAX = _SC_BC_SCALE_MAX;
const Int32 OsConstants::__SC_BC_STRING_MAX = _SC_BC_STRING_MAX;
const Int32 OsConstants::__SC_CHILD_MAX = _SC_CHILD_MAX;
const Int32 OsConstants::__SC_CLK_TCK = _SC_CLK_TCK;
const Int32 OsConstants::__SC_COLL_WEIGHTS_MAX = _SC_COLL_WEIGHTS_MAX;
const Int32 OsConstants::__SC_DELAYTIMER_MAX = _SC_DELAYTIMER_MAX;
const Int32 OsConstants::__SC_EXPR_NEST_MAX = _SC_EXPR_NEST_MAX;
const Int32 OsConstants::__SC_FSYNC = _SC_FSYNC;
const Int32 OsConstants::__SC_GETGR_R_SIZE_MAX = _SC_GETGR_R_SIZE_MAX;
const Int32 OsConstants::__SC_GETPW_R_SIZE_MAX = _SC_GETPW_R_SIZE_MAX;
const Int32 OsConstants::__SC_IOV_MAX = _SC_IOV_MAX;
const Int32 OsConstants::__SC_JOB_CONTROL = _SC_JOB_CONTROL;
const Int32 OsConstants::__SC_LINE_MAX = _SC_LINE_MAX;
const Int32 OsConstants::__SC_LOGIN_NAME_MAX = _SC_LOGIN_NAME_MAX;
const Int32 OsConstants::__SC_MAPPED_FILES = _SC_MAPPED_FILES;
const Int32 OsConstants::__SC_MEMLOCK = _SC_MEMLOCK;
const Int32 OsConstants::__SC_MEMLOCK_RANGE = _SC_MEMLOCK_RANGE;
const Int32 OsConstants::__SC_MEMORY_PROTECTION = _SC_MEMORY_PROTECTION;
const Int32 OsConstants::__SC_MESSAGE_PASSING = _SC_MESSAGE_PASSING;
const Int32 OsConstants::__SC_MQ_OPEN_MAX = _SC_MQ_OPEN_MAX;
const Int32 OsConstants::__SC_MQ_PRIO_MAX = _SC_MQ_PRIO_MAX;
const Int32 OsConstants::__SC_NGROUPS_MAX = _SC_NGROUPS_MAX;
const Int32 OsConstants::__SC_NPROCESSORS_CONF = _SC_NPROCESSORS_CONF;
const Int32 OsConstants::__SC_NPROCESSORS_ONLN = _SC_NPROCESSORS_ONLN;
const Int32 OsConstants::__SC_OPEN_MAX = _SC_OPEN_MAX;
const Int32 OsConstants::__SC_PAGESIZE = _SC_PAGESIZE;
const Int32 OsConstants::__SC_PAGE_SIZE = _SC_PAGE_SIZE;
const Int32 OsConstants::__SC_PASS_MAX = _SC_PASS_MAX;
#if defined(_SC_PHYS_PAGES)
const Int32 OsConstants::__SC_PHYS_PAGES = _SC_PHYS_PAGES;
#else
const Int32 OsConstants::__SC_PHYS_PAGES = 0;
#endif
const Int32 OsConstants::__SC_PRIORITIZED_IO = _SC_PRIORITIZED_IO;
const Int32 OsConstants::__SC_PRIORITY_SCHEDULING = _SC_PRIORITY_SCHEDULING;
const Int32 OsConstants::__SC_REALTIME_SIGNALS = _SC_REALTIME_SIGNALS;
const Int32 OsConstants::__SC_RE_DUP_MAX = _SC_RE_DUP_MAX;
const Int32 OsConstants::__SC_RTSIG_MAX = _SC_RTSIG_MAX;
const Int32 OsConstants::__SC_SAVED_IDS = _SC_SAVED_IDS;
const Int32 OsConstants::__SC_SEMAPHORES = _SC_SEMAPHORES;
const Int32 OsConstants::__SC_SEM_NSEMS_MAX = _SC_SEM_NSEMS_MAX;
const Int32 OsConstants::__SC_SEM_VALUE_MAX = _SC_SEM_VALUE_MAX;
const Int32 OsConstants::__SC_SHARED_MEMORY_OBJECTS = _SC_SHARED_MEMORY_OBJECTS;
const Int32 OsConstants::__SC_SIGQUEUE_MAX = _SC_SIGQUEUE_MAX;
const Int32 OsConstants::__SC_STREAM_MAX = _SC_STREAM_MAX;
const Int32 OsConstants::__SC_SYNCHRONIZED_IO = _SC_SYNCHRONIZED_IO;
const Int32 OsConstants::__SC_THREADS = _SC_THREADS;
const Int32 OsConstants::__SC_THREAD_ATTR_STACKADDR = _SC_THREAD_ATTR_STACKADDR;
const Int32 OsConstants::__SC_THREAD_ATTR_STACKSIZE = _SC_THREAD_ATTR_STACKSIZE;
const Int32 OsConstants::__SC_THREAD_DESTRUCTOR_ITERATIONS = _SC_THREAD_DESTRUCTOR_ITERATIONS;
const Int32 OsConstants::__SC_THREAD_KEYS_MAX = _SC_THREAD_KEYS_MAX;
const Int32 OsConstants::__SC_THREAD_PRIORITY_SCHEDULING = _SC_THREAD_PRIORITY_SCHEDULING;
const Int32 OsConstants::__SC_THREAD_PRIO_INHERIT = _SC_THREAD_PRIO_INHERIT;
const Int32 OsConstants::__SC_THREAD_PRIO_PROTECT = _SC_THREAD_PRIO_PROTECT;
const Int32 OsConstants::__SC_THREAD_SAFE_FUNCTIONS = _SC_THREAD_SAFE_FUNCTIONS;
const Int32 OsConstants::__SC_THREAD_STACK_MIN = _SC_THREAD_STACK_MIN;
const Int32 OsConstants::__SC_THREAD_THREADS_MAX = _SC_THREAD_THREADS_MAX;
const Int32 OsConstants::__SC_TIMERS = _SC_TIMERS;
const Int32 OsConstants::__SC_TIMER_MAX = _SC_TIMER_MAX;
const Int32 OsConstants::__SC_TTY_NAME_MAX = _SC_TTY_NAME_MAX;
const Int32 OsConstants::__SC_TZNAME_MAX = _SC_TZNAME_MAX;
const Int32 OsConstants::__SC_VERSION = _SC_VERSION;
const Int32 OsConstants::__SC_XBS5_ILP32_OFF32 = _SC_XBS5_ILP32_OFF32;
const Int32 OsConstants::__SC_XBS5_ILP32_OFFBIG = _SC_XBS5_ILP32_OFFBIG;
const Int32 OsConstants::__SC_XBS5_LP64_OFF64 = _SC_XBS5_LP64_OFF64;
const Int32 OsConstants::__SC_XBS5_LPBIG_OFFBIG = _SC_XBS5_LPBIG_OFFBIG;
const Int32 OsConstants::__SC_XOPEN_CRYPT = _SC_XOPEN_CRYPT;
const Int32 OsConstants::__SC_XOPEN_ENH_I18N = _SC_XOPEN_ENH_I18N;
const Int32 OsConstants::__SC_XOPEN_LEGACY = _SC_XOPEN_LEGACY;
const Int32 OsConstants::__SC_XOPEN_REALTIME = _SC_XOPEN_REALTIME;
const Int32 OsConstants::__SC_XOPEN_REALTIME_THREADS = _SC_XOPEN_REALTIME_THREADS;
const Int32 OsConstants::__SC_XOPEN_SHM = _SC_XOPEN_SHM;
const Int32 OsConstants::__SC_XOPEN_UNIX = _SC_XOPEN_UNIX;
const Int32 OsConstants::__SC_XOPEN_VERSION = _SC_XOPEN_VERSION;
const Int32 OsConstants::__SC_XOPEN_XCU_VERSION = _SC_XOPEN_XCU_VERSION;

    /**
 * Tests whether the given mode is a block device.
 */
Boolean OsConstants::S_IsBLK(
    /* [in] */ Int32 mode)
{
    return (mode & _S_IFMT) == _S_IFBLK;
}

/**
 * Tests whether the given mode is a character device.
 */
Boolean OsConstants::S_IsCHR(
    /* [in] */ Int32 mode)
{
    return (mode & _S_IFMT) == _S_IFCHR;;
}

/**
 * Tests whether the given mode is a directory.
 */
Boolean OsConstants::S_IsDIR(
    /* [in] */ Int32 mode)
{
    return (mode & _S_IFMT) == _S_IFDIR;;
}

/**
 * Tests whether the given mode is a FIFO.
 */
Boolean OsConstants::S_IsFIFO(
    /* [in] */ Int32 mode)
{
    return (mode & _S_IFMT) == _S_IFIFO;
}

/**
 * Tests whether the given mode is a regular file.
 */
Boolean OsConstants::S_IsREG(
    /* [in] */ Int32 mode)
{
    return (mode & _S_IFMT) == _S_IFREG;;
}

/**
 * Tests whether the given mode is a symbolic link.
 */
Boolean OsConstants::S_IsLNK(
    /* [in] */ Int32 mode)
{
    return (mode & _S_IFMT) == _S_IFLNK;;
}
/**
 * Tests whether the given mode is a socket.
 */
Boolean OsConstants::S_IsSOCK(
    /* [in] */ Int32 mode)
{
    return (mode & _S_IFMT) == _S_IFSOCK;;
}

/**
 * Extracts the exit status of a child. Only valid if WIFEXITED returns true.
 */
Int32 OsConstants::WExitStatus(
    /* [in] */ Int32 status)
{
    return (status & 0xff00) >> 8;
}
/**
 * Tests whether the child dumped core. Only valid if WIFSIGNALED returns true.
 */
Boolean OsConstants::WCoreDump(
    /* [in] */ Int32 status)
{
    return (status & 0x80) != 0;;
}

/**
 * Returns the signal that caused the child to exit. Only valid if WIFSIGNALED returns true.
 */
Int32 OsConstants::WTermSig(
    /* [in] */ Int32 status)
{
    return status & 0x7f;;
}

/**
 * Returns the signal that cause the child to stop. Only valid if WIFSTOPPED returns true.
 */
Int32 OsConstants::WStopSig(
    /* [in] */ Int32 status)
{
    return WExitStatus(status);
}

/**
 * Tests whether the child exited normally.
 */
Boolean OsConstants::WIfExited(
    /* [in] */ Int32 status)
{
    return (WTermSig(status) == 0); ;
}

/**
 * Tests whether the child was stopped (not terminated) by a signal.
 */
Boolean OsConstants::WIfStopped(
    /* [in] */ Int32 status)
{
    return (WTermSig(status) == 0x7f);;
}

/**
 * Tests whether the child was terminated by a signal.
 */
Boolean OsConstants::WIfSignaled(
    /* [in] */ Int32 status)
{
    return (WTermSig(status + 1) >= 2);;
}

/**
 * Returns the string name of a getaddrinfo(3) error value.
 * For example, "EAI_AGAIN".
 */
String OsConstants::GaiName(
    /* [in] */ Int32 error)
{
    if (error == _EAI_AGAIN) {
        return String("EAI_AGAIN");
    }
    if (error == _EAI_BADFLAGS) {
        return String("EAI_BADFLAGS");
    }
    if (error == _EAI_FAIL) {
        return String("EAI_FAIL");
    }
    if (error == _EAI_FAMILY) {
        return String("EAI_FAMILY");
    }
    if (error == _EAI_MEMORY) {
        return String("EAI_MEMORY");
    }
    if (error == _EAI_NODATA) {
        return String("EAI_NODATA");
    }
    if (error == _EAI_NONAME) {
        return String("EAI_NONAME");
    }
    if (error == _EAI_OVERFLOW) {
        return String("EAI_OVERFLOW");
    }
    if (error == _EAI_SERVICE) {
        return String("EAI_SERVICE");
    }
    if (error == _EAI_SOCKTYPE) {
        return String("EAI_SOCKTYPE");
    }
    if (error == _EAI_SYSTEM) {
        return String("EAI_SYSTEM");
    }
    return String(NULL);
}
    /**
 * Returns the string name of an error value.
 * For example, "EACCES". See {@link Os#strerror} for human-readable error descriptions.
 */
String OsConstants::GetErrnoName(
    /* [in] */ Int32 error)
{
    if (error == _E2BIG) {
        return String("E2BIG");
    }
    if (error == _EACCES) {
        return String("EACCES");
    }
    if (error == _EADDRINUSE) {
        return String("EADDRINUSE");
    }
    if (error == _EADDRNOTAVAIL) {
        return String("EADDRNOTAVAIL");
    }
    if (error == _EAFNOSUPPORT) {
        return String("EAFNOSUPPORT");
    }
    if (error == _EAGAIN) {
        return String("EAGAIN");
    }
    if (error == _EALREADY) {
        return String("EALREADY");
    }
    if (error == _EBADF) {
        return String("EBADF");
    }
    if (error == _EBADMSG) {
        return String("EBADMSG");
    }
    if (error == _EBUSY) {
        return String("EBUSY");
    }
    if (error == _ECANCELED) {
        return String("ECANCELED");
    }
    if (error == _ECHILD) {
        return String("ECHILD");
    }
    if (error == _ECONNABORTED) {
        return String("ECONNABORTED");
    }
    if (error == _ECONNREFUSED) {
        return String("ECONNREFUSED");
    }
    if (error == _ECONNRESET) {
        return String("ECONNRESET");
    }
    if (error == _EDEADLK) {
        return String("EDEADLK");
    }
    if (error == _EDESTADDRREQ) {
        return String("EDESTADDRREQ");
    }
    if (error == _EDOM) {
        return String("EDOM");
    }
    if (error == _EDQUOT) {
        return String("EDQUOT");
    }
    if (error == _EEXIST) {
        return String("EEXIST");
    }
    if (error == _EFAULT) {
        return String("EFAULT");
    }
    if (error == _EFBIG) {
        return String("EFBIG");
    }
    if (error == _EHOSTUNREACH) {
        return String("EHOSTUNREACH");
    }
    if (error == _EIDRM) {
        return String("EIDRM");
    }
    if (error == _EILSEQ) {
        return String("EILSEQ");
    }
    if (error == _EINPROGRESS) {
        return String("EINPROGRESS");
    }
    if (error == _EINTR) {
        return String("EINTR");
    }
    if (error == _EINVAL) {
        return String("EINVAL");
    }
    if (error == _EIO) {
        return String("EIO");
    }
    if (error == _EISCONN) {
        return String("EISCONN");
    }
    if (error == _EISDIR) {
        return String("EISDIR");
    }
    if (error == _ELOOP) {
        return String("ELOOP");
    }
    if (error == _EMFILE) {
        return String("EMFILE");
    }
    if (error == _EMLINK) {
        return String("EMLINK");
    }
    if (error == _EMSGSIZE) {
        return String("EMSGSIZE");
    }
    if (error == _EMULTIHOP) {
        return String("EMULTIHOP");
    }
    if (error == _ENAMETOOLONG) {
        return String("ENAMETOOLONG");
    }
    if (error == _ENETDOWN) {
        return String("ENETDOWN");
    }
    if (error == _ENETRESET) {
        return String("ENETRESET");
    }
    if (error == _ENETUNREACH) {
        return String("ENETUNREACH");
    }
    if (error == _ENFILE) {
        return String("ENFILE");
    }
    if (error == _ENOBUFS) {
        return String("ENOBUFS");
    }
    if (error == _ENODATA) {
        return String("ENODATA");
    }
    if (error == _ENODEV) {
        return String("ENODEV");
    }
    if (error == _ENOENT) {
        return String("ENOENT");
    }
    if (error == _ENOEXEC) {
        return String("ENOEXEC");
    }
    if (error == _ENOLCK) {
        return String("ENOLCK");
    }
    if (error == _ENOLINK) {
        return String("ENOLINK");
    }
    if (error == _ENOMEM) {
        return String("ENOMEM");
    }
    if (error == _ENOMSG) {
        return String("ENOMSG");
    }
    if (error == _ENOPROTOOPT) {
        return String("ENOPROTOOPT");
    }
    if (error == _ENOSPC) {
        return String("ENOSPC");
    }
    if (error == _ENOSR) {
        return String("ENOSR");
    }
    if (error == _ENOSTR) {
        return String("ENOSTR");
    }
    if (error == _ENOSYS) {
        return String("ENOSYS");
    }
    if (error == _ENOTCONN) {
        return String("ENOTCONN");
    }
    if (error == _ENOTDIR) {
        return String("ENOTDIR");
    }
    if (error == _ENOTEMPTY) {
        return String("ENOTEMPTY");
    }
    if (error == _ENOTSOCK) {
        return String("ENOTSOCK");
    }
    if (error == _ENOTSUP) {
        return String("ENOTSUP");
    }
    if (error == _ENOTTY) {
        return String("ENOTTY");
    }
    if (error == _ENXIO) {
        return String("ENXIO");
    }
    if (error == _EOPNOTSUPP) {
        return String("EOPNOTSUPP");
    }
    if (error == _EOVERFLOW) {
        return String("EOVERFLOW");
    }
    if (error == _EPERM) {
        return String("EPERM");
    }
    if (error == _EPIPE) {
        return String("EPIPE");
    }
    if (error == _EPROTO) {
        return String("EPROTO");
    }
    if (error == _EPROTONOSUPPORT) {
        return String("EPROTONOSUPPORT");
    }
    if (error == _EPROTOTYPE) {
        return String("EPROTOTYPE");
    }
    if (error == _ERANGE) {
        return String("ERANGE");
    }
    if (error == _EROFS) {
        return String("EROFS");
    }
    if (error == _ESPIPE) {
        return String("ESPIPE");
    }
    if (error == _ESRCH) {
        return String("ESRCH");
    }
    if (error == _ESTALE) {
        return String("ESTALE");
    }
    if (error == _ETIME) {
        return String("ETIME");
    }
    if (error == _ETIMEDOUT) {
        return String("ETIMEDOUT");
    }
    if (error == _ETXTBSY) {
        return String("ETXTBSY");
    }
    if (error == _EXDEV) {
        return String("EXDEV");
    }
    return String(NULL);
}

}// namespace System
}// namespace Droid
}// namespace Elastos

