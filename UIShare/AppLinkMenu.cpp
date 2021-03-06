#include "AppLinkMenu.h"
#include "Config/Config.h"
#include <QTime>
#include "Notify/Notify.h"
AppLinkMenu::AppLinkMenu(AppListInterface * pList,QWidget *parent) : QWidget(parent)
{
	m_pList = pList;
	int title_height=40;
    int status_height=ui_res_width/9;
    int center_height=ui_res_height-status_height-title_height;
    int margin=10;
    int inter=5;
    setWindowFlags(Qt::FramelessWindowHint);//
    setGeometry(0,0,ui_res_width,ui_res_height);
    this->setAutoFillBackground(true);
    QPixmap pixmap(":/images/mainmenu.png");
    QPixmap fitpixmap=pixmap.scaled(this->width(),this->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(fitpixmap));
    this->setPalette(palette);

    m_pIcon=new QLabel(this);
    m_pIcon->setGeometry(margin*2,inter,title_height-inter*2,title_height-inter*2);
    m_pIcon->setStyleSheet(QString::fromUtf8("border-image:url(:/images/home.png)"));
    m_pTime=new QLabel(this);
    m_pTime->setGeometry(ui_res_width*1/3,inter,ui_res_width/3,title_height-inter*2);
    m_pTime->setAlignment(Qt::AlignCenter);
	
	m_pTime->setStyleSheet("font: 55 30pt \"Liberation Serif\";color:rgb(255,255,255);background:transparent;border: 0px");
	m_pTime->setText(QTime::currentTime().toString("HH:mm"));

    m_pNetStatus=new QLabel(this);
    m_pNetStatus->setGeometry(ui_res_width-title_height-margin,inter,title_height-2*inter,title_height-inter*2);
    m_pNetStatus->setStyleSheet(QString::fromUtf8("border-image:url(:/images/wifi.png)"));

    m_pCenter=new QWidget(this);
    m_pCenter->setGeometry(margin,title_height,ui_res_width-2*margin,center_height);
    m_pCenter->setStyleSheet(QString::fromUtf8("border-image:url(:/images/applinkmain.png)"));

    int y=title_height+center_height;
    m_pMainMenu=new QWidget(this);
    m_pMainMenu->setGeometry(margin,y,ui_res_width-2*margin,status_height-inter);
    m_pMainMenu->setStyleSheet(QString::fromUtf8("border-image:url(:/images/downmenu.png)"));
    QString sheet_on[MENU_MAX]={":/images/music_on.png",":/images/key_on.png",":/images/phone_on.png",
                                ":/images/link_on.png",":/images/list_on.png",
                               ":/images/setting_on.png"};
    QString sheet_off[MENU_MAX]={":/images/music_off.png",":/images/key_off.png",":/images/phone_off.png",
                                 ":/images/link_off.png",":/images/list_off.png",
                                ":/images/setting_off.png"};
    QString text[MENU_MAX]={"音频","空调","电话","导航","应用程序","设置"};
    for(int i=0;i<MENU_MAX;i++){
        m_pMenuTab[i]=new MenuButton(m_pMainMenu);
        m_pMenuTab[i]->setGeometry((ui_res_width-2*margin)/MENU_MAX*i,0,(ui_res_width-2*margin)/MENU_MAX,status_height);
        m_pMenuTab[i]->setIcon(sheet_on[i],sheet_off[i]);
        m_pMenuTab[i]->setText(text[i]);
        connect(m_pMenuTab[i],SIGNAL(clicked()),SLOT(onMenuSelected()));
    }
    m_pMenuTab[4]->setActive(true);

    m_AppWidth=(ui_res_width-2*margin)/4;
    m_AppHeight=center_height/2;
    QString appsheet_on[2]={":/images/phonechild_on.png",":/images/listchild_on.png"};
    QString appsheet_off[2]={":/images/phonechild_off.png",":/images/listchild_off.png"};
    QString childText[2]={"连接设备","连接移动应用程序"};
    int funcId[2]={-1,-2};
    for(int i=0;i<2;i++){
        InsertChildApp(i,funcId[i],childText[i],appsheet_on[i],appsheet_off[i]);
    }

    m_pTimer=new QTimer(this);
    m_pTimer->start(1000);//分钟
    connect(m_pTimer,SIGNAL(timeout()),SLOT(onUpdateTime()));

}

