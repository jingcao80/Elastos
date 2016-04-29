
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/utility/PathParser.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Graphics::CPath;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Utility {

const String PathParser::LOGTAG("PathParser");

//===========================================================================
// PathDataNode
//===========================================================================

PathDataNode::PathDataNode(
    /* [in] */ Char32 type,
    /* [in] */ ArrayOf<Float>* params)
{
    mType = type;
    mParams = params;
}

PathDataNode::PathDataNode(
    /* [in] */ PathDataNode* n)
{
    mType = n->mType;
    Arrays::CopyOf(n->mParams, n->mParams->GetLength(), (ArrayOf<Float>**)&mParams);
}

void PathDataNode::NodesToPath(
    /* [in] */ ArrayOf<PathDataNode*>* node,
    /* [in] */ IPath* path)
{
    AutoPtr<ArrayOf<Float> > current = ArrayOf<Float>::Alloc(4);
    Char32 previousCommand = 'm';
    for (Int32 i = 0; i < node->GetLength(); i++) {
        AddCommand(path, current, previousCommand, (*node)[i]->mType, (*node)[i]->mParams);
        previousCommand = (*node)[i]->mType;
    }
}

void PathDataNode::InterpolatePathDataNode(
    /* [in] */ PathDataNode* nodeFrom,
    /* [in] */ PathDataNode* nodeTo,
    /* [in] */ Float fraction)
{
    for (Int32 i = 0; i < nodeFrom->mParams->GetLength(); i++) {
        (*mParams)[i] = (*nodeFrom->mParams)[i] * (1 - fraction)
                + (*nodeTo->mParams)[i] * fraction;
    }
}

void PathDataNode::AddCommand(
    /* [in] */ IPath* path,
    /* [in] */ ArrayOf<Float>* current,
    /* [in] */ Char32 previousCmd,
    /* [in] */ Char32 cmd,
    /* [in] */ ArrayOf<Float>* val)
{
    Int32 incr = 2;
    Float currentX = (*current)[0];
    Float currentY = (*current)[1];
    Float ctrlPointX = (*current)[2];
    Float ctrlPointY = (*current)[3];
    Float reflectiveCtrlPointX;
    Float reflectiveCtrlPointY;

    switch (cmd) {
        case 'z':
        case 'Z':
            path->Close();
            return;
        case 'm':
        case 'M':
        case 'l':
        case 'L':
        case 't':
        case 'T':
            incr = 2;
            break;
        case 'h':
        case 'H':
        case 'v':
        case 'V':
            incr = 1;
            break;
        case 'c':
        case 'C':
            incr = 6;
            break;
        case 's':
        case 'S':
        case 'q':
        case 'Q':
            incr = 4;
            break;
        case 'a':
        case 'A':
            incr = 7;
            break;
    }

    for (Int32 k = 0; k < val->GetLength(); k += incr) {
        switch (cmd) {
            case 'm': // moveto - Start a new sub-path (relative)
                path->RMoveTo((*val)[k + 0], (*val)[k + 1]);
                currentX += (*val)[k + 0];
                currentY += (*val)[k + 1];
                break;
            case 'M': // moveto - Start a new sub-path
                path->MoveTo((*val)[k + 0], (*val)[k + 1]);
                currentX = (*val)[k + 0];
                currentY = (*val)[k + 1];
                break;
            case 'l': // lineto - Draw a line from the current point (relative)
                path->RLineTo((*val)[k + 0], (*val)[k + 1]);
                currentX += (*val)[k + 0];
                currentY += (*val)[k + 1];
                break;
            case 'L': // lineto - Draw a line from the current point
                path->LineTo((*val)[k + 0], (*val)[k + 1]);
                currentX = (*val)[k + 0];
                currentY = (*val)[k + 1];
                break;
            case 'z': // closepath - Close the current subpath
            case 'Z': // closepath - Close the current subpath
                path->Close();
                break;
            case 'h': // horizontal lineto - Draws a horizontal line (relative)
                path->RLineTo((*val)[k + 0], 0);
                currentX += (*val)[k + 0];
                break;
            case 'H': // horizontal lineto - Draws a horizontal line
                path->LineTo((*val)[k + 0], currentY);
                currentX = (*val)[k + 0];
                break;
            case 'v': // vertical lineto - Draws a vertical line from the current point (r)
                path->RLineTo(0, (*val)[k + 0]);
                currentY += (*val)[k + 0];
                break;
            case 'V': // vertical lineto - Draws a vertical line from the current point
                path->LineTo(currentX, (*val)[k + 0]);
                currentY = (*val)[k + 0];
                break;
            case 'c': // curveto - Draws a cubic Bézier curve (relative)
                path->RCubicTo((*val)[k + 0], (*val)[k + 1], (*val)[k + 2], (*val)[k + 3],
                        (*val)[k + 4], (*val)[k + 5]);

                ctrlPointX = currentX + (*val)[k + 2];
                ctrlPointY = currentY + (*val)[k + 3];
                currentX += (*val)[k + 4];
                currentY += (*val)[k + 5];

                break;
            case 'C': // curveto - Draws a cubic Bézier curve
                path->CubicTo((*val)[k + 0], (*val)[k + 1], (*val)[k + 2], (*val)[k + 3],
                        (*val)[k + 4], (*val)[k + 5]);
                currentX = (*val)[k + 4];
                currentY = (*val)[k + 5];
                ctrlPointX = (*val)[k + 2];
                ctrlPointY = (*val)[k + 3];
                break;
            case 's': // smooth curveto - Draws a cubic Bézier curve (reflective cp)
                reflectiveCtrlPointX = 0;
                reflectiveCtrlPointY = 0;
                if (previousCmd == 'c' || previousCmd == 's'
                        || previousCmd == 'C' || previousCmd == 'S') {
                    reflectiveCtrlPointX = currentX - ctrlPointX;
                    reflectiveCtrlPointY = currentY - ctrlPointY;
                }
                path->RCubicTo(reflectiveCtrlPointX, reflectiveCtrlPointY,
                        (*val)[k + 0], (*val)[k + 1],
                        (*val)[k + 2], (*val)[k + 3]);

                ctrlPointX = currentX + (*val)[k + 0];
                ctrlPointY = currentY + (*val)[k + 1];
                currentX += (*val)[k + 2];
                currentY += (*val)[k + 3];
                break;
            case 'S': // shorthand/smooth curveto Draws a cubic Bézier curve(reflective cp)
                reflectiveCtrlPointX = currentX;
                reflectiveCtrlPointY = currentY;
                if (previousCmd == 'c' || previousCmd == 's'
                        || previousCmd == 'C' || previousCmd == 'S') {
                    reflectiveCtrlPointX = 2 * currentX - ctrlPointX;
                    reflectiveCtrlPointY = 2 * currentY - ctrlPointY;
                }
                path->CubicTo(reflectiveCtrlPointX, reflectiveCtrlPointY,
                        (*val)[k + 0], (*val)[k + 1], (*val)[k + 2], (*val)[k + 3]);
                ctrlPointX = (*val)[k + 0];
                ctrlPointY = (*val)[k + 1];
                currentX = (*val)[k + 2];
                currentY = (*val)[k + 3];
                break;
            case 'q': // Draws a quadratic Bézier (relative)
                path->RQuadTo((*val)[k + 0], (*val)[k + 1], (*val)[k + 2], (*val)[k + 3]);
                ctrlPointX = currentX + (*val)[k + 0];
                ctrlPointY = currentY + (*val)[k + 1];
                currentX += (*val)[k + 2];
                currentY += (*val)[k + 3];
                break;
            case 'Q': // Draws a quadratic Bézier
                path->QuadTo((*val)[k + 0], (*val)[k + 1], (*val)[k + 2], (*val)[k + 3]);
                ctrlPointX = (*val)[k + 0];
                ctrlPointY = (*val)[k + 1];
                currentX = (*val)[k + 2];
                currentY = (*val)[k + 3];
                break;
            case 't': // Draws a quadratic Bézier curve(reflective control point)(relative)
                reflectiveCtrlPointX = 0;
                reflectiveCtrlPointY = 0;
                if (previousCmd == 'q' || previousCmd == 't'
                        || previousCmd == 'Q' || previousCmd == 'T') {
                    reflectiveCtrlPointX = currentX - ctrlPointX;
                    reflectiveCtrlPointY = currentY - ctrlPointY;
                }
                path->RQuadTo(reflectiveCtrlPointX, reflectiveCtrlPointY,
                        (*val)[k + 0], (*val)[k + 1]);
                ctrlPointX = currentX + reflectiveCtrlPointX;
                ctrlPointY = currentY + reflectiveCtrlPointY;
                currentX += (*val)[k + 0];
                currentY += (*val)[k + 1];
                break;
            case 'T': // Draws a quadratic Bézier curve (reflective control point)
                reflectiveCtrlPointX = currentX;
                reflectiveCtrlPointY = currentY;
                if (previousCmd == 'q' || previousCmd == 't'
                        || previousCmd == 'Q' || previousCmd == 'T') {
                    reflectiveCtrlPointX = 2 * currentX - ctrlPointX;
                    reflectiveCtrlPointY = 2 * currentY - ctrlPointY;
                }
                path->QuadTo(reflectiveCtrlPointX, reflectiveCtrlPointY,
                        (*val)[k + 0], (*val)[k + 1]);
                ctrlPointX = reflectiveCtrlPointX;
                ctrlPointY = reflectiveCtrlPointY;
                currentX = (*val)[k + 0];
                currentY = (*val)[k + 1];
                break;
            case 'a': // Draws an elliptical arc
                // (rx ry x-axis-rotation large-arc-flag sweep-flag x y)
                DrawArc(path,
                        currentX,
                        currentY,
                        (*val)[k + 5] + currentX,
                        (*val)[k + 6] + currentY,
                        (*val)[k + 0],
                        (*val)[k + 1],
                        (*val)[k + 2],
                        (*val)[k + 3] != 0,
                        (*val)[k + 4] != 0);
                currentX += (*val)[k + 5];
                currentY += (*val)[k + 6];
                ctrlPointX = currentX;
                ctrlPointY = currentY;
                break;
            case 'A': // Draws an elliptical arc
                DrawArc(path,
                        currentX,
                        currentY,
                        (*val)[k + 5],
                        (*val)[k + 6],
                        (*val)[k + 0],
                        (*val)[k + 1],
                        (*val)[k + 2],
                        (*val)[k + 3] != 0,
                        (*val)[k + 4] != 0);
                currentX = (*val)[k + 5];
                currentY = (*val)[k + 6];
                ctrlPointX = currentX;
                ctrlPointY = currentY;
                break;
        }
        previousCmd = cmd;
    }
    (*current)[0] = currentX;
    (*current)[1] = currentY;
    (*current)[2] = ctrlPointX;
    (*current)[3] = ctrlPointY;
}

void PathDataNode::DrawArc(
    /* [in] */ IPath* p,
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [in] */ Float theta,
    /* [in] */ Boolean isMoreThanHalf,
    /* [in] */ Boolean isPositiveArc)
{
    using Elastos::Core::Math;

    /* Convert rotation angle from degrees to radians */
    Double thetaD = Math::ToRadians(theta);
    /* Pre-compute rotation matrix entries */
    Double cosTheta = Math::Cos(thetaD);
    Double sinTheta = Math::Sin(thetaD);
    /* Transform (x0, y0) and (x1, y1) into unit space */
    /* using (inverse) rotation, followed by (inverse) scale */
    Double x0p = (x0 * cosTheta + y0 * sinTheta) / a;
    Double y0p = (-x0 * sinTheta + y0 * cosTheta) / b;
    Double x1p = (x1 * cosTheta + y1 * sinTheta) / a;
    Double y1p = (-x1 * sinTheta + y1 * cosTheta) / b;

    /* Compute differences and averages */
    Double dx = x0p - x1p;
    Double dy = y0p - y1p;
    Double xm = (x0p + x1p) / 2;
    Double ym = (y0p + y1p) / 2;
    /* Solve for intersecting unit circles */
    Double dsq = dx * dx + dy * dy;
    if (dsq == 0.0) {
        Logger::W(PathParser::LOGTAG, " Points are coincident");
        return; /* Points are coincident */
    }
    Double disc = 1.0 / dsq - 1.0 / 4.0;
    if (disc < 0.0) {
        Logger::W(PathParser::LOGTAG, "Points are too far apart %f", dsq);
        Float adjust = (Float) (Math::Sqrt(dsq) / 1.99999);
        DrawArc(p, x0, y0, x1, y1, a * adjust,
                b * adjust, theta, isMoreThanHalf, isPositiveArc);
        return; /* Points are too far apart */
    }
    Double s = Math::Sqrt(disc);
    Double sdx = s * dx;
    Double sdy = s * dy;
    Double cx;
    Double cy;
    if (isMoreThanHalf == isPositiveArc) {
        cx = xm - sdy;
        cy = ym + sdx;
    } else {
        cx = xm + sdy;
        cy = ym - sdx;
    }

    Double eta0 = Math::Atan2((y0p - cy), (x0p - cx));

    Double eta1 = Math::Atan2((y1p - cy), (x1p - cx));

    Double sweep = (eta1 - eta0);
    if (isPositiveArc != (sweep >= 0)) {
        if (sweep > 0) {
            sweep -= 2 * Math::PI;
        } else {
            sweep += 2 * Math::PI;
        }
    }

    cx *= a;
    cy *= b;
    Double tcx = cx;
    cx = cx * cosTheta - cy * sinTheta;
    cy = tcx * sinTheta + cy * cosTheta;

    ArcToBezier(p, cx, cy, a, b, x0, y0, thetaD, eta0, sweep);
}

void PathDataNode::ArcToBezier(
    /* [in] */ IPath* p,
    /* [in] */ Double cx,
    /* [in] */ Double cy,
    /* [in] */ Double a,
    /* [in] */ Double b,
    /* [in] */ Double e1x,
    /* [in] */ Double e1y,
    /* [in] */ Double theta,
    /* [in] */ Double start,
    /* [in] */ Double sweep)
{
    using Elastos::Core::Math;

    // Taken from equations at: http://spaceroots.org/documents/ellipse/node8.html
    // and http://www.spaceroots.org/documents/ellipse/node22.html

    // Maximum of 45 degrees per cubic Bezier segment
    Int32 numSegments = Math::Abs((Int32) Math::Ceil(sweep * 4 / Math::PI));

    Double eta1 = start;
    Double cosTheta = Math::Cos(theta);
    Double sinTheta = Math::Sin(theta);
    Double cosEta1 = Math::Cos(eta1);
    Double sinEta1 = Math::Sin(eta1);
    Double ep1x = (-a * cosTheta * sinEta1) - (b * sinTheta * cosEta1);
    Double ep1y = (-a * sinTheta * sinEta1) + (b * cosTheta * cosEta1);

    Double anglePerSegment = sweep / numSegments;
    for (Int32 i = 0; i < numSegments; i++) {
        Double eta2 = eta1 + anglePerSegment;
        Double sinEta2 = Math::Sin(eta2);
        Double cosEta2 = Math::Cos(eta2);
        Double e2x = cx + (a * cosTheta * cosEta2) - (b * sinTheta * sinEta2);
        Double e2y = cy + (a * sinTheta * cosEta2) + (b * cosTheta * sinEta2);
        Double ep2x = -a * cosTheta * sinEta2 - b * sinTheta * cosEta2;
        Double ep2y = -a * sinTheta * sinEta2 + b * cosTheta * cosEta2;
        Double tanDiff2 = Math::Tan((eta2 - eta1) / 2);
        Double alpha =
                Math::Sin(eta2 - eta1) * (Math::Sqrt(4 + (3 * tanDiff2 * tanDiff2)) - 1) / 3;
        Double q1x = e1x + alpha * ep1x;
        Double q1y = e1y + alpha * ep1y;
        Double q2x = e2x - alpha * ep2x;
        Double q2y = e2y - alpha * ep2y;

        p->CubicTo((Float) q1x,
                (Float) q1y,
                (Float) q2x,
                (Float) q2y,
                (Float) e2x,
                (Float) e2y);
        eta1 = eta2;
        e1x = e2x;
        e1y = e2y;
        ep1x = ep2x;
        ep1y = ep2y;
    }
}


//===========================================================================
// PathParser
//===========================================================================

AutoPtr<IPath> PathParser::CreatePathFromPathData(
    /* [in] */ const String& pathData)
{
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    AutoPtr<ArrayOf<PathDataNode*> > nodes = CreateNodesFromPathData(pathData);
    if (nodes != NULL) {
        PathDataNode::NodesToPath(nodes, path);
        return path;
    }
    return NULL;
}

AutoPtr<ArrayOf<PathDataNode*> > PathParser::CreateNodesFromPathData(
    /* [in] */ const String& pathData)
{
    if (pathData.IsNull()) {
        return NULL;
    }

    Int32 start = 0;
    Int32 end = 1;

    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);

    while (end < pathData.GetLength()) {
        end = NextStart(pathData, end);
        String s = pathData.Substring(start, end).Trim();
        if (s.GetLength() > 0) {
            AutoPtr<ArrayOf<Float> > val;
            GetFloats(s, (ArrayOf<Float>**)&val);
            AddNode(list, s.GetChar(0), val);
        }

        start = end;
        end++;
    }
    if ((end - start) == 1 && start < pathData.GetLength()) {
        AutoPtr<ArrayOf<Float> > empty = ArrayOf<Float>::Alloc(0);
        AddNode(list, pathData.GetChar(start), empty);
    }

    AutoPtr<ArrayOf<IInterface*> > array;
    list->ToArray((ArrayOf<IInterface*>**)&array);
    AutoPtr<ArrayOf<PathDataNode*> > result = ArrayOf<PathDataNode*>::Alloc(array->GetLength());
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        PathDataNode* node = (PathDataNode*)(IObject*)IObject::Probe((*array)[i]);
        result->Set(i, node);
    }
    return result;
}

