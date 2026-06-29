#include "myCCLabel.h"
#include "ccIncludeGL.h"

//#include "ccBasicTypes.h"
#include "ccGenericGLDisplay.h"
#include "ccGenericPointCloud.h"
//#include "ccPointCloud.h"
#include "ccGenericMesh.h"
//#include "ccScalarField.h"
#include "ccSphere.h"

//Qt
#include <QSharedPointer>

//System
#include <assert.h>
#include <string.h>


gpp2DLabel::gpp2DLabel(QString name/*=QString()*/)
    : ccHObject(name.isEmpty() ? "gpplabel" : name)
    , m_showFullBody(true)
    , m_dispIn2D(true)
    , m_relMarkerScale(1.0f)
{
    m_screenPos[0] = m_screenPos[1] = 0.05f;
    clear(false);
    lockVisibility(false);
    setEnabled(true);
}

QString gpp2DLabel::getName() const
{
    QString processedName = m_name;
    return processedName;
}

void gpp2DLabel::bodyContentSet(QString str){
    body.append(str);
}

void gpp2DLabel::bodyContentSet(QStringList str){
    body = str;
}

void gpp2DLabel::setPosition(float x, float y)
{
    m_screenPos[0] = x;
    m_screenPos[1] = y;
}

bool gpp2DLabel::move2D(int x, int y, int dx, int dy, int screenWidth, int screenHeight)
{
    assert(screenHeight > 0 && screenWidth > 0);

    m_screenPos[0] += static_cast<float>(dx) / screenWidth;
    m_screenPos[1] += static_cast<float>(dy) / screenHeight;

    return true;
}

bool gpp2DLabel::addPickedPoint(ccGenericPointCloud* cloud, unsigned pointIndex, bool entityCenter/*=false*/)
{
    if (!cloud || pointIndex >= cloud->size())
            return false;

    cc2DLabel::PickedPoint pp;
    pp._cloud = cloud;
    pp.index = pointIndex;
    pp.entityCenterPoint = entityCenter;

    return addPickedPoint(pp);
}

bool gpp2DLabel::addPickedPoint(ccGenericMesh* mesh, unsigned triangleIndex, const CCVector2d& uv, bool entityCenter/*=false*/)
{
    if (!mesh || triangleIndex >= mesh->size())
            return false;

    cc2DLabel::PickedPoint pp;
    pp._mesh = mesh;
    pp.index = triangleIndex;
    pp.uv = uv;
    pp.entityCenterPoint = entityCenter;

    return addPickedPoint(pp);
}

bool gpp2DLabel::addPickedPoint(const cc2DLabel::PickedPoint& pp)
{
    m_pickedPoints = pp;
    if (pp.entity())
            pp.entity()->addDependency(this, DP_NOTIFY_OTHER_ON_DELETE);
    return true;
}


void gpp2DLabel::clear(bool ignoreDependencies)
{
    m_lastScreenPos[0] = m_lastScreenPos[1] = -1;
    m_labelROI = QRect(0, 0, 0, 0);
    setVisible(false);
    setName("gppLabel");
}


bool gpp2DLabel::acceptClick(int x, int y, Qt::MouseButton button)
{
    if (button == Qt::RightButton)
    {
        if (m_labelROI.contains(x - m_lastScreenPos[0], y - m_lastScreenPos[1]))
        {
            //toggle collapse state
            m_showFullBody = !m_showFullBody;
            return true;
        }
    }
    return false;
}

void gpp2DLabel::drawMeOnly(CC_DRAW_CONTEXT& context)
{
    //2D foreground only
    if (!MACRO_Foreground(context))
        return;

    //Not compatible with virtual transformation (see ccDrawableObject::enableGLTransformation)
    if (MACRO_VirtualTransEnabled(context))
        return;

    if (MACRO_Draw3D(context))
        drawMeOnly3D(context);
    else if (MACRO_Draw2D(context))
        drawMeOnly2D(context);
}

static QSharedPointer<ccSphere> c_unitPointMarker(nullptr);

