#include "importgallerywindow.h"
#include "ui_importgallerywindow.h"

#include <QUrl>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>

#include "tooldefine.h"
#include "imagehelper.h"
#include "resourceobject.h"
#include "resourcelinker.h"
using namespace ORION;

ImportGalleryWindow::ImportGalleryWindow(const QString& category, int filter, QWidget *parent) :
QDialog(parent), cate_(category),
    ui(new Ui::ImportGalleryWindow)
{
    ui->setupUi(this);
	setFixedSize(size());

	if (filter & FILTER_IMAGE){
		ui->imagecheckBox->setChecked(false);
		ui->imagecheckBox->setEnabled(false);
	}

	if (filter & FILTER_AUDIO){
		ui->audiocheckBox->setChecked(false);
		ui->audiocheckBox->setEnabled(false);
	}

	if (filter & FILTER_VIDEO){
		ui->videocheckBox->setChecked(false);
		ui->videocheckBox->setEnabled(false);
	}

	// 加载包列表
	for (TResourceObjectIterator<PackageResourceObject> it; it; ++it){
		PackageResourceObject* pkg = (PackageResourceObject*)(*it);
		ui->pkgcomboBox->addItem(pkg->objectName(), pkg->hashKey());
	}
}

ImportGalleryWindow::~ImportGalleryWindow()
{
    delete ui;
}

void ImportGalleryWindow::on_pathpushButton_clicked(){
	QString filePath = QFileDialog::getExistingDirectory(this, windowTitle(), QString());
	if (!filePath.isEmpty()){
		// 获取工程目录名
		ui->urllineEdit->setText(filePath);

		// 加载目录文件
		ui->filelistWidget->clear();

		// 遍历目录，加载图片
		QVector<QString> dirNames;

		QFileInfoList filst;
		QFileInfoList::iterator curFi;
		QDir dir;

		//初始化
		dirNames << filePath;

		//遍历各级文件夹，并将这些文件夹中的文件删除  
		for (int i = 0; i < dirNames.size(); ++i){
			dir.setPath(dirNames[i]);
			filst = dir.entryInfoList(QDir::Dirs | QDir::Files
				| QDir::Readable | QDir::Writable
				| QDir::Hidden | QDir::NoDotAndDotDot
				, QDir::Name);
			if (filst.size()>0){
				curFi = filst.begin();
				while (curFi != filst.end()){
					//遇到文件夹,则添加至文件夹列表dirs尾部  
					if (curFi->isDir()){
						dirNames.push_back(curFi->filePath());
					}
					else if (curFi->isFile()){
						int  type = 0;
						//遇到文件, 检查扩展名
						if (ImageResourceObject::isValidFile(curFi->suffix())){
							type  = FILTER_IMAGE;
						}
						else if (AudioResourceObject::isValidFile(curFi->suffix())){
							type = FILTER_AUDIO;
						}
						else if (VideoResourceObject::isValidFile(curFi->suffix())){
							type = FILTER_VIDEO;
						}

						if( type > 0){
							loadFiles_.append(curFi->absoluteFilePath());
							loadFile(curFi->absoluteFilePath(), type);
						}
					}
					curFi++;
				}
			}
		}
	}
}

void ImportGalleryWindow::loadFile(const QString& localFile, int type){

	QFileInfo info(localFile);

	QListWidgetItem* item = new QListWidgetItem();
	item->setText(info.baseName());
	item->setData(ITEM_DESC, (int)0);
	item->setData(ITEM_VALUE, localFile);
	item->setData(ITEM_CONTEXT, type);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	item->setCheckState(Qt::Checked);

	if (type == FILTER_IMAGE){
		// 加载文件缩略图
		QImage img(localFile);
		QImage thumb = thnumImage(128, 128, img);
		item->setIcon(QIcon(QPixmap::fromImage(thumb)));
		item->setHidden(!ui->imagecheckBox->isChecked());
	}
	else if (type == FILTER_AUDIO){
		item->setIcon(QIcon(UDQ_T(":/images/gallery_audio.png")));
		item->setHidden(!ui->audiocheckBox->isChecked());
	}
	else if (type == FILTER_VIDEO){
		item->setIcon(QIcon(UDQ_T(":/images/gallery_video.png")));
		item->setHidden(!ui->videocheckBox->isChecked());
	}

	ui->filelistWidget->addItem(item);
}


void ImportGalleryWindow::on_imagecheckBox_stateChanged(int state){
	bool visible = state == Qt::Checked;
	for (int i = 0; i < ui->filelistWidget->count(); i++){
		if (ui->filelistWidget->item(i)->data(ITEM_CONTEXT).toInt() == FILTER_IMAGE){
			ui->filelistWidget->item(i)->setHidden(!visible);
		}
	}
}

