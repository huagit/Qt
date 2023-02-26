#include "CustomTitleBar.h"
#include "qmessagebox.h"
CustomTitleBar::CustomTitleBar(QWidget* parent) :QWidget(parent)
{
	p_backgroundImage = new QLabel(this);//背景图片
	p_labelTitle = new QLabel(this);//标题
	p_minButton = new QToolButton(this);//最小化按钮
	p_maxButton = new QToolButton(this);//最大化按钮
	p_closeButton = new QToolButton(this); //关闭按钮
	
	SetTitleBackgroundImage();
	
	this->resize(QSize(900, 600));//初始化窗口显示大小

	QDesktopWidget* desktop = QApplication::desktop();
	this->move((desktop->screen()->width() - this->width()) / 2,
		(desktop->screen()->height() - this->height()) / 2);//程序显示在屏幕中间

	QPixmap  closePix= style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
	p_closeButton->setIcon(closePix);
	maxPix = style()->standardPixmap(QStyle::SP_TitleBarMaxButton);
	p_maxButton->setIcon(maxPix);
	QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
	p_minButton->setIcon(minPix);
	normalPix = style()->standardPixmap(QStyle::SP_TitleBarNormalButton);

	p_minButton->setObjectName("TitleMinButton");
	p_maxButton->setObjectName("TitleMaxButton");
	p_closeButton->setObjectName("TitleCloseButton");

	/******************信号与槽连接**************/
	//最小化按钮
	connect(p_minButton, SIGNAL(clicked(bool)), SLOT(actionMin()));
	//最大化按钮
	connect(p_maxButton, SIGNAL(clicked(bool)), SLOT(actionMax()));
	//关闭按钮  
	connect(p_closeButton,SIGNAL(clicked(bool)),SLOT(actionClose())); 
	p_labelTitle->setText("MainWindow");
	//setAutoFillBackground(true);

	p_labelTitle->setObjectName("WindowTitleString");

	this->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
	this->setWindowFlags(Qt::FramelessWindowHint); //去掉标题栏
	setObjectName("CustomTitleWindow");	

	setMinimumSize(110,50);

	//创建定时器用于每帧更新
	timer = new QTimer();
	timer->setInterval(200);
	QObject::connect(timer,SIGNAL(timeout()),this,SLOT(customUpdate()));
	timer->start();

	//使用Palette
	//setAutoFillBackground(true);
	//p_minButton->setAutoFillBackground(true);
	//p_maxButton->setAutoFillBackground(true);
	//p_closeButton->setAutoFillBackground(true);
	//p_labelTitle->setAutoFillBackground(true);
	//p_backgroundImage->setAutoFillBackground(true);
	//透明标题
	//QPalette pal = palette();
	//pal.setColor(QPalette::Background, QColor(255, 255, 255, 0));
	//p_labelTitle->setPalette(pal);

	UpdateWidgetStyleSheet();
	UpdateTitle();
}

void CustomTitleBar::SetChildWidget(QWidget* child)
{
	if (child&& p_child==NULL)
	{
		p_child = child;
		p_child->setParent(this);
		p_child->show();
		p_child->setGeometry(borderSize, titleHeight + borderSize, this->width() - borderSize * 2, this->height() - titleHeight - borderSize * 2);
	}
}

