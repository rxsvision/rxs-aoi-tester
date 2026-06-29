#include "photolabel.h"
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>
#include <QFileDialog>
#include <iostream>
#include <QDateTime>


PhotoLabel::PhotoLabel(QWidget *parent):QLabel(parent)
{
    initWidget();
    tmp_imgdata = 0;
    m_difference_x = 0;
    m_difference_y = 0;
    draw_shap = DRAW_NO;
    frame_pen = QPen(QColor(0,174,255),2);
    red_point_pen = QPen(QColor(255,0,0),4);
    timer_id = startTimer(20);
    curRectRoiArea = 0;
    grabKeyboard();
    setMouseTracking(true);
}

PhotoLabel::~PhotoLabel(){
    if(tmp_imgdata!=0){
        free(tmp_imgdata);
        tmp_imgdata=0;
    }
    killTimer(timer_id);
}

void PhotoLabel::initWidget()
{
    //初始化右键菜单
    m_menu = new QMenu(this);
    QAction *loadImage = new QAction;
    loadImage->setText(QString::fromLocal8Bit("选择图片"));
    connect(loadImage, &QAction::triggered, this, &PhotoLabel::onSelectImage);
    m_menu->addAction(loadImage);
    m_menu->addSeparator();

    QAction *saveImgAction = new QAction;
    saveImgAction->setText(QString::fromLocal8Bit("保存"));
    connect(saveImgAction, &QAction::triggered, this, &PhotoLabel::onSaveImage);
    m_menu->addAction(saveImgAction);

    QAction *rectAoi = new QAction;
    rectAoi->setText(QString::fromLocal8Bit("矩形ROI"));
    connect(rectAoi, &QAction::triggered, this, [=](){set_draw_shap(DRAW_SHAP_E::DRAW_RECT);});
    m_menu->addAction(rectAoi);

    QAction *confimAoi = new QAction;
    confimAoi->setText(QString::fromLocal8Bit("确认ROI"));
    connect(confimAoi, &QAction::triggered, this, [=](){set_draw_shap(DRAW_SHAP_E::DRAW_OK);});
    m_menu->addAction(confimAoi);

    QAction *presetAction = new QAction;
    presetAction->setText(QString::fromLocal8Bit("还原"));
    connect(presetAction, &QAction::triggered, this, &PhotoLabel::onPresetImage);
    m_menu->addAction(presetAction);
    m_menu->addSeparator();

    QAction *clearAction = new QAction;
    clearAction->setText(QString::fromLocal8Bit("清空"));
    connect(clearAction, &QAction::triggered, this, &PhotoLabel::clearShow);
    m_menu->addAction(clearAction);
}

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

}


void PhotoLabel::setPhoto(QString name,rxsImg *in){
    if(in ==0 || in->Channels<1 || in->Data==0){return;}
    rxsImg cin = (*in);
    if(tmp_imgdata==0){
        goto cgonext;
    }
    if(tmp_imgdata!=0 && temp_len != cin.Len){
        free(tmp_imgdata);
        tmp_imgdata=0;
        goto cgonext;
    }
    if(tmp_imgdata!=0 && temp_len == cin.Len){
        goto cgonext_1;
    }
    return;
cgonext:
    if(in->Channels!=3){
        imgprocToolbase itb(in);
        cin = itb.GrayToRGB();
    }
    tmp_imgdata = malloc(cin.Len);
    temp_len = cin.Len;

cgonext_1:
    memcpy(tmp_imgdata,cin.Data,cin.Len);
    m_image = QImage((uchar*)tmp_imgdata,cin.cols,cin.rows,QImage::Format::Format_RGB888);
    m_localFileName = name;
}


void PhotoLabel::clearShow()
{
    m_localFileName = "";
    m_image = QImage();
    this->clear();

    draw_shap = DRAW_NO;
    RectRoiAreas.clear();
    EllipseRoiAreas.clear();
}