AutoPtr<ArrayOf<PathDataNode*> > PathParser::DeepCopyNodes(
    /* [in] */ ArrayOf<PathDataNode*>* source)
{
    if (source == NULL) {
        return NULL;
    }

    AutoPtr<ArrayOf<PathDataNode*> > copy = ArrayOf<PathDataNode*>::Alloc(source->GetLength());
    for (Int32 i = 0; i < source->GetLength(); i ++) {
        AutoPtr<PathDataNode> node = new PathDataNode((*source)[i]);
        copy->Set(i, node);
    }
    return copy;
}

Boolean PathParser::CanMorph(
    /* [in] */ ArrayOf<PathDataNode*>* nodesFrom,
    /* [in] */ ArrayOf<PathDataNode*>* nodesTo)
{
    if (nodesFrom == NULL || nodesTo == NULL) {
        return FALSE;
    }

    if (nodesFrom->GetLength() != nodesTo->GetLength()) {
        return FALSE;
    }

    for (Int32 i = 0; i < nodesFrom->GetLength(); i ++) {
        if ((*nodesFrom)[i]->mType != (*nodesTo)[i]->mType
                || (*nodesFrom)[i]->mParams->GetLength() != (*nodesTo)[i]->mParams->GetLength()) {
            return FALSE;
        }
    }
    return TRUE;
}

