
#include <pthread.h>
#include <dlfcn.h>
#include <stdio.h>
#include <prxstub.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

_ELASTOS_NAMESPACE_USING

ECode RegisterModuleInfo(
    /* [in] */ CIModuleInfo* srcModuleInfo);

ECode LookupModuleInfo(
    /* [in] */ REMuid rclsid,
    /* [out] */ CIModuleInfo** moduleInfo);
