#ifndef __ELASTOS_DROID_UTILITY_PATH_PARSER_H__
#define __ELASTOS_DROID_UTILITY_PATH_PARSER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;
using Elastos::Droid::Graphics::IPath;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * Each PathDataNode represents one command in the "d" attribute of the svg
 * file.
 * An array of PathDataNode can represent the whole "d" attribute.
 */
class PathDataNode
    : public Object
{
public:

    PathDataNode(
        /* [in] */ Char32 type,
        /* [in] */ ArrayOf<Float>* params);

    PathDataNode(
        /* [in] */ PathDataNode* n);

    /**
     * Convert an array of PathDataNode to Path.
     *
     * @param node The source array of PathDataNode.
     * @param path The target Path object.
     */
    static void NodesToPath(
        /* [in] */ ArrayOf<PathDataNode*>* node,
        /* [in] */ IPath* path);

    /**
     * The current PathDataNode will be interpolated between the
     * <code>nodeFrom</code> and <code>nodeTo</code> according to the
     * <code>fraction</code>.
     *
     * @param nodeFrom The start value as a PathDataNode.
     * @param nodeTo The end value as a PathDataNode
     * @param fraction The fraction to interpolate.
     */
    void InterpolatePathDataNode(
        /* [in] */ PathDataNode* nodeFrom,
        /* [in] */ PathDataNode* nodeTo,
        /* [in] */ Float fraction);

private:
    static void AddCommand(
        /* [in] */ IPath* path,
        /* [in] */ ArrayOf<Float>* current,
        /* [in] */ Char32 previousCmd,
        /* [in] */ Char32 cmd,
        /* [in] */ ArrayOf<Float>* val);

    static void DrawArc(
        /* [in] */ IPath* p,
        /* [in] */ Float x0,
        /* [in] */ Float y0,
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [in] */ Float theta,
        /* [in] */ Boolean isMoreThanHalf,
        /* [in] */ Boolean isPositiveArc);

    /**
     * Converts an arc to cubic Bezier segments and records them in p.
     *
     * @param p The target for the cubic Bezier segments
     * @param cx The x coordinate center of the ellipse
     * @param cy The y coordinate center of the ellipse
     * @param a The radius of the ellipse in the horizontal direction
     * @param b The radius of the ellipse in the vertical direction
     * @param e1x E(eta1) x coordinate of the starting point of the arc
     * @param e1y E(eta2) y coordinate of the starting point of the arc
     * @param theta The angle that the ellipse bounding rectangle makes with horizontal plane
     * @param start The start angle of the arc on the ellipse
     * @param sweep The angle (positive or negative) of the sweep of the arc on the ellipse
     */
    static void ArcToBezier(
        /* [in] */ IPath* p,
        /* [in] */ Double cx,
        /* [in] */ Double cy,
        /* [in] */ Double a,
        /* [in] */ Double b,
        /* [in] */ Double e1x,
        /* [in] */ Double e1y,
        /* [in] */ Double theta,
        /* [in] */ Double start,
        /* [in] */ Double sweep);

public:
    Char32 mType;
    AutoPtr<ArrayOf<Float> > mParams;
};

} //Utility
} //Droid
} //Elastos


DEFINE_CONVERSION_FOR(Elastos::Droid::Utility::PathDataNode, IInterface)


namespace Elastos {
namespace Droid {
namespace Utility {


/**
 * @hide
 */
class PathParser
{
private:
    class ExtractFloatResult
        : public Object
    {
    public:
        // We need to return the position of the next separator and whether the
        // next float starts with a '-'.
        Int32 mEndPosition;
        Boolean mEndWithNegSign;
    };

public:
/**
     * @param pathData The string representing a path, the same as "d" string in svg file.
     * @return the generated Path object.
     */
    static AutoPtr<IPath> CreatePathFromPathData(
        /* [in] */ const String& pathData);

    /**
     * @param pathData The string representing a path, the same as "d" string in svg file.
     * @return an array of the PathDataNode.
     */
    static AutoPtr<ArrayOf<PathDataNode*> > CreateNodesFromPathData(
        /* [in] */ const String& pathData);

    /**
     * @param source The array of PathDataNode to be duplicated.
     * @return a deep copy of the <code>source</code>.
     */
    static AutoPtr<ArrayOf<PathDataNode*> > DeepCopyNodes(
        /* [in] */ ArrayOf<PathDataNode*>* source);

    /**
     * @param nodesFrom The source path represented in an array of PathDataNode
     * @param nodesTo The target path represented in an array of PathDataNode
     * @return whether the <code>nodesFrom</code> can morph into <code>nodesTo</code>
     */
    static Boolean CanMorph(
        /* [in] */ ArrayOf<PathDataNode*>* nodesFrom,
        /* [in] */ ArrayOf<PathDataNode*>* nodesTo);

    /**
     * Update the target's data to match the source.
     * Before calling this, make sure canMorph(target, source) is true.
     *
     * @param target The target path represented in an array of PathDataNode
     * @param source The source path represented in an array of PathDataNode
     */
    static void UpdateNodes(
        /* [in] */ ArrayOf<PathDataNode*>* target,
        /* [in] */ ArrayOf<PathDataNode*>* source);

private:
    static Int32 NextStart(
        /* [in] */ const String& s,
        /* [in] */ Int32 end);

    static void AddNode(
        /* [in] */ IArrayList* list,
        /* [in] */ Char32 cmd,
        /* [in] */ ArrayOf<Float>* val);

    /**
     * Parse the floats in the string.
     * This is an optimized version of parseFloat(s.split(",|\\s"));
     *
     * @param s the string containing a command and list of floats
     * @return array of floats
     */
    static CARAPI GetFloats(
        /* [in] */ const String& s,
        /* [out, callee] */ ArrayOf<Float>** floats);

    /**
     * Calculate the position of the next comma or space or negative sign
     * @param s the string to search
     * @param start the position to start searching
     * @param result the result of the extraction, including the position of the
     * the starting position of next number, whether it is ending with a '-'.
     */
    static void Extract(
        /* [in] */ const String& s,
        /* [in] */ Int32 start,
        /* [in] */ ExtractFloatResult* result);

private:
    PathParser();

private:
    friend class PathDataNode;
    static const String LOGTAG;
};

} //Utility
} //Droid
} //Elastos

#endif // __ELASTOS_DROID_UTILITY_PATH_PARSER_H__
