#ifndef __ELASTOS_DROID_WIDGET_CTEXTVIEWHELPER_H__
#define __ELASTOS_DROID_WIDGET_CTEXTVIEWHELPER_H__

#include "_Elastos_Droid_Widget_CTextViewHelper.h"
#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#else
#include "Elastos.Droid.Core.h"
#endif

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTextViewHelper)
{
public:

    CARAPI GetTextColors(
        /* [in] */ IContext* context,
        /* [in] */ ITypedArray* attrs,
        /* [out] */ IColorStateList** color);

    /**
     * Returns the default color from the TextView_textColor attribute
     * from the AttributeSet, if set, or the default color from the
     * TextAppearance_textColor from the TextView_textAppearance attribute,
     * if TextView_textColor was not set directly.
     */
    CARAPI GetTextColor(
        /* [in] */ IContext* context,
        /* [in] */ ITypedArray* attrs,
        /* [in] */ Int32 def,
        /* [out] */ Int32* color);
};


}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CTEXTVIEWHELPER_H__
