#ifndef __ELASTOS_DROID_WIDGET_COMMATOKENIZER_H__
#define __ELASTOS_DROID_WIDGET_COMMATOKENIZER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Widget {

class CommaTokenizer
{
public:
    CARAPI_(Int32) FindTokenStart(
        /* [in] */ ICharSequence* text,
        /* [in] */ [in] Int32 cursor);

    CARAPI_(Int32) FindTokenEnd(
        /* [in] */ ICharSequence* text,
        /* [in] */ [in] Int32 cursor);

    CARAPI_(AutoPtr<ICharSequence>) TerminateToken(
        /* [in] */ ICharSequence* text);

protected:
    CommaTokenizer();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_COMMATOKENIZER_H__