void CustomTitleBar::UpdateTitle()
{
	//更新位置和大小
	p_backgroundImage->setGeometry(borderSize, borderSize, this->width()- borderSize*2, titleHeight);
	p_closeButton->setGeometry(	this->width() - (titleBottonWidth)	 - borderSize, 2+ borderSize , titleBottonWidth -4, titleHeight-4 );
	p_maxButton->setGeometry(	this->width() - (titleBottonWidth) * 2- borderSize, 2+ borderSize , titleBottonWidth -4, titleHeight-4 );
	p_minButton->setGeometry(	this->width() - (titleBottonWidth) * 3- borderSize, 2+ borderSize , titleBottonWidth -4, titleHeight-4 );
	p_labelTitle->move(10+ borderSize, borderSize);
	if (p_child != NULL)
	{
		p_child->setGeometry(borderSize, titleHeight+ borderSize, this->width()- borderSize*2, this->height()- titleHeight - borderSize * 2);
	}
	//标题栏的图片样式
	switch (bkImageState)
	{
	case 0:
		break;
	case 1:
		p_backgroundImage->setPixmap(bkImage.scaled(this->width(), titleHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
		break;
	case 2:
		break;
	}
	//其他的一些设置
	QFont font("Microsoft YaHei", titleHeight / 2);
	p_labelTitle->setFont(font);
	//更新缩放点位置
	topHit			= QRect(borderSize, 0, this->width() - borderSize * 2, borderSize);
	bottomHit		= QRect(borderSize, this->height() - borderSize, this->width() - borderSize * 2, borderSize);
	leftHit			= QRect(0, borderSize, borderSize, this->height() - borderSize * 2);
	rightHit		= QRect(this->width() - borderSize, borderSize, borderSize, this->height() - borderSize * 2);
	rightBottomHit	= QRect(this->width() - borderSize, this->height() - borderSize, borderSize, borderSize);
	rightTopHit		= QRect(this->width() - borderSize, 0, borderSize, borderSize);
	leftBottomHit	= QRect(0, this->height() - borderSize, borderSize, borderSize);
	leftTopHit		= QRect(0, 0, borderSize, borderSize);
}

QString CustomTitleBar::GetWidgetStyleSheetFromFile(QString objectName, QString path)
{
	QString result;
	bool bFound=false;
	if(!path.isEmpty())
	{
		QFile styleFile(path);
		if (styleFile.open(QFile::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&styleFile);
			QString strLine;
			while (!in.atEnd())
			{
				strLine = in.readLine();//逐行读取
				int length = objectName.length();
				QString objN = "#"+ objectName;
				if (bFound ==true&&!result.isEmpty()&& strLine.contains("};", Qt::CaseSensitive))//是否到底结束位置
				{
					result += strLine.left(strLine.length()-1);
					break;
				}
				if (bFound==false&&strcmp(strLine.left(length+1).toStdString().c_str(), objN.toStdString().c_str())==0)
				{
					bFound = true;
				}
				if (bFound)
				{
					result += strLine;
				}
			}
		}
		else
		{
			QMessageBox::information(this,"Error","Load ["+ objectName+"] style sheet file failed");
		}
		styleFile.close();
	}
	//QMessageBox::information(this, "Check", result);
	return result;
}

void CustomTitleBar::mousePressEvent(QMouseEvent* event)
{
	mousePosition = event->pos();  //当鼠标单击窗体准备拖动时，初始化鼠标在窗体中的相对位置
	if (mousePosition.y() < 0)
		return;

	if (IsInside(rightBottomHit, mousePosition))
	{
		bResizeByRbHit = true;
		this->setCursor(Qt::SizeFDiagCursor);
		QCursor::setPos(this->geometry().bottomRight());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (IsInside(leftBottomHit, mousePosition))
	{
		bResizeByLbHit = true;
		this->setCursor(Qt::SizeBDiagCursor);
		QCursor::setPos(this->geometry().bottomLeft());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (IsInside(rightTopHit, mousePosition))
	{
		bResizeByRtHit = true;
		this->setCursor(Qt::SizeBDiagCursor);
		QCursor::setPos(this->geometry().topRight());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (IsInside(leftTopHit, mousePosition))
	{
		bResizeByLtHit = true;
		this->setCursor(Qt::SizeFDiagCursor);
		QCursor::setPos(this->geometry().topLeft());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (IsInside(topHit, mousePosition))
	{
		bResizeByTopHit = true;
		this->setCursor(Qt::SizeVerCursor);
		QCursor::setPos(QCursor::pos().x(), this->geometry().top());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (IsInside(bottomHit, mousePosition))
	{
		bResizeByBottomHit = true;
		this->setCursor(Qt::SizeVerCursor);
		QCursor::setPos(QCursor::pos().x(), this->geometry().bottom());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (IsInside(leftHit, mousePosition))
	{
		bResizeByLeftHit = true;
		this->setCursor(Qt::SizeHorCursor);
		QCursor::setPos(this->geometry().left(), QCursor::pos().y());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (IsInside(rightHit, mousePosition))
	{
		bResizeByRightHit = true;
		this->setCursor(Qt::SizeHorCursor);
		QCursor::setPos(this->geometry().right(), QCursor::pos().y());
		lastMouseGlobalPosition = QCursor::pos();
		return;
	}
	else if (mousePosition.y() <= titleHeight)
	{
		bIsMousePressed = true;
		return;
	}

}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent* event)
{
	bIsMousePressed = false;
	bResizeByRbHit = false;
	bResizeByRtHit = false;
	bResizeByLbHit = false;
	bResizeByLtHit = false;
	bResizeByTopHit = false;
	bResizeByBottomHit = false;
	bResizeByLeftHit = false;
	bResizeByRightHit = false;
	this->setCursor(Qt::ArrowCursor);
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (mousePosition.y() < 0)
		return;
	if (mousePosition.y() > titleHeight)
		return;
	if (bIsMax)
	{
		this->showNormal();
		bIsMax = false;
		p_maxButton->setIcon(maxPix);
	}
	else
	{
		this->showMaximized();
		bIsMax = true;
		p_maxButton->setIcon(normalPix);
	}

}
void CustomTitleBar::mouseMoveEvent(QMouseEvent* event)
{
	if (bIsMousePressed)
	{
		bResizeByRbHit = false;
		bResizeByRtHit = false;
		bResizeByLbHit = false;
		bResizeByLtHit = false;
		bResizeByTopHit = false;
		bResizeByBottomHit = false;
		bResizeByLeftHit = false;
		bResizeByRightHit = false;
		QPoint movePot = event->globalPos() - mousePosition;
		move(movePot);

		if (bIsMax)
		{
			this->showMaximized();
			bIsMax = true;
			p_maxButton->setIcon(normalPix);
		}

		
	}
	if (bResizeByRbHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x() + width(), this->geometry().y() + height());//保持定点缩放
		QPoint newPoint = event->globalPos()- lastMouseGlobalPosition;
		this->setGeometry(this->geometry().left(), this->geometry().top(), this->geometry().width()+ newPoint.x(), this->geometry().height()+ newPoint.y());
		lastMouseGlobalPosition = event->globalPos();
	}
	else if (bResizeByRtHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x() + width(), this->geometry().y());
		QPoint newPoint = event->globalPos() - lastMouseGlobalPosition;
		if (this->geometry().height() - newPoint.y() < minimumHeight())
			newPoint.setY(0);
		this->setGeometry(this->geometry().left(), this->geometry().top() + newPoint.y(), this->geometry().width() + newPoint.x(), this->geometry().height() - newPoint.y());
		lastMouseGlobalPosition = event->globalPos();
	}
	else if(bResizeByLbHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x(), this->geometry().y() + height());
		QPoint newPoint = event->globalPos() - lastMouseGlobalPosition;
		if (this->geometry().width() - newPoint.x() < minimumWidth())
			newPoint.setX(0);
		this->setGeometry(this->geometry().left() + newPoint.x(), this->geometry().top(), this->geometry().width() - newPoint.x(), this->geometry().height() + newPoint.y());
		lastMouseGlobalPosition = event->globalPos();
	}
	else if (bResizeByLtHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x(), this->geometry().y());
		QPoint newPoint = event->globalPos() - lastMouseGlobalPosition;
		if (this->geometry().width() - newPoint.x() < minimumWidth())
			newPoint.setX(0);
		if (this->geometry().height() - newPoint.y() < minimumHeight())
			newPoint.setY(0);
		this->setGeometry(this->geometry().left() + newPoint.x(), this->geometry().top() + newPoint.y(), this->geometry().width() - newPoint.x(), this->geometry().height() - newPoint.y());
		lastMouseGlobalPosition = event->globalPos();
	}
	else if (bResizeByTopHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x(), this->geometry().y());
		QPoint newPoint = event->globalPos() - lastMouseGlobalPosition;
		if (this->geometry().height() - newPoint.y() < minimumHeight())
			newPoint.setY(0);
		this->setGeometry(this->geometry().left(), this->geometry().top() + newPoint.y(), this->geometry().width(), this->geometry().height() - newPoint.y());
		lastMouseGlobalPosition = event->globalPos();
	}
	else if (bResizeByBottomHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x(), this->geometry().y() + height());
		QPoint newPoint = event->globalPos() - lastMouseGlobalPosition;
		this->setGeometry(this->geometry().left(), this->geometry().top(), this->geometry().width(), this->geometry().height() + newPoint.y());
		lastMouseGlobalPosition = event->globalPos();
	}
	else if (bResizeByLeftHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x(), this->geometry().y());
		QPoint newPoint = event->globalPos() - lastMouseGlobalPosition;
		if (this->geometry().width() - newPoint.x() < minimumWidth())
			newPoint.setX(0);
		this->setGeometry(this->geometry().left() + newPoint.x(), this->geometry().top(), this->geometry().width() - newPoint.x(), this->geometry().height());
		lastMouseGlobalPosition = event->globalPos();
	}
	else if (bResizeByRightHit)
	{
		if (width() <= minimumSize().width() || height() <= minimumSize().height())
			lastMouseGlobalPosition = QPoint(this->geometry().x()+width(), this->geometry().y());
		QPoint newPoint = event->globalPos() - lastMouseGlobalPosition;
		this->setGeometry(this->geometry().left(), this->geometry().top(), this->geometry().width() + newPoint.x(), this->geometry().height());
		lastMouseGlobalPosition = event->globalPos();
	}
}

void CustomTitleBar::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	UpdateTitle();
}

