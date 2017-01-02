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

#include "CDrawView.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include "R.h"

using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::ShaderTileMode_REPEAT;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::CLinearGradient;

namespace Elastos {
namespace DevSamples {
namespace DrawPathDemo {

CAR_INTERFACE_IMPL(CDrawView, Elastos::Droid::View::View, IDrawView)

CAR_OBJECT_IMPL(CDrawView)

void CDrawView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Elastos::Droid::View::View::OnDraw(canvas);
    /*
     * 方法 说明 drawRect 绘制矩形 drawCircle 绘制圆形 drawOval 绘制椭圆 drawPath 绘制任意多边形
     * drawLine 绘制直线 drawPoin 绘制点
     */
    // 创建画笔
    AutoPtr<IPaint> p;
    CPaint::New((IPaint**)&p);

    p->SetColor(IColor::RED);                   // 设置红色

    canvas->DrawText(String("画圆："), 10, 20, p);       // 画文本
    canvas->DrawCircle(60, 20, 10, p);          // 小圆
    p->SetAntiAlias(TRUE);                      // 设置画笔的锯齿效果。 true是去除，大家一看效果就明白了
    canvas->DrawCircle(120, 20, 20, p);         // 大圆

    canvas->DrawText(String("画线及弧线："), 10, 60, p);
    p->SetColor(IColor::GREEN);                 // 设置绿色
    canvas->DrawLine(60, 40, 100, 40, p);       // 画线
    canvas->DrawLine(110, 40, 190, 80, p);      // 斜线

    //画笑脸弧线
    p->SetStyle(PaintStyle_STROKE);             //设置空心
    AutoPtr<IRectF> oval1;
    CRectF::New(150, 20, 180, 40, (IRectF**)&oval1);
    canvas->DrawArc(oval1, 180, 180, false, p); //小弧形
    oval1->Set(190, 20, 220, 40);
    canvas->DrawArc(oval1, 180, 180, false, p); //小弧形
    oval1->Set(160, 30, 210, 60);
    canvas->DrawArc(oval1, 0, 180, false, p);   //小弧形

    canvas->DrawText(String("画矩形："), 10, 80, p);
    p->SetColor(IColor::GRAY);                  // 设置灰色
    p->SetStyle(PaintStyle_FILL);               //设置填满
    canvas->DrawRect(60, 60, 80, 80, p);        // 正方形
    canvas->DrawRect(60, 90, 160, 100, p);      // 长方形

    canvas->DrawText(String("画扇形和椭圆:"), 10, 120, p);
    /* 设置渐变色 这个正方形的颜色是改变的 */
    AutoPtr<ArrayOf<Int32> > colors = ArrayOf<Int32>::Alloc(5);
    colors->Set(0, IColor::RED); colors->Set(1, IColor::GREEN);
    colors->Set(2, IColor::BLUE); colors->Set(3, IColor::YELLOW);
    colors->Set(4, IColor::LTGRAY);
    // 一个材质,打造出一个线性梯度沿著一条线。
    AutoPtr<IShader> shader;
    CLinearGradient::New(0, 0, 100, 100, colors, NULL, ShaderTileMode_REPEAT, (IShader**)&shader);
    p->SetShader(shader);
    // p->SetColor(IColor::BLUE);

    AutoPtr<IRectF> oval2;
    CRectF::New(60, 100, 200, 240, (IRectF**)&oval2);   // 设置个新的长方形，扫描测量
    canvas->DrawArc(oval2, 200, 130, true, p);
    // 画弧，第一个参数是RectF：该类是第二个参数是角度的开始，第三个参数是多少度，第四个参数是真的时候画扇形，是假的时候画弧线

    //画椭圆，把oval改一下
    oval2->Set(210, 100, 250, 130);
    canvas->DrawOval(oval2, p);

    canvas->DrawText(String("画三角形："), 10, 200, p);
    // 绘制这个三角形,你可以绘制任意多边形
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    path->MoveTo(80, 200);              // 此点为多边形的起点
    path->LineTo(120, 250);
    path->LineTo(80, 250);
    path->Close();                      // 使这些点构成封闭的多边形
    canvas->DrawPath(path, p);

    // 你可以绘制很多任意多边形，比如下面画六连形
    p->Reset();                         //重置
    p->SetColor(IColor::LTGRAY);
    p->SetStyle(PaintStyle_STROKE);     //设置空心
    AutoPtr<IPath> path1;
    CPath::New((IPath**)&path1);
    path1->MoveTo(180, 200);
    path1->LineTo(200, 200);
    path1->LineTo(210, 210);
    path1->LineTo(200, 220);
    path1->LineTo(180, 220);
    path1->LineTo(170, 210);
    path1->Close();//封闭
    canvas->DrawPath(path1, p);

    /*
     * Path类封装复合(多轮廓几何图形的路径
     * 由直线段*、二次曲线,和三次方曲线，也可画以油画。drawPath(路径、油漆),要么已填充的或抚摸
     * (基于油漆的风格),或者可以用于剪断或画画的文本在路径。
     */

    //画圆角矩形
    p->SetStyle(PaintStyle_FILL);                       //充满
    p->SetColor(IColor::LTGRAY);
    p->SetAntiAlias(true);                              // 设置画笔的锯齿效果
    canvas->DrawText(String("画圆角矩形:"), 10, 260, p);
    AutoPtr<IRectF> oval3;
    CRectF::New(80, 260, 200, 300, (IRectF**)&oval3);   // 设置个新的长方形
    canvas->DrawRoundRect(oval3, 20, 15, p);            //第二个参数是x半径，第三个参数是y半径

    //画贝塞尔曲线
    canvas->DrawText(String("画贝塞尔曲线:"), 10, 310, p);
    p->Reset();
    p->SetStyle(PaintStyle_STROKE);
    p->SetColor(IColor::GREEN);

    AutoPtr<IPath> path2;
    CPath::New((IPath**)&path2);
    path2->MoveTo(100, 320);                //设置Path的起点
    path2->QuadTo(150, 310, 170, 400);      //设置贝塞尔曲线的控制点坐标和终点坐标
    canvas->DrawPath(path2, p);             //画出贝塞尔曲线

    //画点
    p->SetStyle(PaintStyle_FILL);
    canvas->DrawText(String("画点："), 10, 390, p);
    canvas->DrawPoint(60, 390, p);          //画一个点
    AutoPtr<ArrayOf<Float> > points = ArrayOf<Float>::Alloc(6);
    points->Set(0, 60); points->Set(1, 400); points->Set(2, 65);
    points->Set(3, 400); points->Set(4, 70); points->Set(5, 400);
    canvas->DrawPoints(points, p);          //画多个点

    //画图片，就是贴图
    AutoPtr<IBitmapFactory> bf;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bf);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IBitmap> bitmap;
    bf->DecodeResource(res, R::drawable::icon, (IBitmap**)&bitmap);
    canvas->DrawBitmap(bitmap, 250, 360, p);
}

}// namespace DrawPathDemo
}// namespace DevSamples
}// namespace Elastos