void PhotoLabel::timerEvent(QTimerEvent *)
{
    this->update();
}


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
    picRect = QRect(-width / 2, -height / 2, width, height);
    painter.translate(this->width() / 2 + m_xPtInterval, this->height() / 2 + m_yPtInterval);
    painter.scale(m_zoomValue, m_zoomValue);
    painter.drawImage(picRect, m_image);
    for(int x=0;x<RectRoiAreas.size();x++){
        uijhRoiArea_Rect *showroi  = &RectRoiAreas[x];
        painter.setPen(frame_pen);//绘制边框线
        painter.drawRect(
            QRect(showroi->rect_left, showroi->rect_top, showroi->rect_width,showroi->rect_height)
            );
        painter.setPen(red_point_pen);//绘制八个点
        painter.drawPoints(showroi->rect_polygon);
    }

    for(int x=0;x<EllipseRoiAreas.size();x++){
        uijhRoiArea_Ellipse *showroi  = &EllipseRoiAreas[x];
        painter.setPen(frame_pen);//绘制边框线
        painter.drawEllipse(
            QRect(
                showroi->ellipse_left,
                showroi->ellipse_top,
                showroi->ellipse_width,
                showroi->ellipse_height));
        painter.setPen(red_point_pen);//绘制四个点
        painter.drawPoints(showroi->ellipse_polygon);
    }
    QWidget::paintEvent(event);
}

void PhotoLabel::wheelEvent(QWheelEvent *event)
{
    if(draw_shap == DRAW_NO){
        int value = event->delta();
        if (value > 0)  //放大
        {
            m_zoomValue += 0.05;
        }
        else            //缩小
        {
            m_zoomValue -= 0.05;
            if (m_zoomValue <= 0)
            {
                m_zoomValue = 0.05;
                return;
            }
        }
    }
}


void PhotoLabel::mousePressEvent(QMouseEvent *event)
{
    m_oldPos = event->pos();
    m_pressed = true;
}


void PhotoLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_pressed){
        m_oldPos = event->pos();
        switch (draw_shap) {
        case (DRAW_RECT)     :rect_mouse_pos = rect_get_mouse_pos(m_oldPos.x(), m_oldPos.y());break;
        case (DRAW_ELLIPSE)  :ellipse_mouse_pos = ellipse_get_mouse_pos(m_oldPos.x(), m_oldPos.y());break;
        default:break;
        }
        return QWidget::mouseMoveEvent(event);
    }

    new_mouse_pos = event->pos();
    if(draw_shap == DRAW_NO){

        int xPtInterval = new_mouse_pos.x() - m_oldPos.x();
        int yPtInterval = new_mouse_pos.y() - m_oldPos.y();

        m_xPtInterval += xPtInterval;
        m_yPtInterval += yPtInterval;

    }else{
            m_difference_x = new_mouse_pos.x() - m_oldPos.x();
            m_difference_y = new_mouse_pos.y() - m_oldPos.y();
            switch (draw_shap) {
                case (DRAW_RECT)     :rect_change_region();break;
                case (DRAW_ELLIPSE)  :ellipse_change_region();break;
                default:break;
            }

    }
    m_oldPos = new_mouse_pos;
    update();
}


void PhotoLabel::mouseReleaseEvent(QMouseEvent */*event*/)
{
    m_pressed = false;
    this->setCursor(Qt::ArrowCursor); //设置鼠标样式
}


void PhotoLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint pos = event->pos();
    pos = this->mapToGlobal(pos);
    m_menu->exec(pos);
}


void PhotoLabel::onSelectImage()
{
    QString path = QFileDialog::getOpenFileName(this, "选择 要显示的图片", "./", tr("Images (*.png *.jpg *.jpeg)"));
    if (path.isEmpty())
        return;

    setPhoto(path);
}

void PhotoLabel::onPresetImage()
{
    m_zoomValue = 1.0;
    m_xPtInterval = 0;
    m_yPtInterval = 0;

}

void PhotoLabel::onSaveImage()
{
    QString path = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("另存为:"), "./", QString::fromLocal8Bit("Images (*.png *.jpg *.jpeg *.bmp)"));
    if (path.isEmpty())
        return;
    m_image.save(path);
}

