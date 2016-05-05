
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/gesture/Gesture.h"
#include "elastos/droid/gesture/CGesture.h"
#include "elastos/droid/gesture/GestureUtils.h"
#include "elastos/droid/gesture/CGestureStroke.h"
#include "elastos/droid/gesture/GestureConstants.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CRectF.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::IDataOutput;
using Elastos::IO::IDataInput;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PaintJoin_ROUND;
using Elastos::Droid::Graphics::PaintCap_ROUND;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Gesture {

Int64 Gesture::GESTURE_ID_BASE = -1;
const Boolean Gesture::BITMAP_RENDERING_ANTIALIAS = TRUE;
const Boolean Gesture::BITMAP_RENDERING_DITHER = TRUE;
AutoPtr<IAtomicInteger32> Gesture::sGestureCount = Gesture::InitGestureCount();

AutoPtr<IAtomicInteger32> Gesture::InitGestureCount()
{
    AutoPtr<IAtomicInteger32> temp;
    CAtomicInteger32::New(0, (IAtomicInteger32**)&temp);
    return temp;
}

CAR_INTERFACE_IMPL_2(Gesture, Object, IGesture, IParcelable);

Gesture::Gesture()
    : mGestureID(0)
{
    CRectF::New((IRectF**)&mBoundingBox);
    CArrayList::New((IArrayList**)&mStrokes);

}

Gesture::~Gesture()
{}

ECode Gesture::constructor()
{
    if (GESTURE_ID_BASE < 0) {
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&GESTURE_ID_BASE);
    }

    Int32 value;
    sGestureCount->IncrementAndGet(&value);
    mGestureID = GESTURE_ID_BASE + value;
    return NOERROR;
}

ECode Gesture::GetStrokes(
    /* [out] */ IArrayList **container)
{
    VALIDATE_NOT_NULL(container)
    *container = mStrokes;
    REFCOUNT_ADD(*container);
    return NOERROR;
}

ECode Gesture::GetStrokesCount(
    /* [out] */ Int32 *count)
{
    VALIDATE_NOT_NULL(count);
    mStrokes->GetSize(count);
    return NOERROR;
}

ECode Gesture::AddStroke(
    /* [in] */ IGestureStroke *stroke)
{
    mStrokes->Add(IInterface::Probe(stroke));
    AutoPtr<IRectF> r;
    stroke->GetBoundingBox((IRectF**)&r);
    mBoundingBox->Union(r);
    return NOERROR;
}

ECode Gesture::GetLength(
    /* [out] */ Float *len)
{
    VALIDATE_NOT_NULL(len);
    *len = 0;

    AutoPtr<IArrayList> strokes = mStrokes;

    Int32 count;
    Float length;

    strokes->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        AutoPtr<IGestureStroke> stroke;

        strokes->Get(i, (IInterface**)&obj);

        stroke = IGestureStroke::Probe(obj);
        stroke->GetLength(&length);

        *len += length;
    }

    return NOERROR;
}

ECode Gesture::GetBoundingBox(
    /* [out] */ IRectF **box)
{
    VALIDATE_NOT_NULL(box);
    *box = mBoundingBox;
    REFCOUNT_ADD(*box);
    return NOERROR;
}

ECode Gesture::ToPath(
    /* [out] */ IPath **outPath)
{
    return ToPath(NULL, outPath);
}

ECode Gesture::ToPath(
    /* [in] */ IPath * _path,
    /* [out] */ IPath ** outPath)
{
    VALIDATE_NOT_NULL(outPath);

    AutoPtr<IPath> path = _path;
    if (path == NULL) {
        CPath::New((IPath**)&path);
    }

    Int32 count;
    mStrokes->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> tempPath;
        mStrokes->Get(i, (IInterface**)&tempPath);
        path->AddPath(IPath::Probe(tempPath));
    }

    *outPath = path;
    REFCOUNT_ADD(*outPath);
    return NOERROR;
}

ECode Gesture::ToPath(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 edge,
    /* [in] */ Int32 numSample,
    /* [out] */ IPath **outPath)
{
    return ToPath(NULL, width, height, edge, numSample, outPath);
}

