#include "customWidgets/UBGlobals.h"
#include "core/UBApplication.h"
#include "frameworks/UBFileSystemUtils.h"
#include "gui/UBMediaPlayer.h"
#include "customWidgets/UBMediaWidget.h"

#include "UBTBPageEditWidget.h"

UBTBPageEditWidget::UBTBPageEditWidget(UBTeacherBarDataMgr *pDataMgr, QWidget *parent, const char *name):QWidget(parent)
  , mpDataMgr(NULL)
  , mpTitleLabel(NULL)
  , mpTitle(NULL)
  , mpMediaLabel(NULL)
  , mpActionLabel(NULL)
  , mpActions(NULL)
  , mpActionButton(NULL)
  , mpLinkLabel(NULL)
  , mpLinks(NULL)
  , mpLinkButton(NULL)
  , mpCommentLabel(NULL)
  , mpComments(NULL)
  , mpDocumentEditbutton(NULL)
  , mpPagePreviewButton(NULL)
  , mpContainer(NULL)
{
    Q_UNUSED(name);
    mpDataMgr = pDataMgr;
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(UBApplication::globalStyleSheet());

    mLayout.setContentsMargins(0, 0, 0, 0);
    setLayout(&mLayout);

    mpContainer = new QWidget(this);
    mpContainer->setObjectName("DockPaletteWidgetBox");
    mpContainer->setLayout(&mContainerLayout);
    mLayout.addWidget(mpContainer, 1);

    // Title
    mpTitleLabel = new QLabel(tr("Title"), mpContainer);
    mpTitle = new QLineEdit(mpContainer);
    mpTitle->setObjectName("DockPaletteWidgetLineEdit");
    connect(mpTitle, SIGNAL(textChanged(const QString&)), this, SLOT(onTitleTextChanged(const QString&)));
    mContainerLayout.addWidget(mpTitleLabel, 0);
    mContainerLayout.addWidget(mpTitle, 0);

    // Actions
    mpActionLabel = new QLabel(tr("Actions"), mpContainer);
    mContainerLayout.addWidget(mpActionLabel, 0);
    mpActions = new UBWidgetList(mpContainer);
    mpActions->setEmptyText(tr("Add actions"));
    mContainerLayout.addWidget(mpActions, 1);
    mpActionButton = new QPushButton(mpContainer);
    mpActionButton->setObjectName("DockPaletteWidgetButton");
    mpActionButton->setText(tr("Add action"));
    mActionLayout.addWidget(mpActionButton, 0);
    mActionLayout.addStretch(1);
    mContainerLayout.addLayout(&mActionLayout, 0);

    // Media
    mpMediaLabel = new QLabel(tr("Medias"), mpContainer);
    mContainerLayout.addWidget(mpMediaLabel, 0);
    mpMediaContainer = new UBTBMediaContainer(mpContainer);
    mpMediaContainer->setEmptyText(tr("Drop media here"));
    mContainerLayout.addWidget(mpMediaContainer, 1);

    // Links
    mpLinkLabel = new QLabel(tr("Links"), mpContainer);
    mContainerLayout.addWidget(mpLinkLabel, 0);
    mpLinks = new UBWidgetList(mpContainer);
    mContainerLayout.addWidget(mpLinks, 1);
    mpLinkButton = new QPushButton(tr("Add link"), mpContainer);
    mpLinkButton->setObjectName("DockPaletteWidgetButton");
    mLinkLayout.addWidget(mpLinkButton, 0);
    mLinkLayout.addStretch(1);
    mContainerLayout.addLayout(&mLinkLayout, 0);

    // Comments
    mpCommentLabel = new QLabel(tr("Comments"), mpContainer);
    mContainerLayout.addWidget(mpCommentLabel, 0);
    mpComments = new QTextEdit(mpContainer);
    mpComments->setObjectName("DockPaletteWidgetBox");
    mpComments->setStyleSheet("background:white;");
    mContainerLayout.addWidget(mpComments, 1);

    mpPagePreviewButton = new QPushButton(tr("Preview"), this);
    mpPagePreviewButton->setObjectName("DockPaletteWidgetButton");
    mpDocumentEditbutton = new QPushButton(tr("Document View"), this);
    mpDocumentEditbutton->setObjectName("DockPaletteWidgetButton");
    mPagePreviewLayout.addWidget(mpDocumentEditbutton, 0);
    mPagePreviewLayout.addWidget(mpPagePreviewButton, 0);
    mPagePreviewLayout.addStretch(1);
    mLayout.addLayout(&mPagePreviewLayout, 0);

    connect(mpTitle, SIGNAL(textChanged(QString)), this, SLOT(onValueChanged()));
    connect(mpActionButton, SIGNAL(clicked()), this, SLOT(onActionButton()));
    connect(mpLinkButton, SIGNAL(clicked()), this, SLOT(onLinkButton()));
    connect(mpDocumentEditbutton, SIGNAL(clicked()), this, SLOT(onDocumentEditClicked()));
    connect(mpPagePreviewButton, SIGNAL(clicked()), this, SLOT(onPagePreviewClicked()));
    connect(mpMediaContainer, SIGNAL(mediaDropped(QString)), this, SLOT(onMediaDropped(QString)));
}