bool PhotoLabel::ConvertPosToRoipos(int *pos_x,int *pos_y){
    float cy = this->height()/2.0f;
    float cx = this->width()/2.0f;
    (*pos_x) = (*pos_x) - cx;
    (*pos_y) = (*pos_y) - cy;
    (*pos_x) = ((*pos_x) - m_xPtInterval)/m_zoomValue;
    (*pos_y) = ((*pos_y) - m_yPtInterval)/m_zoomValue;
    return true;
}

void PhotoLabel::rect_init_region(bool covert)
{
    if(covert){
        int sx = ((m_oldPos.x() - (this->width()/2)) - m_xPtInterval)/m_zoomValue;
        int sy = ((m_oldPos.y() - (this->height()/2)) - m_yPtInterval)/m_zoomValue;
        curRectRoiArea->rect_left   = sx;
        curRectRoiArea->rect_top    = sy;
        curRectRoiArea->rect_width  = 50 ;
        curRectRoiArea->rect_height = 50 ;
        std::cout<< sx << "," << sy << std::endl;
    }
    rect_mouse_pos = RECT_OUTSIDE;
    rect_update_region();
}

void PhotoLabel::rect_update_region()
{
    curRectRoiArea->rect_top_left_x  = curRectRoiArea->rect_left;
    curRectRoiArea->rect_top_left_y  = curRectRoiArea->rect_top;
    curRectRoiArea->rect_top_right_x = curRectRoiArea->rect_left+curRectRoiArea->rect_width;
    curRectRoiArea->rect_top_right_y = curRectRoiArea->rect_top;
    curRectRoiArea->rect_low_left_x  = curRectRoiArea->rect_left;
    curRectRoiArea->rect_low_left_y  = curRectRoiArea->rect_top+curRectRoiArea->rect_height;
    curRectRoiArea->rect_low_right_x = curRectRoiArea->rect_left+curRectRoiArea->rect_width;
    curRectRoiArea->rect_low_right_y = curRectRoiArea->rect_top+curRectRoiArea->rect_height;

    int Middle_X = curRectRoiArea->rect_left + (curRectRoiArea->rect_width>>1);
    int Middle_Y = curRectRoiArea->rect_top + (curRectRoiArea->rect_height>>1);
    curRectRoiArea->rect_polygon.clear();
    curRectRoiArea->rect_polygon<<QPoint(Middle_X, curRectRoiArea->rect_top)//上中
                 <<QPoint(curRectRoiArea->rect_top_right_x, Middle_Y)//右中
                 <<QPoint(Middle_X, curRectRoiArea->rect_low_left_y)//下中
                 <<QPoint(curRectRoiArea->rect_left, Middle_Y)//左中
                 <<QPoint(curRectRoiArea->rect_left, curRectRoiArea->rect_top)//左上角
                 <<QPoint(curRectRoiArea->rect_top_right_x, curRectRoiArea->rect_top)//右上角
                 <<QPoint(curRectRoiArea->rect_top_right_x, curRectRoiArea->rect_low_left_y)//右下角
                 <<QPoint(curRectRoiArea->rect_left, curRectRoiArea->rect_low_left_y);//左下角

}

void PhotoLabel::rect_change_region()
{
    switch (rect_mouse_pos) {
    case (RECT_UPPER):
        curRectRoiArea->rect_top += m_difference_y;
        curRectRoiArea->rect_height -= m_difference_y;
        break;//上边界
    case (RECT_LOWER):
        curRectRoiArea->rect_height += m_difference_y;
        break; //下边界
    case (RECT_LEFT) :
        curRectRoiArea->rect_left += m_difference_x;
        curRectRoiArea->rect_width -= m_difference_x;
        break;//左边界
    case (RECT_RIGHT):
        curRectRoiArea->rect_width += m_difference_x;
        break; //右边界
    case (RECT_LEFTUPPER) : {//左上角
        curRectRoiArea->rect_top += m_difference_y;
        curRectRoiArea->rect_height -= m_difference_y;
        curRectRoiArea->rect_left += m_difference_x;
        curRectRoiArea->rect_width -= m_difference_x;
    }break;
    case (RECT_LEFTLOWER) : {//左下角
        curRectRoiArea->rect_height += m_difference_y;
        curRectRoiArea->rect_left += m_difference_x;
        curRectRoiArea->rect_width -= m_difference_x;
    }break;
    case (RECT_RIGHTLOWER) : {//右下角
        curRectRoiArea->rect_height += m_difference_y;
        curRectRoiArea->rect_width += m_difference_x;
    }break;
    case (RECT_RIGHTUPPER) : {//右上角
        curRectRoiArea->rect_top += m_difference_y;
        curRectRoiArea->rect_height -= m_difference_y;
        curRectRoiArea->rect_width += m_difference_x;
    }break;
    case (RECT_INSIDE) : {//内部
        curRectRoiArea->rect_top += m_difference_y;
        curRectRoiArea->rect_left += m_difference_x;
    }break;
    case (RECT_OUTSIDE) : return;//外部
    }
    rect_update_region();
}

