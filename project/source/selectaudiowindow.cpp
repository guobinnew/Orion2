#include "selectaudiowindow.h"
#include "ui_selectaudiowindow.h"

#include <QStandardPaths>
#include <QMessageBox>

#include "tooldefine.h"
#include "resourceobject.h"
using namespace ORION;


SelectAudioWindow::SelectAudioWindow(QWidget *parent) :
QDialog(parent), audioHash_(0), player_(NULL),
    ui(new Ui::SelectAudioWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	initFromDb();

}

SelectAudioWindow::~SelectAudioWindow()
{
	delete player_;
    delete ui;
}


void  SelectAudioWindow::on_cancelpushButton_clicked(){

	reject();
}

void  SelectAudioWindow::on_savepushButton_clicked(){
	if (audioHash_ == 0){

		QListWidgetItem* item = ui->listWidget->currentItem();
		if (item == NULL){
			QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个音频资源!"));
			return;
		}
		audioHash_ = item->data(ITEM_HASHKEY).toUInt();
	}

	accept();
}


void SelectAudioWindow::initFromDb(void){

	// 加载列表
	for (TResourceObjectIterator<AudioResourceObject> it; it; ++it){
		AudioResourceObject* audio = (AudioResourceObject*)(*it);

		if (audio->flags() & URF_TagGarbage)
			continue;

		QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
		item->setIcon(QIcon(UDQ_T(":/images/gallery_audio.png")));
		item->setText(audio->objectName());
		item->setData(ITEM_HASHKEY, audio->hashKey());

	}

}

void SelectAudioWindow::on_playpushButton_clicked(){
	Q_ASSERT(player_ != NULL);

	player_->setVolume(ui->volumehorizontalSlider->value());
	player_->play();

	ui->playpushButton->setEnabled(false);
	ui->pausepushButton->setEnabled(true);
	ui->stoppushButton->setEnabled(true);
}


void SelectAudioWindow::on_pausepushButton_clicked(){
	Q_ASSERT(player_);

	player_->pause();

	ui->playpushButton->setEnabled(true);
	ui->pausepushButton->setEnabled(false);
	ui->stoppushButton->setEnabled(true);
}
void SelectAudioWindow::on_stoppushButton_clicked(){
	Q_ASSERT(player_);

	player_->stop();

	ui->playpushButton->setEnabled(true);
	ui->pausepushButton->setEnabled(false);
	ui->stoppushButton->setEnabled(false);
}

void SelectAudioWindow::on_volumehorizontalSlider_valueChanged(int value){
	ui->volumelabel->setText(QString(UDQ_T("%1")).arg(value));

	if (player_){
		player_->setVolume(value);
	}
}

void SelectAudioWindow::handleError(QMediaPlayer::Error error){

	bool fault = true;
	switch (error)
	{
	case QMediaPlayer::NoError:
	{
		fault = false;
		ui->loglabel->setText(UDQ_TR("资源已找到!"));
	}
		break;
	case QMediaPlayer::ResourceError:
	{
		ui->loglabel->setText(UDQ_TR("url地址无法解析!"));
	}
		break;
	case QMediaPlayer::FormatError:
	{
		ui->loglabel->setText(UDQ_TR("播放文件格式错误，无法播放!"));
	}
		break;
	case QMediaPlayer::NetworkError:
	{
		ui->loglabel->setText(UDQ_TR("网络故障，无法打开资源!"));
	}
		break;
	case QMediaPlayer::AccessDeniedError:
	{
		ui->loglabel->setText(UDQ_TR("资源拒绝访问!"));
	}
		break;
	case QMediaPlayer::ServiceMissingError:
	{
		ui->loglabel->setText(UDQ_TR("没有找到合适设备，无法播放!"));
	}
		break;
	default:
	{
		ui->loglabel->setText(UDQ_T("未知错误!"));
	}
		break;
	}

	if (fault){
		ui->playpushButton->setEnabled(false);
		ui->pausepushButton->setEnabled(false);
		ui->stoppushButton->setEnabled(false);
	}
}

void  SelectAudioWindow::on_listWidget_itemDoubleClicked(QListWidgetItem * item){
	Q_ASSERT(item != NULL);

	if (player_ != NULL){
		player_->stop();
		delete player_;
		player_ = NULL;
	}


	audioHash_= item->data(ITEM_HASHKEY).toUInt();
	DEFINE_RESOURCE_HASHKEY_VALID(AudioResourceObject, audio, audioHash_);

	QString tempPath = QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0);
	// 将当前音频拷贝临时文件
	QString tempFIlePath = QString(UDQ_T("%1/audio_%2.mp3")).arg(tempPath).arg(audio->hashKey());
	
	// 确保临时目录存在
	QDir dir(tempPath);
	if (!dir.exists()){
		dir.mkpath(tempPath);
	}

	QFile file(tempFIlePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("指定的音频资源导出失败!"));
		return;
	}
	file.write(audio->content_);
	file.close();

	player_ = new QMediaPlayer;
	connect(player_, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError(QMediaPlayer::Error)));
	player_->setMedia(QUrl::fromLocalFile(tempFIlePath));

	// 启动文件播放
	on_playpushButton_clicked();
}