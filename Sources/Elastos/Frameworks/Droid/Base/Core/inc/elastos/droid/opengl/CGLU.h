
#ifndef __ELASTOS_DROID_OPENGL_CGLU_H__
#define __ELASTOS_DROID_OPENGL_CGLU_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CGLU.h"

#include <elastos/coredef.h>
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

using Elastosx::Microedition::Khronos::Opengles::IGL10;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLU)
    , public Singleton
    , public IGLU
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

/**
 * Return an error string from a GL or GLU error code.
 *
 * @param error - a GL or GLU error code.
 * @return the error string for the input error code, or NULL if the input
 *         was not a valid GL or GLU error code.
 */
    CARAPI GluErrorString(
        /* [in] */ Int32 error,
        /* [out] */ String* str);

/**
 * Define a viewing transformation in terms of an eye point, a center of
 * view, and an up vector.
 *
 * @param gl a GL10 interface
 * @param eyeX eye point X
 * @param eyeY eye point Y
 * @param eyeZ eye point Z
 * @param centerX center of view X
 * @param centerY center of view Y
 * @param centerZ center of view Z
 * @param upX up vector X
 * @param upY up vector Y
 * @param upZ up vector Z
 */
    CARAPI GluLookAt(
        /* [in] */ IGL10* gl,
        /* [in] */ Float eyeX,
        /* [in] */ Float eyeY,
        /* [in] */ Float eyeZ,
        /* [in] */ Float centerX,
        /* [in] */ Float centerY,
        /* [in] */ Float centerZ,
        /* [in] */ Float upX,
        /* [in] */ Float upY,
        /* [in] */ Float upZ);

/**
 * Set up a 2D orthographic projection matrix
 *
 * @param gl
 * @param left
 * @param right
 * @param bottom
 * @param top
 */
    CARAPI GluOrtho2D(
        /* [in] */ IGL10* gl,
        /* [in] */ Float left,
        /* [in] */ Float right,
        /* [in] */ Float bottom,
        /* [in] */ Float top);

/**
 * Set up a perspective projection matrix
 *
 * @param gl a GL10 interface
 * @param fovy specifies the field of view angle, in degrees, in the Y
 *        direction.
 * @param aspect specifies the aspect ration that determins the field of
 *        view in the x direction. The aspect ratio is the ratio of x
 *        (width) to y (height).
 * @param zNear specifies the distance from the viewer to the near clipping
 *        plane (always positive).
 * @param zFar specifies the distance from the viewer to the far clipping
 *        plane (always positive).
 */
    CARAPI GluPerspective(
        /* [in] */ IGL10* gl,
        /* [in] */ Float fovy,
        /* [in] */ Float aspect,
        /* [in] */ Float zNear,
        /* [in] */ Float zFar);

/**
 * Map object coordinates into window coordinates. gluProject transforms the
 * specified object coordinates into window coordinates using model, proj,
 * and view. The result is stored in win.
 * <p>
 * Note that you can use the OES_matrix_get extension, if present, to get
 * the current modelView and projection matrices.
 *
 * @param objX object coordinates X
 * @param objY object coordinates Y
 * @param objZ object coordinates Z
 * @param model the current modelview matrix
 * @param modelOffset the offset into the model array where the modelview
 *        maxtrix data starts.
 * @param project the current projection matrix
 * @param projectOffset the offset into the project array where the project
 *        matrix data starts.
 * @param view the current view, {x, y, width, height}
 * @param viewOffset the offset into the view array where the view vector
 *        data starts.
 * @param win the output vector {winX, winY, winZ}, that returns the
 *        computed window coordinates.
 * @param winOffset the offset into the win array where the win vector data
 *        starts.
 * @return A return value of GL_TRUE indicates success, a return value of
 *         CARAPI gl_FALSE indicates failure.
 */
    CARAPI GluProject(
        /* [in] */ Float objX,
        /* [in] */ Float objY,
        /* [in] */ Float objZ,
        /* [in] */ ArrayOf<Float>* model,
        /* [in] */ Int32 modelOffset,
        /* [in] */ ArrayOf<Float>* project,
        /* [in] */ Int32 projectOffset,
        /* [in] */ ArrayOf<Int32>* view,
        /* [in] */ Int32 viewOffset,
        /* [in] */ ArrayOf<Float>* win,
        /* [in] */ Int32 winOffset,
        /* [out] */ Int32* rst);

/**
 * Map window coordinates to object coordinates. gluUnProject maps the
 * specified window coordinates into object coordinates using model, proj,
 * and view. The result is stored in obj.
 * <p>
 * Note that you can use the OES_matrix_get extension, if present, to get
 * the current modelView and projection matrices.
 *
 * @param winX window coordinates X
 * @param winY window coordinates Y
 * @param winZ window coordinates Z
 * @param model the current modelview matrix
 * @param modelOffset the offset into the model array where the modelview
 *        maxtrix data starts.
 * @param project the current projection matrix
 * @param projectOffset the offset into the project array where the project
 *        matrix data starts.
 * @param view the current view, {x, y, width, height}
 * @param viewOffset the offset into the view array where the view vector
 *        data starts.
 * @param obj the output vector {objX, objY, objZ}, that returns the
 *        computed object coordinates.
 * @param objOffset the offset into the obj array where the obj vector data
 *        starts.
 * @return A return value of GL10.GL_TRUE indicates success, a return value
 *         of GL10.GL_FALSE indicates failure.
 */
    CARAPI GluUnProject(
        /* [in] */ Float winX,
        /* [in] */ Float winY,
        /* [in] */ Float winZ,
        /* [in] */ ArrayOf<Float>* model,
        /* [in] */ Int32 modelOffset,
        /* [in] */ ArrayOf<Float>* project,
        /* [in] */ Int32 projectOffset,
        /* [in] */ ArrayOf<Int32>* view,
        /* [in] */ Int32 viewOffset,
        /* [in] */ ArrayOf<Float>* obj,
        /* [in] */ Int32 objOffset,
        /* [out] */ Int32* rst);

private:
    static AutoPtr<ArrayOf<Float> > sScratch;
    static Object sLock;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLU_H__