void CustomTitleBar::showEvent(QShowEvent* event)
{
	this->setAttribute(Qt::WA_Mapped);
	QWidget::showEvent(event);
}
void CustomTitleBar::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);//使用QSS

	QWidget::paintEvent(event);
}


void CustomTitleBar::focusOutEvent(QFocusEvent* event)
{
	bIsMousePressed = false;
	bResizeByRbHit = false;
	bResizeByRtHit = false;
	bResizeByLbHit = false;
	bResizeByLtHit = false;
	bResizeByTopHit = false;
	bResizeByBottomHit = false;
	bResizeByLeftHit = false;
	bResizeByRightHit = false;
	QWidget::focusOutEvent(event);
}

void CustomTitleBar::actionMin()
{
	showMinimized();
}

void CustomTitleBar::actionMax()
{
	if (bIsMax)
	{
		this->showNormal();
		bIsMax = false;
		p_maxButton->setIcon(maxPix);
	}
	else
	{
		this->showMaximized();
		bIsMax = true;
		p_maxButton->setIcon(normalPix);
	}
}

void CustomTitleBar::actionClose()
{
	close();    //关闭
}

void CustomTitleBar::customUpdate()
{
	//最好判断下是否允许切换光标，不然每帧总在切换的话会导致缩放卡顿
	QPoint CurrentPos = mapFromGlobal(QCursor::pos());
	if (IsInside(rightBottomHit, CurrentPos)|| bResizeByRbHit)
	{
		if(cursor() != Qt::SizeFDiagCursor)
			this->setCursor(Qt::SizeFDiagCursor);
	}
	else if (IsInside(rightTopHit, CurrentPos)|| bResizeByRtHit)
	{
		if(cursor() != Qt::SizeBDiagCursor)
			this->setCursor(Qt::SizeBDiagCursor);
	}
	else if (IsInside(leftBottomHit, CurrentPos)|| bResizeByLbHit)
	{
		if(cursor() != Qt::SizeBDiagCursor)
			this->setCursor(Qt::SizeBDiagCursor);
	}
	else if (IsInside(leftTopHit, CurrentPos)|| bResizeByLtHit)
	{
		if(cursor() != Qt::SizeFDiagCursor)
			this->setCursor(Qt::SizeFDiagCursor);
	}
	else if (IsInside(topHit, CurrentPos)|| bResizeByTopHit)
	{
		if(cursor() != Qt::SizeVerCursor)
			this->setCursor(Qt::SizeVerCursor);
	}
	else if (IsInside(bottomHit, CurrentPos) || bResizeByBottomHit)
	{
		if(cursor() != Qt::SizeVerCursor)
			this->setCursor(Qt::SizeVerCursor);
	}
	else if (IsInside(leftHit, CurrentPos) || bResizeByLeftHit)
	{
		if(cursor() != Qt::SizeHorCursor)
			this->setCursor(Qt::SizeHorCursor);
	}
	else if (IsInside(rightHit, CurrentPos)|| bResizeByRightHit)
	{
		if(cursor() != Qt::SizeHorCursor)
			this->setCursor(Qt::SizeHorCursor);
	}
	else
	{
		if(cursor() != Qt::ArrowCursor)
			this->setCursor(Qt::ArrowCursor);
	}
}