void PathParser::UpdateNodes(
    /* [in] */ ArrayOf<PathDataNode*>* target,
    /* [in] */ ArrayOf<PathDataNode*>* source)
{
    for (Int32 i = 0; i < source->GetLength(); i ++) {
        (*target)[i]->mType = (*source)[i]->mType;
        for (Int32 j = 0; j < (*source)[i]->mParams->GetLength(); j ++) {
            (*target)[i]->mParams->Set(j, (*((*source)[i]->mParams))[j]);
        }
    }
}

Int32 PathParser::NextStart(
    /* [in] */ const String& s,
    /* [in] */ Int32 end)
{
    Char32 c;

    while (end < s.GetLength()) {
        c = s.GetChar(end);
        if ((Int32((c - 'A') * (c - 'Z')) <= 0) || (Int32((c - 'a') * (c - 'z')) <= 0)) {
            return end;
        }
        end++;
    }
    return end;
}

void PathParser::AddNode(
    /* [in] */ IArrayList* list,
    /* [in] */ Char32 cmd,
    /* [in] */ ArrayOf<Float>* val)
{
    AutoPtr<IInterface> obj = (IObject*)new PathDataNode(cmd, val);
    list->Add(obj);
}

ECode PathParser::GetFloats(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Float>** floats)
{
    VALIDATE_NOT_NULL(floats)
    *floats = NULL;

    Char32 firstChar = s.GetChar(0);
    if (firstChar == 'z' || firstChar == 'Z') {
        *floats = ArrayOf<Float>::Alloc(0);
        REFCOUNT_ADD(*floats)
        return NOERROR;
    }

    // try {
    AutoPtr<ArrayOf<Float> > results = ArrayOf<Float>::Alloc(s.GetLength());
    Int32 count = 0;
    Int32 startPosition = 1;
    Int32 endPosition = 0;

    AutoPtr<ExtractFloatResult> result = new ExtractFloatResult();
    Int32 totalLength = s.GetLength();

    // The startPosition should always be the first character of the
    // current number, and endPosition is the character after the current
    // number.
    while (startPosition < totalLength) {
        Extract(s, startPosition, result);
        endPosition = result->mEndPosition;

        if (startPosition < endPosition) {
            FAIL_RETURN(StringUtils::Parse(s.Substring(startPosition, endPosition),
                &(*results)[count++]))
        }

        if (result->mEndWithNegSign) {
            // Keep the '-' sign with next number.
            startPosition = endPosition;
        }
        else {
            startPosition = endPosition + 1;
        }
    }

    return Arrays::CopyOf(results, count, floats);

    // } catch (NumberFormatException e) {
    //     Log.e(LOGTAG, "error in parsing \"" + s + "\"");
    //     throw e;
    // }
}

void PathParser::Extract(
    /* [in] */ const String& s,
    /* [in] */ Int32 start,
    /* [in] */ ExtractFloatResult* result)
{
    // Now looking for ' ', ',' or '-' from the start.
    Int32 currentIndex = start;
    Boolean foundSeparator = FALSE;
    result->mEndWithNegSign = FALSE;
    for (; currentIndex < s.GetLength(); currentIndex++) {
        Char32 currentChar = s.GetChar(currentIndex);
        switch (currentChar) {
            case ' ':
            case ',':
                foundSeparator = TRUE;
                break;
            case '-':
                if (currentIndex != start) {
                    foundSeparator = TRUE;
                    result->mEndWithNegSign = TRUE;
                }
                break;
        }
        if (foundSeparator) {
            break;
        }
    }
    // When there is nothing found, then we put the end position to the end
    // of the string.
    result->mEndPosition = currentIndex;
}


} //Utility
} //Droid
} //Elastos
