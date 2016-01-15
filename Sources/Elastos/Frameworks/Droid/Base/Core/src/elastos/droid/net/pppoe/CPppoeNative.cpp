
#include "CPppoeNative.h"
#include "elastos/droid/ext/frameworkext.h"
#include "jni.h"
#include <inttypes.h>
#include <utils/misc.h>
//#include <android_runtime/AndroidRuntime.h>
#include <utils/Log.h>
#include <asm/types.h>

/*
The statement
    typedef unsigned short sa_family_t;
is removed from bionic/libc/kernel/common/linux/socket.h.
To avoid compile failure, add the statement here.
*/
typedef unsigned short sa_family_t;
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <net/if_arp.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

#define PPPOE_PKG_NAME "android/net/pppoe/PppoeNative"
const String CPppoeNative::TAG("CPppoeNative");

static struct fieldIds {
    jclass dhcpInfoClass;
    jmethodID constructorId;
    jfieldID ipaddress;
    jfieldID gateway;
    jfieldID netmask;
    jfieldID dns1;
    jfieldID dns2;
    jfieldID serverAddress;
    jfieldID leaseDuration;
} dhcpInfoFieldIds;


typedef struct _interface_info_t {
    unsigned int i;                            /* interface index        */
    char *name;                       /* name (ppp0, ppp1, ...) */
    struct _interface_info_t *next;
} interface_info_t;


interface_info_t *pppoe_if_list = NULL;
interface_info_t *pppoe_if_list_old = NULL;
static int nr_pppoe_if = 0;


#define RET_STR_SZ       4096
#define NL_POLL_MSG_SZ   8*1024
#define SYSFS_PATH_MAX   256

static const char SYSFS_CLASS_NET[]     = "/sys/class/net";
static struct sockaddr_nl addr_msg;
static int nl_sk = -1;
static struct sockaddr_nl addr_poll;

static char flags_desc[256];

static char * flag_desc_tbl[32] = {
        "UP",
        "BC",
        "DBG",
        "LOOPBACK",

        "PPP",
        "NT",
        "RUNNING",
        "NOARP",

        "PROMISC",
        "ALLMULTI",
        "MASTER",
        "SLAVE",

        "MC",
        "PORTSEL",
        "AUTOMEDIA",
        "DYNAMIC",

        "LINK_UP",
        "DORMANT",
        "UNDEFINED",
        "UNDEFINED",

        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",

        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",
        "UNDEFINED",

    };

static char* if_flags_desc(int flags, char *desc);

static int getinterfacename(int index, char *name, size_t len);
static void free_pppoe_list();
static void free_pppoe_list_old();
static void backup_pppoe_list() ;
static char * find_removed_pppoe_if() ;
static int netlink_init_pppoe_list(void);

char* if_flags_desc(int flags, char *desc) {
    desc[0] = '\0';
    for (int i = 0; i< 18; i++) {
        if (flags & (1<<i)) {
            strcat(desc, " ");
            strcat(desc, flag_desc_tbl[i]);
        }
    }

    return desc;
}

static interface_info_t *find_info_by_index(unsigned int index)
{
    interface_info_t *info = pppoe_if_list;

    while( info) {
        info = info->next;
    }

    info = pppoe_if_list;
    while( info) {
        if (info->i == index)
            return info;
        info = info->next;
    }
    return NULL;
}