UBTBPageEditWidget::~UBTBPageEditWidget()
{
    DELETEPTR(mpDocumentEditbutton);
    DELETEPTR(mpPagePreviewButton);
    DELETEPTR(mpComments);
    DELETEPTR(mpCommentLabel);
    DELETEPTR(mpLinks);
    DELETEPTR(mpLinkLabel);
    DELETEPTR(mpLinkButton);
    DELETEPTR(mpMediaLabel);
    DELETEPTR(mpActionButton);
    DELETEPTR(mpActionLabel);
    DELETEPTR(mpTitleLabel);
    DELETEPTR(mpTitle);
}

void UBTBPageEditWidget::onValueChanged()
{
    mpDataMgr->setPageTitle(mpTitle->text());
    mpDataMgr->setComments(mpComments->document()->toPlainText());
    emit valueChanged();
}

void UBTBPageEditWidget::onActionButton()
{
    UBTeacherStudentAction* pAction = new UBTeacherStudentAction(this);
    mpDataMgr->actions() << pAction;
    mpActions->addWidget(pAction);
}

void UBTBPageEditWidget::onLinkButton()
{
    UBUrlWidget* pUrl = new UBUrlWidget(this);
    mpDataMgr->urls() << pUrl;
    mpLinks->addWidget(pUrl);
}

void UBTBPageEditWidget::onMediaDropped(const QString &url)
{
    if("" != url){
        QWidget* pMedia = mpMediaContainer->generateMediaWidget(url);
        if(NULL != pMedia){
            mpDataMgr->medias() << pMedia;
            mpMediaContainer->addWidget(pMedia);
        }
    }
}


void UBTBPageEditWidget::saveInfos(sTeacherBarInfos *infos)
{
    if(NULL != infos){
        infos->title = mpTitle->text();

        // Actions
        for(int i=0; i<mpDataMgr->actions().size(); i++){
            infos->actions << QString("%0;%1").arg(mpDataMgr->actions().at(i)->comboValue()).arg(mpDataMgr->actions().at(i)->text());
        }
        // Media
        foreach(QString media, mpMediaContainer->mediaUrls()){
            infos->medias << media;
        }

        // Links
        for(int j=0; j<mpDataMgr->urls().size(); j++){
            if("" != mpDataMgr->urls().at(j)->url()){
                infos->urls << mpDataMgr->urls().at(j)->url();
            }
        }
        // Comments
        infos->comments = mpComments->document()->toPlainText();
    }
}

void UBTBPageEditWidget::loadInfos(sTeacherBarInfos* infos)
{
    if(NULL != infos){
        // Title
        mpTitle->setText(infos->title);
        mpDataMgr->setPageTitle(infos->title);

        // Actions
        for(int i=0; i<infos->actions.size(); i++){
            QStringList qslAction = infos->actions.at(i).split(";");
            if(qslAction.size() >= 2){
                UBTeacherStudentAction* pAction = new UBTeacherStudentAction(this);
                pAction->setComboValue(qslAction.at(0).toInt());
                pAction->setText(qslAction.at(1));
                mpDataMgr->actions() << pAction;
                mpActions->addWidget(pAction);
            }
        }
        // Media
        foreach(QString url, infos->medias){
            if("" != url){
                QWidget* pMedia = mpMediaContainer->generateMediaWidget(url);
                if(NULL != pMedia){
                    mpDataMgr->medias() << pMedia;
                    mpMediaContainer->addWidget(pMedia);
                }
            }
        }

        // Links
        for(int j=0; j<infos->urls.size(); j++){
            QString qsUrl = infos->urls.at(j);
            if("" != qsUrl){
                UBUrlWidget* pLink = new UBUrlWidget(this);
                pLink->setUrl(qsUrl);
                mpDataMgr->urls() << pLink;
                mpLinks->addWidget(pLink);
            }
        }
        // Comments
        if(NULL != mpComments){
            mpComments->document()->setPlainText(infos->comments);
        }
    }
}

void UBTBPageEditWidget::onDocumentEditClicked()
{
    emit changeTBState(eTeacherBarState_DocumentEdit);
}

void UBTBPageEditWidget::onPagePreviewClicked()
{
    emit changeTBState(eTeacherBarState_PagePreview);
}

// ---------------------------------------------------------------------------------------------
UBUrlWidget::UBUrlWidget(QWidget *parent, const char *name):QWidget(parent)
  , mpLayout(NULL)
  , mpUrlLabel(NULL)
  , mpUrl(NULL)
{
    setObjectName(name);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(UBApplication::globalStyleSheet());

    mpLayout = new QVBoxLayout(this);
    setLayout(mpLayout);

    mpLabelLayout = new QHBoxLayout(this);
    mpUrlLabel = new QLabel(tr("Url"), this);
    mpLabelLayout->addWidget(mpUrlLabel, 0);
    mpUrl = new QLineEdit(this);
    mpUrl->setObjectName("DockPaletteWidgetLineEdit");
    mpUrl->setMinimumHeight(20);
    mpLabelLayout->addWidget(mpUrl, 1);

    mpTitleLayout = new QHBoxLayout(this);
    mpTitleLabel = new QLabel(tr("Title"),this);
    mpTitleLayout->addWidget(mpTitleLabel,0);
    mpTitle = new QLineEdit(this);
    mpTitle->setObjectName("DockPaletteWidgetLineEdit");
    mpTitle->setMinimumHeight(20);
    mpTitleLayout->addWidget(mpTitle,1);

    mpLayout->addLayout(mpTitleLayout);
    mpLayout->addLayout(mpLabelLayout);
}