void gpp2DLabel::drawMeOnly3D(CC_DRAW_CONTEXT& context)
{

    //get the set of OpenGL functions (version 2.1)
    QOpenGLFunctions_2_1 *glFunc = context.glFunctions<QOpenGLFunctions_2_1>();
    if (glFunc == nullptr)
    {
        assert(false);
        return;
    }

    //standard case: list names pushing
    bool pushName = MACRO_DrawEntityNames(context);
    if (pushName)
    {
        //not particularly fast
        if (MACRO_DrawFastNamesOnly(context))
            return;
        glFunc->glPushName(getUniqueIDForDisplay());
    }

    //we always project the points in 2D (maybe useful later, even when displaying the label during the 2D pass!)
    ccGLCameraParameters camera;
    //we can't use the context 'ccGLCameraParameters' (viewport, modelView matrix, etc. )
    //because it doesn't take the temporary 'GL transformation' into account!
    //context.display->getGLCameraParameters(camera);
    glFunc->glGetIntegerv(GL_VIEWPORT, camera.viewport);
    glFunc->glGetDoublev(GL_PROJECTION_MATRIX, camera.projectionMat.data());
    glFunc->glGetDoublev(GL_MODELVIEW_MATRIX, camera.modelViewMat.data());

    //project the point in 2D
    CCVector3 P3D = m_pickedPoints.getPointPosition();
    camera.project(P3D, m_pickedPoints.pos2D);
        //display point marker as spheres

            if (!c_unitPointMarker)
            {
                c_unitPointMarker = QSharedPointer<ccSphere>(new ccSphere(1.0f, nullptr, "PointMarker", 12));
                c_unitPointMarker->showColors(true);
                c_unitPointMarker->setVisible(true);
                c_unitPointMarker->setEnabled(true);
            }

            //build-up point maker own 'context'
            CC_DRAW_CONTEXT markerContext = context;
            markerContext.drawingFlags &= (~CC_DRAW_ENTITY_NAMES); //we must remove the 'push name flag' so that the sphere doesn't push its own!
            markerContext.display = nullptr;

            if (isSelected() && !pushName)
                c_unitPointMarker->setTempColor(ccColor::red);
            else
                c_unitPointMarker->setTempColor(context.labelDefaultMarkerCol);

            const ccViewportParameters& viewportParams = context.display->getViewportParameters();

                glFunc->glMatrixMode(GL_MODELVIEW);
                glFunc->glPushMatrix();
                CCVector3 P = m_pickedPoints.getPointPosition();
                ccGL::Translate(glFunc, P.x, P.y, P.z);
                float scale = context.labelMarkerSize * m_relMarkerScale;
                if (viewportParams.perspectiveView && viewportParams.zFar > 0)
                {
                    //in perspective view, the actual scale depends on the distance to the camera!
                    double d = (camera.modelViewMat * CCVector3d::fromArray(P.u)).norm();
                    double unitD = viewportParams.zFar / 2; //we consider that the 'standard' scale is at half the depth
                    scale = static_cast<float>(scale * sqrt(d / unitD)); //sqrt = empirical (probably because the marker size is already partly compensated by ccGLWindow::computeActualPixelSize())
                }
                glFunc->glScalef(scale, scale, scale);
                c_unitPointMarker->draw(markerContext);
                glFunc->glPopMatrix();


    if (pushName)
    {
        glFunc->glPopName();
    }
}

void gpp2DLabel::onDeletionOf(const ccHObject* obj)
{
    ccHObject::onDeletionOf(obj); //remove dependencies, etc.
    //check that associated clouds are not about to be deleted!
    //size_t pointsToRemove = 0;
    if(m_pickedPoints.entity() == obj){
        clear(true);
    }
}

//display parameters
static const int c_margin = 5;
//static const int c_tabMarginX = 5;
//static const int c_tabMarginY = 2;
static const int c_arrowBaseSize = 3;
//static const int c_buttonSize = 10;

static const ccColor::Rgba c_darkGreen(0, 200, 0, 255);


