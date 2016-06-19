
#ifndef __ELASTOS_DROID_GALAXY4_SCRIPTC_H__
#define __ELASTOS_DROID_GALAXY4_SCRIPTC_H__

#include "Elastos.Droid.Content.h"
#include "Script.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class ScriptC : public Script
{
protected:
    CARAPI constructor(
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* resources,
        /* [in] */ Int32 id);

private:
    static CARAPI InternalCreate(
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* resources,
        /* [in] */ Int32 resourceID,
        /* [out] */ Int64* id);

private:
    static const String CACHE_PATH;
    static String sCachePath;
    static Object sLock;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_SCRIPTC_H__
