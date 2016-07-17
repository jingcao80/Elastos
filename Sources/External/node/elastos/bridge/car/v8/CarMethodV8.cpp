
#include "ElNode.h"
#include "config.h"

#include "etl/HashSet.h"
#include "elastos.h"

#include "CarMethodV8.h"
#include "ext/frameworkdef.h"

#include <utils/Log.h>

#if ENABLE(CAR_BRIDGE)

using namespace JSC::Bindings;

#endif // ENABLE(CAR_BRIDGE)

using namespace JSC::Bindings;

CarMethodV8::CarMethodV8(IMethodInfo* aMethod)
: mIsRunOnUiThread(false)
{
    mMethodInfo = aMethod;

    //Get method name
    Elastos::String nameBuf;
    aMethod->GetName(&nameBuf);
    mName = CarString(Elastos::String((const char*)nameBuf));

    //Get parameters
    Int32 N;
    aMethod->GetParamCount(&N);
    if (N > 0) {
        ArrayOf<IParamInfo*>* paramInfos = ArrayOf<IParamInfo*>::Alloc(N);
        aMethod->GetAllParamInfos(paramInfos);
        for (Int32 i = 0; i < N; ++i){
                AutoPtr<IParamInfo> aParameter = (*paramInfos)[i];
                aParameter->GetName(&nameBuf);
                mParameters.append((const char*)nameBuf);
                //(*paramInfos)[i]->Release();
                //(*paramInfos)[i] = NULL;
        }
        ArrayOf<IParamInfo*>::Free(paramInfos);
    }

    //Created lazily
    mSignature = 0;
}

CarMethodV8::~CarMethodV8()
{
    if (mSignature) {
        fastFree(mSignature);
    }
}
