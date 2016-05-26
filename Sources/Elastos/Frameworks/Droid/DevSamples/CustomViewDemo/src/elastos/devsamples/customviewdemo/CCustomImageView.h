
#ifndef __Elastos_DevSamples_CustomViewDemo_CCustomImageView_H__
#define __Elastos_DevSamples_CustomViewDemo_CCustomImageView_H__

#include "_Elastos_DevSamples_CustomViewDemo_CCustomImageView.h"
#include <elastos/droid/widget/ImageView.h>

using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace DevSamples {
namespace CustomViewDemo {

CarClass(CCustomImageView)
    , public ImageView
    , public ICustomImageView
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    SetImageName(
        /* [in] */ const String& name);

    GetImageName(
        /* [out] */ String* name);
private:
    String mImageName;
};

}// namespace CustomViewDemo
}// namespace DevSamples
}// namespace Elastos

#endif // __Elastos_DevSamples_CustomViewDemo_CCustomViewDemo_H__
