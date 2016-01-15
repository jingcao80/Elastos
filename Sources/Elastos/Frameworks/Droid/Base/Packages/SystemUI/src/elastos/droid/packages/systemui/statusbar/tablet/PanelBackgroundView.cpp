#include "elastos/droid/systemui/statusbar/tablet/PanelBackgroundView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


PanelBackgroundView::PanelBackgroundView()
{}

PanelBackgroundView::PanelBackgroundView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : View(context, attrs)
{
    InitImpl(context, attrs);
}

ECode PanelBackgroundView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::Init(context, attrs);
    return InitImpl(context, attrs);
}

ECode PanelBackgroundView::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    /*
    mTexture = BitmapFactory.decodeResource(getResources(),
            com.android.internal.R.drawable.status_bar_background);
    mTextureWidth = mTexture.getWidth();
    mTextureHeight = mTexture.getHeight();

    mPaint = new Paint();
    mPaint.setDither(false);
    */
   return NOERROR;
}

void PanelBackgroundView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    /*
    final Bitmap texture = mTexture;
    final Paint paint = mPaint;

    final int width = getWidth();
    final int height = getHeight();

    final int textureWidth = mTextureWidth;
    final int textureHeight = mTextureHeight;

    int x = 0;
    int y;

    while (x < width) {
        y = 0;
        while (y < height) {
            canvas.drawBitmap(texture, x, y, paint);
            y += textureHeight;
        }
        x += textureWidth;
    }
    */
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
