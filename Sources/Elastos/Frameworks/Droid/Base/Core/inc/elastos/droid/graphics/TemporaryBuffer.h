
#ifndef __ELASTOS_DROID_GRAPHICS_TEMPORARYBUFFER_H__
#define __ELASTOS_DROID_GRAPHICS_TEMPORARYBUFFER_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Graphics {

class TemporaryBuffer
{
public:
    static CARAPI_(AutoPtr< ArrayOf<Char32> >) Obtain(
       /* [in] */ Int32 len);

    static CARAPI_(void) Recycle(
       /*  [in] */ ArrayOf<Char32>* temp);

private:
    static AutoPtr< ArrayOf<Char32> > sTemp;
    static Object sObject;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __CTEMPORARYBUFFER_H__
