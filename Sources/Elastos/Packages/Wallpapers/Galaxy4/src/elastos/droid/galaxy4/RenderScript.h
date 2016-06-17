
#ifndef __ELASTOS_DROID_GALAXY4_RENDERSCRIPT_H__
#define __ELASTOS_DROID_GALAXY4_RENDERSCRIPT_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class RenderScript
    : public Object
{
public:
    /**
     * @deprecated in API 16
     * Class which is used to describe a pixel format for a graphical buffer.
     * This is used to describe the intended format of the display surface.
     *
     * The configuration is described by pairs of minimum and preferred bit
     * depths for each component within the config and additional structural
     * information.
     */
    class SurfaceConfig
        : public Object
    {
    public:
        SurfaceConfig()
            : mDepthMin(0)
            , mDepthPref(0)
            , mStencilMin(0)
            , mStencilPref(0)
            , mColorMin(8)
            , mColorPref(8)
            , mAlphaMin(0)
            , mAlphaPref(0)
            , mSamplesMin(1)
            , mSamplesPref(1)
            , mSamplesQ(1.0)
        {}

    public:
        Int32 mDepthMin;
        Int32 mDepthPref;
        Int32 mStencilMin;
        Int32 mStencilPref;
        Int32 mColorMin;
        Int32 mColorPref;
        Int32 mAlphaMin;
        Int32 mAlphaPref;
        Int32 mSamplesMin;
        Int32 mSamplesPref;
        Float mSamplesQ;
    };
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_RENDERSCRIPT_H__