ECode CPppoeNative::WaitForEvent(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    char *buff;
    struct nlmsghdr *nh;
    struct ifinfomsg *einfo;
    struct iovec iov;
    struct msghdr msg;
    char *result = NULL;
    char rbuf[4096];
    rbuf[0] = '\0';
    unsigned int left = 4096;
    interface_info_t *info;
    int len;
    int last_total_cnt;

    /*
     *wait on uevent netlink socket for the pppoe device
     */
    buff = (char *)malloc(NL_POLL_MSG_SZ);
    if (!buff) {
        ALOGE("Allocate poll buffer failed");
        goto error;
    }

    iov.iov_base = buff;
    iov.iov_len = NL_POLL_MSG_SZ;
    msg.msg_name = (void *)&addr_msg;
    msg.msg_namelen =  sizeof(addr_msg);
    msg.msg_iov =  &iov;
    msg.msg_iovlen =  1;
    msg.msg_control =  NULL;
    msg.msg_controllen =  0;
    msg.msg_flags =  0;
    last_total_cnt = nr_pppoe_if;

    if((len = recvmsg(nl_sk, &msg, 0))>= 0) {
        backup_pppoe_list();
        free_pppoe_list();
        netlink_init_pppoe_list();
        result = rbuf;

        for (nh = (struct nlmsghdr *) buff; NLMSG_OK (nh, (unsigned)len);
             nh = NLMSG_NEXT (nh, len))
        {

            if (nh->nlmsg_type == NLMSG_DONE){
                ALOGE("Did not find useful pppoe interface information");
                goto error;
            }

            if (nh->nlmsg_type == NLMSG_ERROR){

                /* Do some error handling. */
                ALOGE("Read device name failed");
                goto error;
            }

            einfo = (struct ifinfomsg *)NLMSG_DATA(nh);
            if (nh->nlmsg_type == RTM_DELLINK ||
                nh->nlmsg_type == RTM_NEWLINK ||
                nh->nlmsg_type == RTM_DELADDR ||
                nh->nlmsg_type == RTM_NEWADDR) {
                int type = nh->nlmsg_type;

                if (type == RTM_NEWLINK &&
                    (!(einfo->ifi_flags & IFF_LOWER_UP))) {
                    ALOGI("For NETLINK bug, RTM_NEWLINK ==> RTM_DELLINK");
                    type = RTM_DELLINK;
                }

                const char *desc = (type == RTM_DELLINK) ? "DELLINK" :
                         ((type == RTM_NEWLINK) ? "NEWLINK":
                         ((type == RTM_DELADDR) ? "DELADDR" : "NEWADDR"));

                if (einfo->ifi_flags & IFF_LOOPBACK){
                    ALOGI("Event(%s) from LOOP interface, ignore it", desc);
                    continue;
                }

                if (!(einfo->ifi_flags & IFF_POINTOPOINT)){
                    ALOGI("Event(%s) NOT from PPP interface, ignore it", desc);
                    continue;
                }

                if (nr_pppoe_if > last_total_cnt) {
                    ALOGI("Had device add: last = %d; now = %d", last_total_cnt, nr_pppoe_if);
                }

                ALOGI("event :%s(%d), flags=0X%X",
                    desc, nh->nlmsg_type, einfo->ifi_flags);
                ALOGI("flags:%s", if_flags_desc(einfo->ifi_flags, flags_desc));

                if ((info = find_info_by_index
                      (((struct ifinfomsg*) NLMSG_DATA(nh))->ifi_index)) != NULL)
                    snprintf(result,left, "%s:%d:",info->name,type);
                else if (type == RTM_DELLINK){
                    /*when link removed, we cann`t find it from pppoe_if_list, try
                    get it by compare pppoe_if_list and pppoe_if_list_old to find it*/
                    snprintf(result,left, "%s:%d:",find_removed_pppoe_if(),type);
                }
                left = left - strlen(result);
                result =(char *)(result+ strlen(result));
            }

        }

        rbuf[4096 - left] = '\0';
        if ( left != 4096 )
            ALOGI("poll state :%s",rbuf);
    }
    else {
        ALOGI("recvmsg failed %s, left = %d",strerror(errno), left);
    }


error:
    *str = String(4096 - left > 0 ? rbuf : NULL);
    if (buff) {
        free(buff);
    }
    return NOERROR;
}

static void free_pppoe_list()
{
    interface_info_t *tmp = pppoe_if_list;
    while(tmp) {
        if (tmp->name) free(tmp->name);
        pppoe_if_list = tmp->next;
        free(tmp);
        tmp = pppoe_if_list;
        nr_pppoe_if--;
    }
    if (nr_pppoe_if != 0 )
    {
        ALOGE("Wrong interface count found");
        nr_pppoe_if = 0;
    }
}

static void free_pppoe_list_old()
{
    interface_info_t *tmp = pppoe_if_list_old;
    while(tmp) {
        if (tmp->name) free(tmp->name);
        pppoe_if_list_old = tmp->next;
        free(tmp);
        tmp = pppoe_if_list_old;
    }
}

/*backup pppoe_if_list to pppoe_if_list_old*/
static void backup_pppoe_list() {
 free_pppoe_list_old();
    interface_info_t *tmp = pppoe_if_list;
 interface_info_t *intfinfo;
    while(tmp) {
        intfinfo = (interface_info_t *)
            malloc(sizeof(struct _interface_info_t));
        if (intfinfo == NULL) {
            ALOGE("malloc in netlink_init_interfaces_table");
            return;
        }
        /* copy the interface name (ppp0, ppp1, ...) */
        intfinfo->name = strndup((char *) tmp->name, SYSFS_PATH_MAX);
        intfinfo->i = tmp->i;
        intfinfo->next = pppoe_if_list_old;
     pppoe_if_list_old = intfinfo;
     tmp = tmp->next;
    }
}

/* find removed interface by comparing pppoe_if_list and pppoe_if_list_old*/
static char * find_removed_pppoe_if()
{
    interface_info_t *tmp1 = pppoe_if_list_old;
    interface_info_t *tmp2 = pppoe_if_list;
    while(tmp1) {
        int found = 0;
        while(tmp2) {
            if( 0 == strcmp(tmp1->name, tmp2->name)){
                 found = 1;
                 break;
            }
            tmp2 = tmp2->next;
        }

        if(!found)
            return tmp1->name;

        tmp1 = tmp1->next;
    }

    return NULL;
}

static void add_int_to_list(interface_info_t *node)
{
    /*
     *Todo: Lock here!!!!
     */
    node->next = pppoe_if_list;
    pppoe_if_list = node;
    nr_pppoe_if ++;
}

static int netlink_init_pppoe_list(void)
{
    int ret = -1;
    DIR  *netdir;
    struct dirent *de;
    char path[SYSFS_PATH_MAX];
    interface_info_t *intfinfo;
    int index;
    FILE *ifidx;
    #define MAX_FGETS_LEN 4
    char idx[MAX_FGETS_LEN+1];

    if ((netdir = opendir(SYSFS_CLASS_NET)) != NULL) {
         while((de = readdir(netdir))!=NULL) {
            if (strcmp(de->d_name,"ppp0"))
                continue;


            snprintf(path, SYSFS_PATH_MAX, "%s/%s/type", SYSFS_CLASS_NET, de->d_name);
            FILE *typefd;
            if ((typefd = fopen(path, "r")) != NULL) {
                char typestr[MAX_FGETS_LEN + 1];
                int type = 0;
                memset(typestr, 0, MAX_FGETS_LEN + 1);
                if (fgets(typestr, MAX_FGETS_LEN, typefd) != NULL) {
                    type = strtoimax(typestr, NULL, 10);
                }
                fclose(typefd);
                if (type >= ARPHRD_TUNNEL && type < ARPHRD_IEEE802_TR)
                    continue;
            }

            snprintf(path, SYSFS_PATH_MAX,"%s/%s/ifindex",SYSFS_CLASS_NET,de->d_name);
            if ((ifidx = fopen(path,"r")) != NULL ) {
                memset(idx,0,MAX_FGETS_LEN+1);
                if(fgets(idx,MAX_FGETS_LEN,ifidx) != NULL) {
                    index = strtoimax(idx,NULL,10);
                } else {
                    ALOGE("Can not read %s",path);
                    fclose(ifidx);
                    continue;
                }
                fclose(ifidx);
            } else {
                ALOGE("Can not open %s for read",path);
                continue;
            }
            /* make some room! */
            intfinfo = (interface_info_t *)
                malloc(sizeof(struct _interface_info_t));
            if (intfinfo == NULL) {
                ALOGE("malloc in netlink_init_interfaces_table");
                closedir(netdir);
                goto error;
            }
            /* copy the interface name (ppp0, ppp1, ...) */
            intfinfo->name = strndup((char *) de->d_name, SYSFS_PATH_MAX);
            intfinfo->i = index;

            //ALOGI("interface %s:%d found",intfinfo->name,intfinfo->i);
            add_int_to_list(intfinfo);
        }
        closedir(netdir);
    }
    ret = 0;
error:
    return ret;
}

ECode CPppoeNative::InitPppoeNative(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    int ret = -1;

    ALOGI("==>%s",__FUNCTION__);
    memset(&addr_msg, 0, sizeof(sockaddr_nl));
    addr_msg.nl_family = AF_NETLINK;
    memset(&addr_poll, 0, sizeof(sockaddr_nl));
    addr_poll.nl_family = AF_NETLINK;
    addr_poll.nl_pid = 0;//getpid();
    addr_poll.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    nl_sk = socket(AF_NETLINK,SOCK_RAW,NETLINK_ROUTE);
    if (nl_sk <= 0) {
        ALOGE("Can not create netlink poll socket");
        goto error;
    }

    errno = 0;
    if(bind(nl_sk, (struct sockaddr *)(&addr_poll),
            sizeof(struct sockaddr_nl))) {
        ALOGE("Can not bind to netlink poll socket,%s",strerror(errno));

        goto error;
    }

    if ((ret = netlink_init_pppoe_list()) < 0) {
        ALOGE("Can not collect the interface list");
        goto error;
    }
    ALOGE("%s exited with success",__FUNCTION__);
    return ret;
error:
    ALOGE("%s exited with error",__FUNCTION__);

    if (nl_sk >0)
        close(nl_sk);
    *val = ret;
    return NOERROR;
}

ECode CPppoeNative::GetInterfaceName(
    /* [in] */ Int32 index,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    int i = 0;
    interface_info_t *info;
    ALOGI("User ask for device name on %d, list:%X, total:%d",index,
      (unsigned int)pppoe_if_list, nr_pppoe_if);
    info= pppoe_if_list;
    if (nr_pppoe_if != 0 && index <= (nr_pppoe_if -1)) {
     while (info != NULL) {
         if (index == i) {
             ALOGI("Found :%s",info->name);
             *str = String(info->name);
             return NOERROR;
         }
         info = info->next;
         i ++;
     }
    }
    ALOGI("No device name found");
    *str = String(NULL);
    return NOERROR;
}

ECode CPppoeNative::GetInterfaceCnt(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    *val = nr_pppoe_if;
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeNative::IsInterfaceAdded(
    /* [in] */ const String& ifname,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    int retval = 0;
    /*
    const char * ppp_name = env->GetStringUTFChars(ifname, NULL);
    if (ppp_name == NULL) {
        ALOGE("Device name NULL!");
        return 0;
    }
    while (TRUE) {
        ALOGE("android_net_pppoe_isInterfaceAdded undefined!");
    }

    env->ReleaseStringUTFChars(ifname, ppp_name);
    */
    *val = retval;
    return NOERROR;
}

} // Pppoe
} // Net
} // Droid
} // Elastos