void gpp2DLabel::drawMeOnly2D(CC_DRAW_CONTEXT& context)
{
    QOpenGLFunctions_2_1 *glFunc = context.glFunctions<QOpenGLFunctions_2_1>();
    if (glFunc == nullptr)
    {
        assert(false);
        return;
    }

    //standard case: list names pushing
    bool pushName = MACRO_DrawEntityNames(context);
    if (pushName)
    {
        glFunc->glPushName(getUniqueIDForDisplay());
    }

    float halfW = context.glW / 2.0f;
    float halfH = context.glH / 2.0f;

    if (!m_dispIn2D)
    {
        //nothing to do
        if (pushName)
        {
            glFunc->glPopName();
        }
        return;
    }

    //label title
    //const int precision = context.dispNumberPrecision;
    QString title = m_name;//getTitle(precision);
    //render zoom
    int margin        = static_cast<int>(c_margin        * context.renderZoom);
    //int tabMarginX    = static_cast<int>(c_tabMarginX    * context.renderZoom);
    //int tabMarginY    = static_cast<int>(c_tabMarginY    * context.renderZoom);
    int arrowBaseSize = static_cast<int>(c_arrowBaseSize * context.renderZoom);
    int rowHeight = 0;
    int titleHeight = 0;
    QFont bodyFont;
    QFont titleFont;

    if (!pushName)
    {
        /*** label border ***/
        bodyFont = context.display->getLabelDisplayFont(); //takes rendering zoom into account!
        titleFont = bodyFont; //takes rendering zoom into account!
        //titleFont.setBold(true);

        QFontMetrics titleFontMetrics(titleFont);
        titleHeight = titleFontMetrics.height();

        QFontMetrics bodyFontMetrics(bodyFont);
        rowHeight = bodyFontMetrics.height();

        //get label box dimension
        int dx = 100;
        int dy = 0;
        //base box dimension
        dx = std::max(dx, titleFontMetrics.width(title));
        dy += margin;		//top vertical margin
        dy += titleHeight;	//title

        if (m_showFullBody)
        {

            //body = getLabelContent(precision);
            if (!body.empty())
            {
                dy += margin;	//vertical margin above separator
                for (int j = 0; j < body.size(); ++j)
                {
                    dx = std::max(dx, bodyFontMetrics.width(body[j]));
                    dy += rowHeight; //body line height
                }
                dy += margin;	//vertical margin below text
            }
        }

        dx += margin * 2;	// horizontal margins


        m_labelROI = QRect(0, 0, dx, dy);

        int buttonSize    = static_cast<int>(10 * context.renderZoom);
        m_closeButtonROI = QRect(dx-margin-buttonSize,margin,dx-margin,margin+buttonSize);
        //m_closeButtonROI.right()   = dx-margin;
        //m_closeButtonROI.left()    = m_closeButtonROI.right()-buttonSize;
        //m_closeButtonROI.bottom()  = margin;
        //m_closeButtonROI.top()     = m_closeButtonROI.bottom()+buttonSize;

        //automatically elide the title
        //title = titleFontMetrics.elidedText(title, Qt::ElideRight, m_closeButtonROI[0] - 2 * margin);
    }

    //draw label rectangle
    const int xStart = static_cast<int>(context.glW * m_screenPos[0]);
    const int yStart = static_cast<int>(context.glH * (1.0f - m_screenPos[1]));

    m_lastScreenPos[0] = xStart;
    m_lastScreenPos[1] = yStart - m_labelROI.height();

    //colors
    bool highlighted = (!pushName && isSelected());
    //default background color
    unsigned char alpha = static_cast<unsigned char>((context.labelOpacity / 100.0) * 255);
    ccColor::Rgbaub defaultBkgColor(context.labelDefaultBkgCol, alpha);
    //default border color (mustn't be totally transparent!)
    ccColor::Rgbaub defaultBorderColor(ccColor::red, 255);
    if (!highlighted)
    {
        //apply only half of the transparency
        unsigned char halfAlpha = static_cast<unsigned char>((50.0 + context.labelOpacity / 200.0) * 255);
        defaultBorderColor = ccColor::Rgbaub(context.labelDefaultBkgCol, halfAlpha);
    }

    glFunc->glPushAttrib(GL_COLOR_BUFFER_BIT);
    glFunc->glEnable(GL_BLEND);

    glFunc->glMatrixMode(GL_MODELVIEW);
    glFunc->glPushMatrix();
    glFunc->glTranslatef(static_cast<GLfloat>(xStart - halfW), static_cast<GLfloat>(yStart - halfH), 0);

    if (!pushName)
    {
        //compute arrow base position relatively to the label rectangle (for 0 to 8)
        int arrowBaseConfig = 0;

        //compute arrow head position
        CCVector3d arrowDest2D(0, 0, 0);

        arrowDest2D += m_pickedPoints.pos2D;
        arrowDest2D /= static_cast<PointCoordinateType>(1);

        int iArrowDestX = static_cast<int>(arrowDest2D.x - xStart);
        int iArrowDestY = static_cast<int>(arrowDest2D.y - yStart);
        {
            if (iArrowDestX < m_labelROI.left()) //left
                arrowBaseConfig += 0;
            else if (iArrowDestX > m_labelROI.right()) //Right
                arrowBaseConfig += 2;
            else  //Middle
                arrowBaseConfig += 1;

            if (iArrowDestY > -m_labelROI.top()) //Top
                arrowBaseConfig += 0;
            else if (iArrowDestY < -m_labelROI.bottom()) //Bottom
                arrowBaseConfig += 6;
            else  //Middle
                arrowBaseConfig += 3;
        }

        //we make the arrow base start from the nearest corner
        if (arrowBaseConfig != 4) //4 = label above point!
        {
            glFunc->glColor4ubv(defaultBorderColor.rgba);
            glFunc->glBegin(GL_TRIANGLE_FAN);
            glFunc->glVertex2i(iArrowDestX, iArrowDestY);
            switch (arrowBaseConfig)
            {
            case 0: //top-left corner
                glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.top() - 2 * arrowBaseSize);
                glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.top());
                glFunc->glVertex2i(m_labelROI.left() + 2 * arrowBaseSize, -m_labelROI.top());
                break;
            case 1: //top-middle edge
                glFunc->glVertex2i(std::max(m_labelROI.left(), iArrowDestX - arrowBaseSize), -m_labelROI.top());
                glFunc->glVertex2i(std::min(m_labelROI.right(), iArrowDestX + arrowBaseSize), -m_labelROI.top());
                break;
            case 2: //top-right corner
                glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.top() - 2 * arrowBaseSize);
                glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.top());
                glFunc->glVertex2i(m_labelROI.right() - 2 * arrowBaseSize, -m_labelROI.top());
                break;
            case 3: //middle-left edge
                glFunc->glVertex2i(m_labelROI.left(), std::min(-m_labelROI.top(), iArrowDestY + arrowBaseSize));
                glFunc->glVertex2i(m_labelROI.left(), std::max(-m_labelROI.bottom(), iArrowDestY - arrowBaseSize));
                break;
            case 4: //middle of rectangle!
                break;
            case 5: //middle-right edge
                glFunc->glVertex2i(m_labelROI.right(), std::min(-m_labelROI.top(), iArrowDestY + arrowBaseSize));
                glFunc->glVertex2i(m_labelROI.right(), std::max(-m_labelROI.bottom(), iArrowDestY - arrowBaseSize));
                break;
            case 6: //bottom-left corner
                glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.bottom() + 2 * arrowBaseSize);
                glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.bottom());
                glFunc->glVertex2i(m_labelROI.left() + 2 * arrowBaseSize, -m_labelROI.bottom());
                break;
            case 7: //bottom-middle edge
                glFunc->glVertex2i(std::max(m_labelROI.left(), iArrowDestX - arrowBaseSize), -m_labelROI.bottom());
                glFunc->glVertex2i(std::min(m_labelROI.right(), iArrowDestX + arrowBaseSize), -m_labelROI.bottom());
                break;
            case 8: //bottom-right corner
                glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.bottom() + 2 * arrowBaseSize);
                glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.bottom());
                glFunc->glVertex2i(m_labelROI.right() - 2 * arrowBaseSize, -m_labelROI.bottom());
                break;
            }
            glFunc->glEnd();
        }
    }

    //main rectangle
    glFunc->glColor4ubv(defaultBkgColor.rgba);
    glFunc->glBegin(GL_QUADS);
    glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.top());
    glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.bottom());
    glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.bottom());
    glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.top());
    glFunc->glEnd();

    //if (highlighted)
    {
        glFunc->glPushAttrib(GL_LINE_BIT);
        glFunc->glLineWidth(3.0f * context.renderZoom);
        glFunc->glColor4ubv(defaultBorderColor.rgba);
        glFunc->glBegin(GL_LINE_LOOP);
        glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.top());
        glFunc->glVertex2i(m_labelROI.left(), -m_labelROI.bottom());
        glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.bottom());
        glFunc->glVertex2i(m_labelROI.right(), -m_labelROI.top());
        glFunc->glEnd();
        glFunc->glPopAttrib(); //GL_LINE_BIT
    }