void ImportGalleryWindow::on_audiocheckBox_stateChanged(int state){
	bool visible = state == Qt::Checked;
	for (int i = 0; i < ui->filelistWidget->count(); i++){
		if (ui->filelistWidget->item(i)->data(ITEM_CONTEXT).toInt() == FILTER_AUDIO){
			ui->filelistWidget->item(i)->setHidden(!visible);
		}
	}
}

void ImportGalleryWindow::on_videocheckBox_stateChanged(int state){
	bool visible = state == Qt::Checked;
	for (int i = 0; i < ui->filelistWidget->count(); i++){
		if (ui->filelistWidget->item(i)->data(ITEM_CONTEXT).toInt() == FILTER_VIDEO){
			ui->filelistWidget->item(i)->setHidden(!visible);
		}
	}
}

void ImportGalleryWindow::on_cancelpushButton_clicked(){
	reject();
}

void ImportGalleryWindow::on_savepushButton_clicked(){

	QString pkg = ui->pkgcomboBox->currentText();
	if (pkg.isEmpty()){
		QMessageBox::critical(this, windowTitle(), UDQ_TR("请指定一个素材包!"));
		return;
	}

	bool overwrite = ui->overwritecheckBox->isChecked();

	int invalidItem = 0;
	// 向数据库中导入素材
	for (int i = 0; i < ui->filelistWidget->count(); i++){
		QListWidgetItem* item = ui->filelistWidget->item(i);
		if (!item->isHidden() && item->checkState() == Qt::Unchecked)
			continue;

		if (!vaildCheck(item, pkg, overwrite)){
			invalidItem++;
			item->setBackgroundColor(QColor(200, 75, 75));
			continue;
		}
		else{
			item->setBackgroundColor(QColor(75, 200, 75));
			item->setCheckState(Qt::Unchecked);
		}

		// 添加素材
		int fromMem = item->data(ITEM_DESC).toInt();
		if (fromMem == 0){
			QString localFile = item->data(ITEM_VALUE).toString();
			saveToDb(item, pkg, localFile);
		}
		else{
			QByteArray fileData = item->data(ITEM_VALUE).toByteArray();
			saveToDb(item, pkg, fileData, UDQ_T("png"));  //  目前支持png图片格式
		}
	}

	if (invalidItem > 0){
		QMessageBox::information(this, windowTitle(), UDQ_TR("部分素材ID不合法（标记红色），无法导入!"));
	}
	else{
		QMessageBox::information(this, windowTitle(), UDQ_TR("所选素材全部导入成功!"));
	}

}

void ImportGalleryWindow::on_selectallcheckBox_stateChanged(int state){
	for (int i = 0; i < ui->filelistWidget->count(); i++){
			ui->filelistWidget->item(i)->setCheckState((Qt::CheckState)state);
	}
}

// 有效性检查
bool ImportGalleryWindow::vaildCheck(QListWidgetItem* item, const QString& pkg, bool overwrite){

	QString name = item->text();
	// 检查名字是否合法
	QRegExp r(REG_GALLERYID);
	if (!r.exactMatch(name)){
		return false;
	}

	int type = item->data(ITEM_CONTEXT).toInt();

	if (type == FILTER_IMAGE){
		// 检查是否存在
		ImageResourceObject* image = TFindResource<ImageResourceObject>(pkg, name, false);
		if (image != NULL && !overwrite){
			// 图像已经存在，提示是否覆盖
			if (QMessageBox::question(this, windowTitle(), QString(UDQ_TR("图片[%1.%2]已经存在, 是否覆盖？")).arg(pkg).arg(name)) == QMessageBox::No)
				return false;
		}
	}
	else if (type == FILTER_AUDIO){
		// 检查是否存在
		AudioResourceObject* audio = TFindResource<AudioResourceObject>(pkg, name, false);
		if (audio != NULL && !overwrite){
			// 音频已经存在，提示是否覆盖
			if (QMessageBox::question(this, windowTitle(), QString(UDQ_TR(" 音频[%1.%2]已经存在, 是否覆盖？")).arg(pkg).arg(name)) == QMessageBox::No)
				return false;
		}
	}
	else if (type == FILTER_VIDEO){
		// 检查是否存在
		VideoResourceObject* video = TFindResource<VideoResourceObject>(pkg, name, false);
		if (video != NULL && !overwrite){
			// 图像已经存在，提示是否覆盖
			if (QMessageBox::question(this, windowTitle(), QString(UDQ_TR("视频[%1.%2]已经存在, 是否覆盖？")).arg(pkg).arg(name)) == QMessageBox::No)
				return false;
		}
	}

	return true;
}

void ImportGalleryWindow::saveToDb(QListWidgetItem* item, const QString& pkg, const QString& path){

	QByteArray data;
	QFile file(path);
	if (file.open(QIODevice::ReadOnly)){
		data = file.readAll();
		file.close();
	}
	saveToDb(item, pkg, data, QFileInfo(path).suffix());

}