void CustomTitleBar::UpdateWidgetStyleSheet()
{
	//p_labelTitle->setStyleSheet("#WindowTitleString{color:rgb(200,200,200); }");
	//p_minButton->setStyleSheet("#TitleMinButton{background-color:rgb(130,130,130);color:rgb(255,255,255);border-top-left-radius:5px;border-top-right-radius:5px; }#TitleMinButton:pressed{background-color:rgb(125,125,255);border-top-left-radius:5px;border-top-right-radius:5px;}");
   //p_maxButton->setStyleSheet("#TitleMaxButton{background-color:rgb(130,130,130);color:rgb(255,255,255);border-top-left-radius:5px;border-top-right-radius:5px;}#TitleMaxButton:pressed{background-color:rgb(255,255,125);border-top-left-radius:5px;border-top-right-radius:5px;}");
	//p_closeButton->setStyleSheet("#TitleCloseButton{background-color:rgb(130,130,130) ;color:rgb(255,255,255);border-top-left-radius:5px;border-top-right-radius:5px;}#TitleCloseButton:pressed{background-color:rgb(255,100,100);border-top-left-radius:5px;border-top-right-radius:5px;}");
	//setStyleSheet("#CustomTitleWindow{background-color :rgb(25,25,25);\
	//	border-radius:5px;\
	//	}");
	p_labelTitle->setStyleSheet(GetWidgetStyleSheetFromFile(p_labelTitle->objectName()));
	p_minButton->setStyleSheet(GetWidgetStyleSheetFromFile(p_minButton->objectName()));
	p_maxButton->setStyleSheet(GetWidgetStyleSheetFromFile(p_maxButton->objectName()));
	p_closeButton->setStyleSheet(GetWidgetStyleSheetFromFile(p_closeButton->objectName()));
	this->setStyleSheet(GetWidgetStyleSheetFromFile(this->objectName()));
	//标题栏的图片样式
	switch (bkImageState)
	{
	case 0:
		p_backgroundImage->setStyleSheet("background-color:black;background-image:url(" + bkUrl + ");background-position:center;border-top-left-radius:5px;border-top-right-radius:5px; ");
		break;
	case 1:
		p_backgroundImage->setStyleSheet("background-color:black;border-top-left-radius:5px;border-top-right-radius:5px; ");
		break;
	case 2:
		p_backgroundImage->setStyleSheet("background-color:black;background-image:url(" + bkUrl + ");background-position:top left;border-top-left-radius:5px;border-top-right-radius:5px; ");
		break;
	}

}