//    //draw close button
//    glFunc->glColor4ubv(ccColor::black.rgba);
//    glFunc->glBegin(GL_LINE_LOOP);
//    glFunc->glVertex2i(m_closeButtonROI.left(),-m_closeButtonROI.top());
//    glFunc->glVertex2i(m_closeButtonROI.left(),-m_closeButtonROI.bottom());
//    glFunc->glVertex2i(m_closeButtonROI.right(),-m_closeButtonROI.bottom());
//    glFunc->glVertex2i(m_closeButtonROI.right(),-m_closeButtonROI.top());
//    glFunc->glEnd();
//    glFunc->glBegin(GL_LINES);
//    glFunc->glVertex2i(m_closeButtonROI.left()+2,-m_closeButtonROI.top()+2);
//    glFunc->glVertex2i(m_closeButtonROI.right()-2,-m_closeButtonROI.bottom()-2);
//    glFunc->glVertex2i(m_closeButtonROI.right()-2,-m_closeButtonROI.top()+2);
//    glFunc->glVertex2i(m_closeButtonROI.left()+2,-m_closeButtonROI.bottom()-2);
//    glFunc->glEnd();

    //display text
    if (!pushName)
    {
        int xStartRel = margin;
        int yStartRel = 0;
        yStartRel -= titleHeight;

        ccColor::Rgba defaultTextColor;
        if (context.labelOpacity < 40)
        {
            //under a given opacity level, we use the default text color instead!
            defaultTextColor = context.textDefaultCol;
        }
        else
        {
            defaultTextColor = ccColor::Rgba(	255 - context.labelDefaultBkgCol.r,
                                             255 - context.labelDefaultBkgCol.g,
                                             255 - context.labelDefaultBkgCol.b,
                                             context.labelDefaultBkgCol.a);
        }

        //label title
        context.display->displayText(title,
                                     xStart + xStartRel,
                                     yStart + yStartRel,
                                     ccGenericGLDisplay::ALIGN_DEFAULT,
                                     0,
                                     &defaultTextColor,
                                     &titleFont);
        yStartRel -= margin;

        if (m_showFullBody)
        {
            if (!body.empty())
            {
                //display body
                yStartRel -= margin;
                for (int i = 0; i < body.size(); ++i)
                {
                    yStartRel -= rowHeight;
                    context.display->displayText(body[i], xStart + xStartRel, yStart + yStartRel, ccGenericGLDisplay::ALIGN_DEFAULT, 0, &defaultTextColor, &bodyFont);
                }
            }

        }
    }

    glFunc->glPopAttrib(); //GL_COLOR_BUFFER_BIT

    glFunc->glPopMatrix();

    if (pushName)
    {
        glFunc->glPopName();
    }
}