void ImportGalleryWindow::saveToDb(QListWidgetItem* item, const QString& pkg, const QByteArray& data, const QString& originType){
	QString name = item->text();
	int type = item->data(ITEM_CONTEXT).toInt();

	if (type == FILTER_IMAGE){
		// 保存到数据库中
		ImageResourceObject* newImage = TFindResource<ImageResourceObject>(pkg, name, true);
		Q_ASSERT(newImage != 0);
			
		newImage->setContent( data );
		newImage->image_ = QImage::fromData(newImage->content_);
		newImage->format_ = originType;
		newImage->makeThumbnail();
		newImage->isGroup_ = false;
		newImage->frames_.clear();
		newImage->frames_.append(QRect(0, 0, newImage->image_.width(), newImage->image_.height()));

		// 设置当前选择的类目
		newImage->categories_.append(cate_);

		newImage->clearFlags(URF_TagGarbage);
		newImage->setFlags(URF_TagSave);

		// 保存到本地
		SAVE_PACKAGE_RESOURCE(ImageResourceObject, newImage);
	}
	else if (type == FILTER_AUDIO){
		// 保存到数据库中
		AudioResourceObject* newAudio = TFindResource<AudioResourceObject>(pkg, name, true);
		Q_ASSERT(newAudio != 0);
		
		newAudio->setContent( data );
		newAudio->format_ = originType;
		newAudio->makeThumbnail();

		// 设置当前选择的类目
		newAudio->categories_.append(cate_);


		newAudio->clearFlags(URF_TagGarbage);
		newAudio->setFlags(URF_TagSave);

		// 保存到本地
		SAVE_PACKAGE_RESOURCE(AudioResourceObject, newAudio);
	}
	else if (type == FILTER_VIDEO){
		VideoResourceObject* newVideo = TFindResource<VideoResourceObject>(pkg, name, true);
		Q_ASSERT(newVideo != 0);
	
		newVideo->setContent( data );
		newVideo->format_ = originType;
		newVideo->makeThumbnail();

		// 设置当前选择的类目
		newVideo->categories_.append(cate_);

		newVideo->clearFlags(URF_TagGarbage);
		newVideo->setFlags(URF_TagSave);

		// 保存到本地
		SAVE_PACKAGE_RESOURCE(VideoResourceObject, newVideo);
	}
}


void ImportGalleryWindow::on_filepushButton_clicked(){
	QString file = QFileDialog::getOpenFileName(this, windowTitle(), QString(), UDQ_TR("EaselJS(*.json);;cocos2d(*.plist)"));
	if (!file.isEmpty()){
		// 获取工程目录名
		ui->urllineEdit->setText(file);

		// 加载目录文件
		ui->filelistWidget->clear();

		QFileInfo info(file);
		if (info.suffix().toLower() == UDQ_T("json")){
			// 加载json文件，解析json文件，生成列表项
			QJsonParseError error;
			QByteArray jsonData;
			QFile inFile(file);
			if (inFile.open(QIODevice::ReadOnly | QIODevice::Text)){
				jsonData = inFile.readAll();
				inFile.close();
			}

			if (jsonData.size() == 0){
				QMessageBox::warning(this, windowTitle(), UDQ_TR("文件为空"));
				return;
			}

			// 获取文件所在目录
			QString fileDir = info.absolutePath();

			// TODO:
			// 目前仅考虑单文件，默认与json同名，后续扩展为支持多文件
			QString textureFile = fileDir + UDQ_T("/") + info.baseName() + UDQ_T(".png");

			// 加载png文件
			QImage img;
			img.load(textureFile);

			// 解析json
			QJsonDocument document = QJsonDocument::fromJson(jsonData, &error);
			Q_ASSERT(error.error == QJsonParseError::NoError && document.isObject());

			QJsonObject obj = document.object();
			QJsonArray arr = obj.value(UDQ_T("frames")).toArray();

			for (int i = 0; i < arr.size(); i++){
				QJsonArray frame = arr[i].toArray();
				QRect bound;
				bound.setLeft(frame[0].toInt());
				bound.setTop(frame[1].toInt());
				bound.setWidth(frame[2].toInt());
				bound.setHeight(frame[3].toInt());

				// 从原始图像提取部分图片
				QImage itemImg = img.copy(bound);
				QByteArray data;
				{
					QBuffer buffer(&data);
					buffer.open(QIODevice::WriteOnly);
					itemImg.save(&buffer, "PNG");
				}

				QListWidgetItem* item = new QListWidgetItem();
				item->setText(info.baseName() + QString(UDQ_T("frame%1")).arg(i));
				item->setData(ITEM_DESC, (int)1);  // 内存数据
				item->setData(ITEM_VALUE, data);
				item->setData(ITEM_CONTEXT, (int)FILTER_IMAGE);
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);

				// 加载文件缩略图
				QImage thumb = thnumImage(128, 128, itemImg);
				item->setIcon(QIcon(QPixmap::fromImage(thumb)));
				item->setHidden(!ui->imagecheckBox->isChecked());
				ui->filelistWidget->addItem(item);

			}

		}

	}
}