ECode Gesture::ToPath(
    /* [in] */ IPath *path,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 edge,
    /* [in] */ Int32 numSample,
    /* [out] */ IPath **outPath)
{
    VALIDATE_NOT_NULL(outPath);

    if (path == NULL)
        CPath::New((IPath**)&path);

    Int32 count;
    mStrokes->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mStrokes->Get(i, (IInterface**)&obj);
        AutoPtr<IPath> tempPath;
        IGestureStroke::Probe(obj)->ToPath(width - 2 * edge, height - 2 * edge, numSample, (IPath**)&tempPath);
        path->AddPath(tempPath);
    }

    *outPath = path;
    REFCOUNT_ADD(*outPath);
    return NOERROR;
}

ECode Gesture::SetID(
    /* [in] */ Int64 id)
{
    mGestureID = id;
    return NOERROR;
}

ECode Gesture::GetID(
    /* [out] */ Int64 *id)
{
    VALIDATE_NOT_NULL(id);
    *id = mGestureID;
    return NOERROR;
}

ECode Gesture::ToBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 edge,
    /* [in] */ Int32 numSample,
    /* [in] */ Int32 color,
    /* [out] */ IBitmap **bm)
{
    VALIDATE_NOT_NULL(bm);

    AutoPtr<IBitmap> bitmap;
    CBitmap::CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);

    AutoPtr<ICanvas> canvas;
    CCanvas::New(bitmap, (ICanvas**)&canvas);
    canvas->Translate(edge, edge);

    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);
    paint->SetAntiAlias(BITMAP_RENDERING_ANTIALIAS);
    paint->SetDither(BITMAP_RENDERING_DITHER);
    paint->SetColor(color);
    paint->SetStyle(PaintStyle_STROKE);
    paint->SetStrokeJoin(PaintJoin_ROUND);
    paint->SetStrokeCap(PaintCap_ROUND);
    paint->SetStrokeWidth(BITMAP_RENDERING_WIDTH);

    Int32 count;
    mStrokes->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mStrokes->Get(i, (IInterface**)&obj);
        AutoPtr<IPath> tempPath;
        IGestureStroke::Probe(obj)->ToPath(width - 2 * edge, height - 2 * edge, numSample, (IPath**)&tempPath);
        canvas->DrawPath(tempPath, paint);
    }

    *bm = bitmap;
    REFCOUNT_ADD(*bm);
    return NOERROR;
}

ECode Gesture::ToBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 inset,
    /* [in] */ Int32 color,
    /* [out] */ IBitmap **bm)
{
    VALIDATE_NOT_NULL(bm);

    AutoPtr<IBitmap> bitmap;
    CBitmap::CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);

    AutoPtr<ICanvas> canvas;
    CCanvas::New(bitmap, (ICanvas**)&canvas);

    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);

    paint->SetAntiAlias(BITMAP_RENDERING_ANTIALIAS);
    paint->SetDither(BITMAP_RENDERING_DITHER);
    paint->SetColor(color);
    paint->SetStyle(PaintStyle_STROKE);
    paint->SetStrokeJoin(PaintJoin_ROUND);
    paint->SetStrokeCap(PaintCap_ROUND);
    paint->SetStrokeWidth(BITMAP_RENDERING_WIDTH);

    AutoPtr<IPath> path;
    ToPath((IPath**)&path);
    AutoPtr<IRectF> bounds;
    CRectF::New((IRectF**)&bounds);
    path->ComputeBounds(bounds, TRUE);

    Float w, h;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);
    const Float sx = (width - 2 * inset) / w;
    const Float sy = (height - 2 * inset) / h;
    const Float scale = sx > sy ? sy : sx;
    paint->SetStrokeWidth(2.0f / scale);

    Float l, t;
    bounds->GetLeft(&l);
    bounds->GetTop(&t);
    path->Offset(-l + (width - w * scale) / 2.0f,
            -t + (height - h * scale) / 2.0f);

    canvas->Translate(inset, inset);
    canvas->Scale(scale, scale);
    canvas->DrawPath(path, paint);

    *bm = bitmap;
    REFCOUNT_ADD(*bm);
    return NOERROR;
}