AppLinkMenu::~AppLinkMenu()
{
    delete m_pIcon;
    delete m_pTime;
    delete m_pNetStatus;
    delete m_pCenter;
    delete m_pMainMenu;
	delete m_pTimer;
	for (int i = 0; i < MENU_MAX; i++){
		delete m_pMenuTab[i];
	}
}

QWidget* AppLinkMenu::CenterWidget()
{
    return m_pCenter;
}

void AppLinkMenu::onMenuSelected()
{
    MenuButton *button=static_cast<MenuButton*>(sender());
    if(NULL==button)
        return;
    for(int i=0;i<MENU_MAX;i++){
        if(button==m_pMenuTab[i]){
            m_pMenuTab[i]->setActive(true);
        }
        else{
            m_pMenuTab[i]->setActive(false);
        }
    }
}

void AppLinkMenu::onChildAppSelected(int funcId)
{
    if(funcId==-1){
        Notify n(this);
        n.showDlg();
    }
    else if(funcId==-2){

    }
    else
    {
         m_pList->OnAppActivated(funcId);
    }
}


void AppLinkMenu::onUpdateTime()
{
    m_pTime->setText(QTime::currentTime().toString("HH:mm"));
}

void AppLinkMenu::InsertChildApp(int index,int appId,QString text,QString on,QString off,bool bPaint)
{
    for(int i=index;i<m_pChildApps.size();i++){
        CAppButton *button=m_pChildApps.at(i);
        int r=(i+1)/4;
        int c=(i+1)%4;
        button->setGeometry(5+m_AppWidth*c,5+m_AppHeight*r,m_AppWidth-10,m_AppHeight-10);
    }
    CAppButton *newbutton=new CAppButton(m_pCenter);
    int r=index/4;
    int c=index%4;
	newbutton->setGeometry(5 + m_AppWidth*c, 5 + m_AppHeight*r, m_AppWidth - 10, m_AppHeight - 10);
	newbutton->setFuncId(appId);
    newbutton->setIcon(on, off,bPaint);
	newbutton->setText(text);
    newbutton->show();
    connect(newbutton, SIGNAL(clickedWitchFuncId(int)), SLOT(onChildAppSelected(int)));
	m_pChildApps.insert(index, newbutton);
}

void AppLinkMenu::DeleteChildApp(int index)
{
    CAppButton *button=m_pChildApps.at(index);
    m_pChildApps.removeAt(index);
    disconnect(button,SIGNAL(clickedWitchFuncId(int)),this,SLOT(onChildAppSelected(int)));
    delete button;
    for(int i=index;i<m_pChildApps.size();i++){
        CAppButton *button=m_pChildApps.at(i);
        int r=i/4;
        int c=i%4;
        button->setGeometry(5+m_AppWidth*c,5+m_AppHeight*r,m_AppWidth-10,m_AppHeight-10);
    }
}


void AppLinkMenu::showEvent(QShowEvent * e)
{
    std::vector<int> vAppIDs;
    std::vector<std::string> vAppNames;
    std::vector<std::string> vIconPath;
    m_pList->getAppList(vAppIDs, vAppNames,vIconPath);
    int count=m_pChildApps.size();
    for(int i=2;i<count;i++){
        DeleteChildApp(2);
    }

    if(vAppIDs.size()>0){

        for(int i=0;i<vAppIDs.size();i++){
            InsertChildApp(2+i,vAppIDs.at(i),vAppNames.at(i).c_str(),vIconPath.at(i).c_str(),vIconPath.at(i).c_str(),true);
        }
    }

    //repaint();
    //this->show();
}
