
#ifndef PHOTOLABEL_H
#define PHOTOLABEL_H
#include "rxsFixdal.h"
#include <QObject>
#include <QLabel>
#include <QMenu>
#include <qpoint.h>
#include <qpen.h>



typedef enum draw_shap_e{
    DRAW_RECT,    //画矩形
    DRAW_ELLIPSE, //画椭圆
    DRAW_NO,      //不画
    DRAW_OK
}DRAW_SHAP_E;

/* 用来表示鼠标在矩形区域的位置信息
 *
 */
typedef enum rect_mouse_position_e{
    RECT_UPPER=0,     //上边缘
    RECT_LOWER=1,     //下边缘
    RECT_LEFT,        //左边缘
    RECT_RIGHT,       //右边缘
    RECT_LEFTUPPER,   //左上角
    RECT_LEFTLOWER,   //左下角
    RECT_RIGHTLOWER,  //右下角
    RECT_RIGHTUPPER,  //右上角
    RECT_INSIDE,      //区域内部
    RECT_OUTSIDE      //区域外部
}RECT_MOUSE_POSITION_E;

/* 用来表示鼠标在椭圆区域的位置信息
 *
 */
typedef enum ellipse_mouse_position_e{
    ELLIPSE_UPPER=0,     //上顶角
    ELLIPSE_LOWER=1,     //下顶角
    ELLIPSE_LEFT,        //左顶角
    ELLIPSE_RIGHT,       //右顶角
    ELLIPSE_INSIDE,      //区域内部
    ELLIPSE_OUTSIDE      //区域外部
}ELLIPSE_MOUSE_POSITION_E;




class PhotoLabel : public QLabel
{
public:
    explicit PhotoLabel(QWidget *parent = nullptr);
    ~PhotoLabel();
    void setPhoto(QString);  //设置图片
    void setPhoto(QString name,rxsImg *);

    void clearShow();   //清空显示
    void set_draw_shap(DRAW_SHAP_E d);
    void set_StepModel(StepBlockBase *sbb);
    void set_RoiArea(int sx,int sy,int w,int h);

    void Save(const char *path);
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;   //右键菜单
    void timerEvent(QTimerEvent*) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void initWidget();      //初始化
    void onSelectImage();   //选择打开图片
    void onPresetImage();   //图片还原
    void onSaveImage();   //图片保存

private:
    int temp_len;
    void *tmp_imgdata;
    QImage m_image;           //显示的图片
    qreal m_zoomValue = 1.0;  //鼠标缩放值
    int m_xPtInterval = 0;    //平移X轴的值
    int m_yPtInterval = 0;    //平移Y轴的值
    QPoint m_oldPos;          //旧的鼠标位置
    bool m_pressed = false;   //鼠标是否被摁压
    QString m_localFileName;  //文件名称
    QMenu *m_menu;            //右键菜单

    DRAW_SHAP_E draw_shap;
    int                              m_difference_x;
    int                              m_difference_y;
    QPoint                           new_mouse_pos;
    QPen                             frame_pen;       //用来绘制区域边框
    QPen                             red_point_pen;   //用来绘制红色点
    const int                        BoundaryRange = 6;//用来表示边界的宽度范围，用于拖拽

    /* 椭圆区域相关变量和函数
     * 注意:drawEllipse(20,20,210,160);
     * 第1，2个参数表示圆/椭圆距屏幕左上角的像素数，第3,4个参数表示圆/椭圆的宽度和高度。
     * 更加确切地表述，这个圆或椭圆是在矩形中，这个矩形的左上角的顶点在坐标轴中的位置为（20，20），
     * 这个圆或椭圆的中心为这个矩形的中心
     */
    typedef struct {
        int        ellipse_left;        //表示椭圆右上角的X坐标
        int        ellipse_top;         //表示椭圆右上角的Y坐标
        int        ellipse_width;       //表示椭圆的宽(即水平长度)
        int        ellipse_height;      //表示椭圆的高(即垂直长度)
        int        ellipse_middle_x;
        int        ellipse_middle_y;
        QPolygon                         ellipse_polygon;          //装载4个红点的坐标
    }uijhRoiArea_Ellipse;
    /* 矩形区域相关变量和函数 注意:QPoint的0点是左上角，横向向右为X的正方向，竖向向下为Y的正方向 */
    typedef struct {
        int        rect_left;        //表示矩形右上角的X坐标
        int        rect_top;         //表示矩形右上角的Y坐标
        int        rect_width;       //表示矩形的宽(即水平长度)
        int        rect_height;      //表示矩形的高(即垂直长度)
        int        rect_top_left_x,   rect_top_left_y;       //左上
        int        rect_top_right_x,  rect_top_right_y;      //右上
        int        rect_low_left_x,   rect_low_left_y;       //左下
        int        rect_low_right_x,  rect_low_right_y;      //右下
        QPolygon                      rect_polygon;          //装载8个红点的坐标
    }uijhRoiArea_Rect;

    RECT_MOUSE_POSITION_E         rect_mouse_pos;
    void                          rect_init_region(bool covert=true);
    void                          rect_update_region();
    void                          rect_change_region();
    RECT_MOUSE_POSITION_E         rect_get_mouse_pos(int pos_x, int pos_y);

    ELLIPSE_MOUSE_POSITION_E         ellipse_mouse_pos;
    void                             ellipse_init_region();
    void                             ellipse_update_region();
    void                             ellipse_change_region();
    ELLIPSE_MOUSE_POSITION_E         ellipse_get_mouse_pos(int pos_x, int pos_y);
    int                 timer_id;

    QRect picRect;
    bool ConvertPosToRoipos(int *x,int *y);
    bool ConvertRoiposToPicpos(int *sx,int *sy,int *w,int *h);

    uijhRoiArea_Rect *curRectRoiArea;
    uijhRoiArea_Ellipse *curEllipseRoiArea;

    std::vector<uijhRoiArea_Rect> RectRoiAreas;
    std::vector<uijhRoiArea_Ellipse> EllipseRoiAreas;

    StepBlockBase *sbb;
};

#endif // PHOTOLABEL_H
