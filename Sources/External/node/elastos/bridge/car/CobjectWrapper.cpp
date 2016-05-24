
#include "ElNode.h"
#include "config.h"

//#include "elastos/HashSet.h"
#include "etl/HashSet.h"
//#include "elastos.h"

#include "CobjectWrapper.h"

//#include "ext/frameworkdef.h"

#include <utils/Log.h>

#define LOG_ERROR ALOGD

#if ENABLE(CAR_BRIDGE)

using namespace JSC::Bindings;

CobjectWrapper::CobjectWrapper(IInterface* instance, IDataTypeInfo* dataTypeInfo)
    : mDataTypeInfo(0),
    mRefCount(0)
{
    assert(instance);

    // Cache the JNIEnv used to get the global ref for this java instanace.
    // It'll be used to delete the reference.
    mInstance = NULL;
    setInstance(instance);

    setDataTypeInfo(dataTypeInfo);

    //if (!mInstance) {
    //    LOG_ERROR("CobjectWrapper instance is NULL");
    //}
}

CobjectWrapper::~CobjectWrapper()
{
    mInstance = NULL;
}

#endif // ENABLE(CAR_BRIDGE)
