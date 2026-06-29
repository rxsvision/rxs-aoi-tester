#ifndef MYCCLABEL_H
#define MYCCLABEL_H

#include "cc2DLabel.h"
#include <QFontMetrics>

//自定义的2D标签
class gpp2DLabel : public ccHObject, public ccInteractor
{
public:
    gpp2DLabel(QString name = QString("gppDefLabel"));
    QString getName() const override;
    inline CC_CLASS_ENUM getClassID() const override { return CC_TYPES::LABEL_2D; }
    inline bool isSerializable() const override { return true; }

    inline QString getRawName() const { return m_name; }

    //inherited from ccInteractor
    virtual bool acceptClick(int x, int y, Qt::MouseButton button) override;
    virtual bool move2D(int x, int y, int dx, int dy, int screenWidth, int screenHeight) override;

    void setPosition(float x, float y);
    inline const float* getPosition() const { return m_screenPos; }
    void clear(bool ignoreDependencies = false);

    bool addPickedPoint(ccGenericPointCloud* cloud, unsigned pointIndex, bool entityCenter = false);
    bool addPickedPoint(ccGenericMesh* mesh, unsigned triangleIndex, const CCVector2d& uv, bool entityCenter = false);
    bool addPickedPoint(const cc2DLabel::PickedPoint& pp);

    void bodyContentSet(QString str);
    void bodyContentSet(QStringList str);
protected:
    virtual void drawMeOnly(CC_DRAW_CONTEXT& context) override;
    virtual void onDeletionOf(const ccHObject* obj) override;

    void drawMeOnly2D(CC_DRAW_CONTEXT& context);
    void drawMeOnly3D(CC_DRAW_CONTEXT& context);

private:
    cc2DLabel::PickedPoint m_pickedPoints;
    bool m_showFullBody;
    QStringList body;
    //标签的大小区域
    QRect m_labelROI;
    QRect m_closeButtonROI;

    //标签的位置，屏幕的百分比
    float m_screenPos[2];

    //标签最后显示的位置，(绝对位置)
    int m_lastScreenPos[2];

    bool m_dispIn2D;

    //! Relative marker scale
    float m_relMarkerScale;
};










#endif // MYCCLABEL_H