RECT_MOUSE_POSITION_E PhotoLabel::rect_get_mouse_pos(int pos_x, int pos_y)
{
    ConvertPosToRoipos(&pos_x,&pos_y);
    if (pos_x < (curRectRoiArea->rect_top_left_x)||
        pos_x > (curRectRoiArea->rect_top_right_x)||
        pos_y < (curRectRoiArea->rect_top_left_y) ||
        pos_y > (curRectRoiArea->rect_low_left_y)) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        return RECT_OUTSIDE;
    }else if (pos_y <= (curRectRoiArea->rect_top_left_y+BoundaryRange)){ //1:左上角  2:右上角  3:上边缘
        if(pos_x <= (curRectRoiArea->rect_top_left_x+BoundaryRange)) {
            this->setCursor(QCursor(Qt::SizeFDiagCursor));
            return RECT_LEFTUPPER;
        }
        else if (pos_x >= (curRectRoiArea->rect_top_right_x-BoundaryRange)){
            this->setCursor(QCursor(Qt::SizeBDiagCursor));
            return RECT_RIGHTUPPER;
        }
        else{
            this->setCursor(QCursor(Qt::SizeVerCursor));
            return RECT_UPPER;
        }
    }else if (pos_y >= (curRectRoiArea->rect_low_left_y-BoundaryRange)){ //1:左下角  2:右下角  3:下边缘
        if(pos_x <= (curRectRoiArea->rect_low_left_x+BoundaryRange))
        {
            this->setCursor(QCursor(Qt::SizeBDiagCursor));
            return RECT_LEFTLOWER;
        }
        else if (pos_x >= (curRectRoiArea->rect_low_right_x-BoundaryRange))
        {
            this->setCursor(QCursor(Qt::SizeFDiagCursor));
            return RECT_RIGHTLOWER;
        }
        else
        {
            this->setCursor(QCursor(Qt::SizeVerCursor));
            return RECT_LOWER;
        }
    }else if (pos_x <= (curRectRoiArea->rect_top_left_x+BoundaryRange)) {//左边缘
        this->setCursor(QCursor(Qt::SizeHorCursor));
        return RECT_LEFT;
    }else if (pos_x >= (curRectRoiArea->rect_top_right_x-BoundaryRange)) {//右边缘
        this->setCursor(QCursor(Qt::SizeHorCursor));
        return RECT_RIGHT;
    }else {
        this->setCursor(QCursor(Qt::SizeAllCursor));
        return RECT_INSIDE;
    }
}

void PhotoLabel::ellipse_init_region()
{
    curEllipseRoiArea->ellipse_left= 100;
    curEllipseRoiArea->ellipse_top= 200;
    curEllipseRoiArea->ellipse_width= 101;
    curEllipseRoiArea->ellipse_height = 101;
    ellipse_mouse_pos = ELLIPSE_OUTSIDE;
    ellipse_update_region();
}

