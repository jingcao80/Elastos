#ifndef __SHADERUTIL_H__
#define __SHADERUTIL_H__

#include "Elastos.Droid.Core.h"
#include "Elastos.Core.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace OpenglEarthDemo {

class ShaderUtil
{
public:
    static CARAPI_(Int32) LoadShader(
        /* [in] */ Int32 shaderType,
        /* [in] */ const String& source);

    static CARAPI_(Int32) CreateProgram(
        /* [in] */ const String& vSource,
        /* [in] */ const String& fSource);

    static CARAPI_(String) LoadFromAssetsFile(
        /* [in] */ const String& fName,
        /* [in] */ IResources* r);
};

} // namespace OpenglEarthDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif
