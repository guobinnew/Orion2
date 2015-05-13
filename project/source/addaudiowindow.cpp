#include "addaudiowindow.h"
#include "ui_addaudiowindow.h"
#include <QMessageBox>
#include "tooldefine.h"

AddAudioWindow::AddAudioWindow(const QString& filePath, const QStringList& categories, QWidget *parent) :
QDialog(parent),
    ui(new Ui::AddAudioWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	ui->catecomboBox->addItems(categories);

	// 加载包列表
	for (TResourceObjectIterator<PackageResourceObject> it; it; ++it){
		PackageResourceObject* pkg = (PackageResourceObject*)(*it);
		ui->pkgcomboBox->addItem(pkg->objectName(), pkg->hashKey());
	}

	QFile audioFile(filePath);
	if (audioFile.open(QIODevice::ReadOnly)){
		data_ = audioFile.readAll();
		audioFile.close();
	}

	// 加载音频文件
	player_ = new QMediaPlayer;
	connect(player_, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError(QMediaPlayer::Error)));
	player_->setMedia(QUrl::fromLocalFile(filePath));

	QFileInfo fileInfo(filePath);
	ui->namelineEdit->setText(fileInfo.baseName());
	format_ = fileInfo.suffix();

	// 获取音频属性
	qreal size = data_.size() / 1024.0;
	ui->infolabel->setText(QString(UDQ_TR("数据大小: %1KB ")).arg(QLocale().toString(size, 'f', 2)));
	
}

AddAudioWindow::~AddAudioWindow()
{
	delete player_;
    delete ui;
}


void  AddAudioWindow::on_cancelpushButton_clicked(){
	reject();
}

bool AddAudioWindow::canSave(void){
	if (ui->pkgcomboBox->currentIndex() < 0){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请指定一个资源包!"));
		return false;
	}

	// 检查是否合法
	LINEEDIT_VALID(ui->namelineEdit, windowTitle(), UDQ_TR("资源ID"), REG_GALLERYID);

	// 检查是否存在
	AudioResourceObject* audio = TFindResource<AudioResourceObject>(
		ui->pkgcomboBox->currentText(),
		ui->namelineEdit->text(), false);

	if (audio != NULL){
		//音频已经存在，提示是否覆盖
		if (QMessageBox::question(this, windowTitle(), UDQ_TR("同名的音频已经存在，是否覆盖?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
			return false;
		}
	}

	return true;
}

void  AddAudioWindow::on_savepushButton_clicked(){
	if (!canSave())
		return;

	// 保存到数据库中
	newAudio_ = TFindResource<AudioResourceObject>(
		ui->pkgcomboBox->currentText(),
		ui->namelineEdit->text(), true);
	Q_ASSERT(newAudio_ != 0);

	if (ui->catecomboBox->currentIndex() >= 0){
		newAudio_->categories_.append(ui->catecomboBox->currentText());
	}

	newAudio_->tags_ = ui->taglineEdit->text().split(UDQ_T(";"));
	newAudio_->setContent( data_ );
	newAudio_->format_ = format_;

	newAudio_->clearFlags(URF_TagGarbage);
	newAudio_->setFlags(URF_TagSave);

	// 保存到本地
	SAVE_PACKAGE_RESOURCE(AudioResourceObject, newAudio_);

	accept();

}


void AddAudioWindow::on_playpushButton_clicked(){
	Q_ASSERT(player_ != NULL);

	player_->setVolume(ui->volumehorizontalSlider->value());
	player_->play();

	ui->playpushButton->setEnabled(false);
	ui->pausepushButton->setEnabled(true);
	ui->stoppushButton->setEnabled(true);
}


void AddAudioWindow::on_pausepushButton_clicked(){
	Q_ASSERT(player_);

	player_->pause();

	ui->playpushButton->setEnabled(true);
	ui->pausepushButton->setEnabled(false);
	ui->stoppushButton->setEnabled(true);
}
void AddAudioWindow::on_stoppushButton_clicked(){
	Q_ASSERT(player_);

	player_->stop();

	ui->playpushButton->setEnabled(true);
	ui->pausepushButton->setEnabled(false);
	ui->stoppushButton->setEnabled(false);
}

void AddAudioWindow::on_volumehorizontalSlider_valueChanged(int value){
	ui->volumelabel->setText(QString(UDQ_T("%1")).arg(value));

	if (player_){
		player_->setVolume(value);
	}
}

void AddAudioWindow::handleError(QMediaPlayer::Error error){

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
		ui->loglabel->setText(UDQ_TR("未知错误!"));
	}
		break;
	}

	if (fault){
		ui->playpushButton->setEnabled(false);
		ui->pausepushButton->setEnabled(false);
		ui->stoppushButton->setEnabled(false);
	}
}