void PhotoLabel::ellipse_update_region()
{
    curEllipseRoiArea->ellipse_middle_x = curEllipseRoiArea->ellipse_left + (curEllipseRoiArea->ellipse_width>>1);
    curEllipseRoiArea->ellipse_middle_y = curEllipseRoiArea->ellipse_top + (curEllipseRoiArea->ellipse_height>>1);
    curEllipseRoiArea->ellipse_polygon.clear();
    curEllipseRoiArea->ellipse_polygon<<QPoint(curEllipseRoiArea->ellipse_middle_x,curEllipseRoiArea->ellipse_top)//上顶角
                    <<QPoint(curEllipseRoiArea->ellipse_left+curEllipseRoiArea->ellipse_width,curEllipseRoiArea->ellipse_middle_y)    //右顶角
                    <<QPoint(curEllipseRoiArea->ellipse_middle_x,curEllipseRoiArea->ellipse_top+curEllipseRoiArea->ellipse_height)    //下顶角
                    <<QPoint(curEllipseRoiArea->ellipse_left,curEllipseRoiArea->ellipse_middle_y);//左顶角
}

void PhotoLabel::ellipse_change_region()
{
    switch (ellipse_mouse_pos) {
    case (ELLIPSE_UPPER):
        curEllipseRoiArea->ellipse_top += m_difference_y;
        curEllipseRoiArea->ellipse_height -= m_difference_y;
        break;
    case (ELLIPSE_LOWER):
        curEllipseRoiArea->ellipse_height += m_difference_y;
        break;
    case (ELLIPSE_LEFT):
        curEllipseRoiArea->ellipse_left += m_difference_x;
        curEllipseRoiArea->ellipse_width -= m_difference_x;
        break;
    case (ELLIPSE_RIGHT):
        curEllipseRoiArea->ellipse_width += m_difference_x;
        break;
    case (ELLIPSE_INSIDE):
        curEllipseRoiArea->ellipse_top += m_difference_y;
        curEllipseRoiArea->ellipse_left += m_difference_x;
        break;
    case (ELLIPSE_OUTSIDE): return;
    }
    ellipse_update_region();
}

ELLIPSE_MOUSE_POSITION_E PhotoLabel::ellipse_get_mouse_pos(int pos_x, int pos_y)
{
    ConvertPosToRoipos(&pos_x,&pos_y);
    if (pos_x < curEllipseRoiArea->ellipse_left
        || pos_x > (curEllipseRoiArea->ellipse_left+curEllipseRoiArea->ellipse_width)
        || pos_y < curEllipseRoiArea->ellipse_top
        || pos_y > (curEllipseRoiArea->ellipse_top+curEllipseRoiArea->ellipse_height)) {
        this->setCursor(QCursor(Qt::ArrowCursor));
        return ELLIPSE_OUTSIDE;
    }else if (pos_y <= curEllipseRoiArea->ellipse_top+BoundaryRange){ //上顶角
        if (pos_x >= (curEllipseRoiArea->ellipse_middle_x-3) && pos_x <= (curEllipseRoiArea->ellipse_middle_x+3)){
            this->setCursor(QCursor(Qt::SizeVerCursor));
            return ELLIPSE_UPPER;
        }else{
            this->setCursor(QCursor(Qt::SizeAllCursor));
            return ELLIPSE_INSIDE;
        }
    }else if (pos_y >= curEllipseRoiArea->ellipse_top+curEllipseRoiArea->ellipse_height-BoundaryRange){ //下顶角
        if (pos_x >= (curEllipseRoiArea->ellipse_middle_x-3) && pos_x <= (curEllipseRoiArea->ellipse_middle_x+3)) {
            this->setCursor(QCursor(Qt::SizeVerCursor));
            return ELLIPSE_LOWER;
        }else{
            this->setCursor(QCursor(Qt::SizeAllCursor));
            return ELLIPSE_INSIDE;
        }
    }else if (pos_x <= curEllipseRoiArea->ellipse_left+BoundaryRange) {   //左顶角
        if (pos_y >= (curEllipseRoiArea->ellipse_middle_y-3) && pos_y <= (curEllipseRoiArea->ellipse_middle_y+3)) {
            this->setCursor(QCursor(Qt::SizeHorCursor));
            return ELLIPSE_LEFT;
        }else{
            this->setCursor(QCursor(Qt::SizeAllCursor));
            return ELLIPSE_INSIDE;
        }
    }else if (pos_x >= curEllipseRoiArea->ellipse_left+curEllipseRoiArea->ellipse_width-BoundaryRange) {  //右顶角
        if (pos_y >= (curEllipseRoiArea->ellipse_middle_y-3) && pos_y <= (curEllipseRoiArea->ellipse_middle_y+3)) {
            this->setCursor(QCursor(Qt::SizeHorCursor));
            return ELLIPSE_RIGHT;
        }else{
            this->setCursor(QCursor(Qt::SizeAllCursor));
            return ELLIPSE_INSIDE;
        }
    }else {
        this->setCursor(QCursor(Qt::SizeAllCursor));
        return ELLIPSE_INSIDE;
    }
}

