#include "photolabel.h"
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>
#include <QFileDialog>

PhotoLabel::PhotoLabel(QWidget *parent):QLabel(parent)
{
    initWidget();
}

/**
* @brief PhotoLabel::initWidget 初始化
*/
void PhotoLabel::initWidget()
{
    //初始化右键菜单
    m_menu = new QMenu(this);
    QAction *loadImage = new QAction;
    loadImage->setText("选择图片");
    connect(loadImage, &QAction::triggered, this, &PhotoLabel::onSelectImage);
    m_menu->addAction(loadImage);
    m_menu->addSeparator();

    QAction *saveImgAction = new QAction;
    saveImgAction->setText("保存");
    connect(saveImgAction, &QAction::triggered, this, &PhotoLabel::onSaveImage);
    m_menu->addAction(saveImgAction);

    QAction *zoomInAction = new QAction;
    zoomInAction->setText("放大");
    connect(zoomInAction, &QAction::triggered, this, &PhotoLabel::onZoomInImage);
    m_menu->addAction(zoomInAction);

    QAction *zoomOutAction = new QAction;
    zoomOutAction->setText("缩小");
    connect(zoomOutAction, &QAction::triggered, this, &PhotoLabel::onZoomOutImage);
    m_menu->addAction(zoomOutAction);

    QAction *presetAction = new QAction;
    presetAction->setText("还原");
    connect(presetAction, &QAction::triggered, this, &PhotoLabel::onPresetImage);
    m_menu->addAction(presetAction);
    m_menu->addSeparator();

    QAction *clearAction = new QAction;
    clearAction->setText("清空");
    connect(clearAction, &QAction::triggered, this, &PhotoLabel::clearShow);
    m_menu->addAction(clearAction);
}

/**
* @brief PhotoLabel::setPhoto 设置要显示的图片
* @param path 图片路径
*/
void PhotoLabel::setPhoto(QString path)
{
    if(path.isEmpty())
    {
        return;
    }

    m_zoomValue = 1.0;
    m_xPtInterval = 0;
    m_yPtInterval = 0;

    m_localFileName = path;
    m_image.load(m_localFileName);
    update();
}


void PhotoLabel::setPhoto(QString name,rxsImg *in){
    QImage::Format Val=QImage::Format::Format_Mono;
    if(in->Channels==1){
        Val=QImage::Format::Format_Mono;
    }else if(in->Channels==3){
        Val=QImage::Format::Format_RGB888;
    }else {
        Val=QImage::Format::Format_ARGB32;
    }
    QImage img((uchar*)in->Data,in->cols,in->rows,Val);
    m_image = img;
    update();
}

/**
* @brief PhotoLabel::clearShow 清空
*/
void PhotoLabel::clearShow()
{
    m_localFileName = "";
    m_image = QImage();
    this->clear();
}

/**
* @brief PhotoLabel::paintEvent 绘图事件
* @param event
*/
void PhotoLabel::paintEvent(QPaintEvent *event)
{
    if(m_image.isNull())
        return QWidget::paintEvent(event);

    QPainter painter(this);

    // 根据窗口计算应该显示的图片的大小
    int width = qMin(m_image.width(), this->width());
    int height = int(width * 1.0 / (m_image.width() * 1.0 / m_image.height()));
    height = qMin(height, this->height());
    width = int(height * 1.0 * (m_image.width() * 1.0 / m_image.height()));

    // 平移
    painter.translate(this->width() / 2 + m_xPtInterval, this->height() / 2 + m_yPtInterval);

    // 缩放
    painter.scale(m_zoomValue, m_zoomValue);

    // 绘制图像
    QRect picRect(-width / 2, -height / 2, width, height);
    painter.drawImage(picRect, m_image);

    QWidget::paintEvent(event);
}

/**
* @brief PhotoLabel::wheelEvent 滚轮滚动缩放图片
* @param event
*/
void PhotoLabel::wheelEvent(QWheelEvent *event)
{
    int value = event->delta();
    if (value > 0)  //放大
        onZoomInImage();
    else            //缩小
        onZoomOutImage();

    update();
}

/**
* @brief PhotoLabel::mousePressEvent 鼠标按下，为移动图片做准备
* @param event
*/
void PhotoLabel::mousePressEvent(QMouseEvent *event)
{
    m_oldPos = event->pos();
    m_pressed = true;
    this->setCursor(Qt::ClosedHandCursor); //设置鼠标样式
}

/**
* @brief PhotoLabel::mouseMoveEvent 鼠标按下后，再移动鼠标，图片随之移动
* @param event
*/
void PhotoLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_pressed)
        return QWidget::mouseMoveEvent(event);

    QPoint pos = event->pos();
    int xPtInterval = pos.x() - m_oldPos.x();
    int yPtInterval = pos.y() - m_oldPos.y();

    m_xPtInterval += xPtInterval;
    m_yPtInterval += yPtInterval;

    m_oldPos = pos;
    update();
}

/**
* @brief PhotoLabel::mouseReleaseEvent 鼠标抬起，图片移动结束
*/
void PhotoLabel::mouseReleaseEvent(QMouseEvent */*event*/)
{
    m_pressed = false;
    this->setCursor(Qt::ArrowCursor); //设置鼠标样式
}

/**
* @brief PhotoLabel::contextMenuEvent 右键显示菜单栏
* @param event
*/
void PhotoLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    m_menu->exec(pos);
}

/**
* @brief PhotoLabel::onSelectImage 选择图片
*/
void PhotoLabel::onSelectImage()
{
    QString path = QFileDialog::getOpenFileName(this, "选择 要显示的图片", "./", tr("Images (*.png *.jpg *.jpeg)"));
    if (path.isEmpty())
        return;

    setPhoto(path);
}

/**
* @brief PhotoLabel::onZoomInImage 图片放大
*/
void PhotoLabel::onZoomInImage()
{
    m_zoomValue += 0.05;
    update();
}

/**
* @brief PhotoLabel::onZoomOutImage 图片缩小
*/
void PhotoLabel::onZoomOutImage()
{
    m_zoomValue -= 0.05;
    if (m_zoomValue <= 0)
    {
        m_zoomValue = 0.05;
        return;
    }

    update();
}

/**
* @brief PhotoLabel::onPresetImage 图片还原
*/
void PhotoLabel::onPresetImage()
{
    m_zoomValue = 1.0;
    m_xPtInterval = 0;
    m_yPtInterval = 0;
    update();
}

void PhotoLabel::onSaveImage()
{
    QString path = QFileDialog::getSaveFileName(this, "另存为:", "./", tr("Images (*.png *.jpg *.jpeg)"));
    if (path.isEmpty())
        return;
    m_image.save(path);
}