ECode Gesture::Serialize(
    /* [in] */ IDataOutputStream *out)
{
    VALIDATE_NOT_NULL(out);

    Int32 count;
    mStrokes->GetSize(&count);

    // Write gesture ID
    IDataOutput::Probe(out)->WriteInt64(mGestureID);
    // Write number of strokes
    IDataOutput::Probe(out)->WriteInt32(count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mStrokes->Get(i, (IInterface**)&obj);
        IGestureStroke::Probe(obj)->Serialize(out);
    }

    return NOERROR;
}

ECode Gesture::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt64(mGestureID);

    Boolean result = FALSE;
    AutoPtr<IByteArrayOutputStream> byteStream;
    CByteArrayOutputStream::New(GestureConstants::IO_BUFFER_SIZE, (IByteArrayOutputStream**)&byteStream);
    AutoPtr<IDataOutputStream> outStream;
    AutoPtr<IOutputStream> os = IOutputStream::Probe(byteStream);
    CDataOutputStream::New(os, (IDataOutputStream**)&outStream);

    ECode ec = Serialize(outStream);
    if (FAILED(ec))
        Logger::E(GestureConstants::myLOG_TAG, "Error writing Gesture to parcel:0x%x", ec);
    result = TRUE;

    AutoPtr<ICloseable> ic, ic2;
    ic = ICloseable::Probe(outStream);
    ic2 = ICloseable::Probe(byteStream);

    GestureUtils::CloseStream(ic);
    GestureUtils::CloseStream(ic2);

    if (result) {
        AutoPtr<ArrayOf<Byte> > streamBytes;
        byteStream->ToByteArray((ArrayOf<Byte>**)&streamBytes);
        out->WriteArrayOf((Handle32)streamBytes.Get());
    }
    return NOERROR;
}

ECode Gesture::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int64 gestureID;
    in->ReadInt64(&gestureID);
#if 0
    AutoPtr<ArrayOf<Byte> > bytes;

    bytes = ArrayOf<Byte>::Alloc(in->dataCapacity);
    if (bytes == NULL)
        Logger::E(GestureConstants::myLOG_TAG, "No Memory");

    in->read(bytes->GetPayload(), in->dataCapacity);

    AutoPtr<IByteArrayInputStream> byteStream;
    CByteArrayInputStream::New(bytes, (IByteArrayInputStream**)&byteStream);
    AutoPtr<IDataInputStream> inStream;
    CDataInputStream::New(byteStream, (IDataInputStream**)&inStream);

    AutoPtr<IDataInput> ins = IDataInput::Probe(inStream);
    ECode ec1 = ins->ReadInt64(&mGestureID);
    Int32 count;
    ECode ec2 = ins->ReadInt32(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IGestureStroke> stroke;
        CGestureStroke::Deserialize(inStream, (IGestureStroke**)&stroke);
        AddStroke(stroke);
    }

    if (FAILED(ec1) || FAILED(ec2))
        Logger::E(GestureConstants::myLOG_TAG, "Error reading Gesture from parcel:0x%x || 0x%x", ec1, ec2);

    GestureUtils::CloseStream(inStream);

    if (FAILED(ec1) || FAILED(ec2))
        mGestureID = gestureID;
#endif
    return NOERROR;
}

ECode Gesture::Deserialize(
    /* [in] */ IDataInputStream *in,
    /* [out] */ IGesture** gesture)
{
    CGesture::New(gesture);

    Int64 gestureID;
    FAIL_RETURN(IDataInput::Probe(in)->ReadInt64(&gestureID));
    ((Gesture*)(*gesture))->SetID(gestureID);

    Int32 count;
    FAIL_RETURN(IDataInput::Probe(in)->ReadInt32(&count));

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IGestureStroke> stroke;
        CGestureStroke::Deserialize(in, (IGestureStroke**)&stroke);
        (*gesture)->AddStroke(stroke);
    }

    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