bool PhotoLabel::ConvertRoiposToPicpos(int *sx,int *sy,int *w,int *h){
    int px,py,pw,ph;
    picRect.getRect(&px,&py,&pw,&ph);
    float zoomH = ph*1.0/(m_image.height()*1.0);
    float zoomW = pw*1.0/(m_image.width()*1.0);
    std::cout<<"rate:"<<zoomH <<"," <<zoomW<<","<<m_zoomValue<< " xy-offset:" << m_xPtInterval <<"," <<m_yPtInterval<<std::endl;

    *w = int((*w)*1.0)/zoomW;
    *h = int((*h)*1.0)/zoomH;
    int nx = int((*sx)*1.0/zoomW);
    int ny = int((*sy)*1.0/zoomH);

    nx = nx + m_image.width()/2;
    ny = ny + m_image.height()/2;
    std::cout<<"new roi=>"<<(*sx) <<"," <<(*sy)<<" | "<< nx << "," << ny << "   WH:"<< (*w)<<","<<(*h) <<std::endl;
    WorkProcessDal *wrkpd = WorkProcessDalGet();
    return wrkpd->RoiMangerAreaSet((roiGrpMangerStep *)sbb,RectRoiAreas.size()-1,nx,ny,(*w),(*h));
}


void PhotoLabel::set_draw_shap(DRAW_SHAP_E d)
{
    if(DRAW_OK == d){
        int sx = curRectRoiArea->rect_left;
        int sy = curRectRoiArea->rect_top;
        int rw = curRectRoiArea->rect_width;
        int rh = curRectRoiArea->rect_height;
        switch (draw_shap) {
        case DRAW_RECT:
            ConvertRoiposToPicpos(&sx,&sy,&rw,&rh);
            break;

        case DRAW_ELLIPSE:
            break;

        default:
            break;
        }
        draw_shap = DRAW_NO;
        return;
    }else
    {
        switch(d)
        {
        case DRAW_RECT:{
            uijhRoiArea_Rect rar;
            RectRoiAreas.push_back(rar);
            curRectRoiArea = &RectRoiAreas[RectRoiAreas.size()-1];
            rect_init_region();
            break;
        }
        case DRAW_ELLIPSE:
            ellipse_init_region();
            break;
        }
    }
    draw_shap = d;
}

void PhotoLabel::set_StepModel(StepBlockBase *sbbptr){
    sbb = sbbptr;
}

void PhotoLabel::set_RoiArea(int sx,int sy,int w,int h){
    int px,py,pw,ph;
    picRect.getRect(&px,&py,&pw,&ph);
    float zoomH = ph*1.0/(m_image.height()*1.0);
    float zoomW = pw*1.0/(m_image.width()*1.0);

    w = w * zoomW;
    h = h * zoomH;
    sx = sx * zoomW - (this->width()/2);
    sy = sy * zoomH - (this->height()/2);

    uijhRoiArea_Rect rar={sx,sy,w,h,0,0,0,0,0,0,0,0};
    RectRoiAreas.push_back(rar);
    curRectRoiArea = &RectRoiAreas[RectRoiAreas.size()-1];
    rect_init_region();
}

void PhotoLabel::Save(const char *path){
    QDateTime now = QDateTime::currentDateTime();
    QString fn = QString::fromLocal8Bit(path);
    fn += now.toString("yyyyMMddhhmmsszzz");
    fn += ".bmp";
    m_image.save(fn);
}