UBUrlWidget::~UBUrlWidget()
{
    DELETEPTR(mpTitle);
    DELETEPTR(mpTitleLabel);
    DELETEPTR(mpUrlLabel);
    DELETEPTR(mpUrl);
    DELETEPTR(mpTitleLayout);
    DELETEPTR(mpLabelLayout);
    DELETEPTR(mpLayout);
}

QString UBUrlWidget::url()
{
    QString str;

    if(NULL != mpUrl){
        str = mpUrl->text() + ";" + mpTitle->text();
    }

    return str;
}

void UBUrlWidget::setUrl(const QString &url)
{
    QStringList list = url.split(";");
    if(NULL != mpUrl){
        mpUrl->setText(list.at(0));
        mpTitle->setText(list.at(1));
    }
}

//  ------------------------------------------------------------------------------------------------------------------------------------
UBTBMediaContainer::UBTBMediaContainer(QWidget *parent, const char *name) : UBWidgetList(parent)
{
    setObjectName(name);
    setAcceptDrops(true);
}

UBTBMediaContainer::~UBTBMediaContainer()
{

}

void UBTBMediaContainer::dropEvent(QDropEvent* pEvent)
{
    QPixmap pixFromDropEvent;
    QString mimeType;
    QString resourcePath;
    if(pEvent->mimeData()->hasText()){
        resourcePath = pEvent->mimeData()->text();
    }
    else if(pEvent->mimeData()->hasUrls()){
        resourcePath = pEvent->mimeData()->urls().at(0).toLocalFile();
    }
    else if(pEvent->mimeData()->hasImage()){
        pixFromDropEvent.loadFromData(pEvent->mimeData()->imageData().toByteArray());
        if(!pixFromDropEvent.isNull())
            mimeType = "image";
    }

    if (mimeType.isEmpty() && resourcePath.isEmpty()){
        pEvent->acceptProposedAction();
        return;
    }
    if(!resourcePath.isEmpty()){
        emit mediaDropped(resourcePath);
        pEvent->acceptProposedAction();
    }
}

void UBTBMediaContainer::dragEnterEvent(QDragEnterEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void UBTBMediaContainer::dragMoveEvent(QDragMoveEvent* pEvent)
{
    pEvent->acceptProposedAction();
}

void UBTBMediaContainer::dragLeaveEvent(QDragLeaveEvent* pEvent)
{
    pEvent->accept();
}

void UBTBMediaContainer::addMedia(const QString& mediaPath)
{
    if(!mediaPath.isEmpty())
        mMediaList.append(mediaPath);
    else
        qWarning() << __FUNCTION__ <<  "empty path";

    QString mimeType = UBFileSystemUtils::mimeTypeFromFileName(mediaPath);
    if(mimeType.contains("image")){
        QPixmap pix = QPixmap(mediaPath);
        QLabel* label = new QLabel();
        label->setPixmap(pix);
        label->setScaledContents(true);
        addWidget(label);
    }
    else if(mimeType.contains("video") || mimeType.contains("audio")){
        UBMediaPlayer* mediaPlayer = new UBMediaPlayer();
        mediaPlayer->setFile(mediaPath);
        addWidget(mediaPlayer);
    }
    else{
        qWarning() << "pMediaPath" << mediaPath;
        qWarning() << "bad idea to come here";
    }
}

QStringList UBTBMediaContainer::mediaUrls()
{
    return mMediaList;
}

void UBTBMediaContainer::cleanMedias()
{
    mMediaList.clear();
}

QWidget* UBTBMediaContainer::generateMediaWidget(const QString& url)
{
    QWidget* pW = NULL;

    if(!url.isEmpty())
        mMediaList.append(url);
    else
        qWarning() << __FUNCTION__ <<  "empty path";

    QString mimeType = UBFileSystemUtils::mimeTypeFromFileName(url);
    if(mimeType.contains("image")){
        QPixmap pix = QPixmap(url);
        QLabel* label = new QLabel();
        pix.scaledToWidth(label->width());
        label->resize(pix.width(), pix.height());
        label->setPixmap(pix);
        label->setScaledContents(true);
        pW = label;
    }
    else if(mimeType.contains("video") || mimeType.contains("audio")){
        UBMediaWidget* mediaPlayer = new UBMediaWidget(mimeType.contains("audio")?eMediaType_Audio:eMediaType_Video);
        mediaPlayer->setFile(url);
        pW = mediaPlayer;
    }
    else{
        qWarning() << "pMediaPath" << url;
        qWarning() << "bad idea to come here";
    }

    return pW;
}