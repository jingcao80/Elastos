#ifndef __ELASTOS_TEXT_TEXTNORMALIZER_H__
#define __ELASTOS_TEXT_TEXTNORMALIZER_H__

#include "Elastos.CoreLibrary.h"

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Text {

class Normalizer {
public:
    static CARAPI_(Boolean) IsNormalized(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form);

    static CARAPI_(String) Normalize(
        /* [in] */ ICharSequence* src,
        /* [in] */ NormalizerForm form);

private:
    Normalizer();
    Normalizer(const Normalizer&);
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_TEXTNORMALIZER_H__