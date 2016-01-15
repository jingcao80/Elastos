
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CPaintFlagsDrawFilter.h"
#include <skia/effects/SkPaintFlagsDrawFilter.h>
#include <skia/core/SkPaint.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPaintFlagsDrawFilter);
ECode CPaintFlagsDrawFilter::constructor(
    /* [in] */ Int32 clearBits,
    /* [in] */ Int32 setBits)
{
    mClearBits = clearBits;
    mSetBits = setBits;
    mNativeInstance = NativeConstructor(clearBits, setBits);
    return NOERROR;
}

PInterface CPaintFlagsDrawFilter::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_DrawFilter) {
        return reinterpret_cast<PInterface>((DrawFilter*)this);
    }
    else if (riid == EIID_IPaintFlagsDrawFilter) {
        return (IPaintFlagsDrawFilter*)this;
    }
    return DrawFilter::Probe(riid);
}

UInt32 CPaintFlagsDrawFilter::AddRef()
{
    return DrawFilter::AddRef();
}

UInt32 CPaintFlagsDrawFilter::Release()
{
    return DrawFilter::Release();
}

ECode CPaintFlagsDrawFilter::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return DrawFilter::GetInterfaceID(object, iid);
}

// Custom version of SkPaintFlagsDrawFilter that also calls setFilterLevel.
class CompatFlagsDrawFilter : public SkPaintFlagsDrawFilter {
public:
    CompatFlagsDrawFilter(uint32_t clearFlags, uint32_t setFlags,
            SkPaint::FilterLevel desiredLevel)
    : SkPaintFlagsDrawFilter(clearFlags, setFlags)
    , fDesiredLevel(desiredLevel) {
    }

    virtual bool filter(SkPaint* paint, Type type) {
        SkPaintFlagsDrawFilter::filter(paint, type);
        paint->setFilterLevel(fDesiredLevel);
        return true;
    }

private:
    const SkPaint::FilterLevel fDesiredLevel;
};

// Returns whether flags contains FILTER_BITMAP_FLAG. If flags does, remove it.
static inline bool hadFiltering(Int32& flags) {
    // Equivalent to the Java Paint's FILTER_BITMAP_FLAG.
    static const uint32_t sFilterBitmapFlag = 0x02;

    const bool result = (flags & sFilterBitmapFlag) != 0;
    flags &= ~sFilterBitmapFlag;
    return result;
}

Int64 CPaintFlagsDrawFilter::NativeConstructor(
    /* [in] */ Int32 clearFlags,
    /* [in] */ Int32 setFlags)
{
    if (clearFlags | setFlags) {
        // Mask both groups of flags to remove FILTER_BITMAP_FLAG, which no
        // longer has a Skia equivalent flag (instead it corresponds to
        // calling setFilterLevel), and keep track of which group(s), if
        // any, had the flag set.
        const bool turnFilteringOn = hadFiltering(setFlags);
        const bool turnFilteringOff = hadFiltering(clearFlags);

        SkDrawFilter* filter;
        if (turnFilteringOn) {
            // Turning filtering on overrides turning it off.
            filter = new CompatFlagsDrawFilter(clearFlags, setFlags,
                    SkPaint::kLow_FilterLevel);
        } else if (turnFilteringOff) {
            filter = new CompatFlagsDrawFilter(clearFlags, setFlags,
                    SkPaint::kNone_FilterLevel);
        } else {
            filter = new SkPaintFlagsDrawFilter(clearFlags, setFlags);
        }
        return reinterpret_cast<Int64>(filter);
    }
    return 0;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
