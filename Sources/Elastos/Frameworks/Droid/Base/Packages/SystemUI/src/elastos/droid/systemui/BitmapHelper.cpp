//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/BitmapHelper.h"
#include "Elastos.Droid.Graphics.h"

using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::MatrixScaleToFit_CENTER;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IBitmapShader;
using Elastos::Droid::Graphics::CBitmapShader;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IShader;

namespace Elastos {
namespace Droid {
namespace SystemUI {

AutoPtr<IBitmap> BitmapHelper::CreateCircularClip(
    /* [in] */ IBitmap* input,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (input == NULL) {
        return NULL;
    }

    Int32 inWidth = 0;
    input->GetWidth(&inWidth);
    Int32 inHeight = 0;
    input->GetHeight(&inHeight);
    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    AutoPtr<IBitmap> output;
    helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&output);
    AutoPtr<ICanvas> canvas;
    CCanvas::New(output, (ICanvas**)&canvas);
    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    AutoPtr<IBitmapShader> p;
    CBitmapShader::New(input, ShaderTileMode_CLAMP, ShaderTileMode_CLAMP, (IBitmapShader**)&p);
    paint->SetShader(IShader::Probe(p));
    paint->SetAntiAlias(TRUE);
    AutoPtr<IRectF> srcRect;
    CRectF::New(0, 0, inWidth, inHeight, (IRectF**)&srcRect);
    AutoPtr<IRectF> dstRect;
    CRectF::New(0, 0, width, height, (IRectF**)&dstRect);
    AutoPtr<IMatrix> m;
    CMatrix::New((IMatrix**)&m);
    Boolean bRes = FALSE;
    m->SetRectToRect(srcRect, dstRect, MatrixScaleToFit_CENTER, &bRes);
    canvas->SetMatrix(m);
    canvas->DrawCircle(inWidth / 2, inHeight / 2, inWidth / 2, paint);
    return output;
}

} // namespace SystemUI
} // namepsace Droid
} // namespace Elastos