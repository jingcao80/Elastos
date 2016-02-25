#ifndef __ELASTOS_DROID_VIEW_CTEXTUREVIEW_H__
#define __ELASTOS_DROID_VIEW_CTEXTUREVIEW_H__

#include "_Elastos_Droid_View_CTextureView.h"
#include "elastos/droid/view/TextureView.h"

using Elastos::Droid::View::TextureView;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CTextureView)
    , public TextureView
{
public:
    CAR_OBJECT_DECL()
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CTEXTUREVIEW_H__
