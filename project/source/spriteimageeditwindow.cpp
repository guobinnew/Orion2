#include "spriteimageeditwindow.h"
#include "ui_spriteimageeditwindow.h"
#include "tooldefine.h"


SpriteImageEditWindow::SpriteImageEditWindow(SpriteSequenceResourceObject* seq, QWidget *parent) :
QDialog(parent), sequence_(seq),
    ui(new Ui::SpriteImageEditWindow)
{
    ui->setupUi(this);

	scene_ = new ImageScene();
	ui->editorView->setScene(scene_);

	// 更新列表
	foreach(SequenceFrame* frame, sequence_->frames_.values()){
		addFrame(frame);
	}

	anchorIcon_ = QIcon(UDQ_T(":/images/anchor.png"));
	// 更新锚点列表
	QListWidgetItem* item = NULL;
	foreach(QString anchor, sequence_->anchors_){
		item = new QListWidgetItem();
		item->setIcon(anchorIcon_);
		item->setText(anchor);
		if (anchor == ANCHOR_CENTER){
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		}
		else{
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
		}
		ui->anchorlistWidget->addItem(item);
	}

	Q_ASSERT(ui->anchorlistWidget->count() > 0);
	ui->anchorlistWidget->setCurrentItem(ui->anchorlistWidget->item(0));
}

SpriteImageEditWindow::~SpriteImageEditWindow()
{
    delete ui;
}

QListWidgetItem* SpriteImageEditWindow::addFrame(SequenceFrame* frame){
	QListWidgetItem* item = new QListWidgetItem( );
	item->setText(QString::number(ui->listWidget->count()));
	item->setIcon( QPixmap::fromImage( frame->buffer ) );
	ui->listWidget->addItem(item);
	return item;
}

void	SpriteImageEditWindow::resizeEvent(QResizeEvent * event){
	QSize newSize = event->size() - QSize(WIDGET_MARGIN * 2, WIDGET_MARGIN * 2);
	ui->layoutWidget->setGeometry(QRect(QPoint(WIDGET_MARGIN, WIDGET_MARGIN), newSize));
}

void SpriteImageEditWindow::on_listWidget_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous){
	int index = 0;
	SequenceFrame* frame = NULL;

	QListWidgetItem* anchoritem = ui->anchorlistWidget->currentItem();
	Q_ASSERT(anchoritem != NULL);

	// 不默认保存
	//if (previous){
	//	index = ui->listWidget->row(previous);
	//	frame = sequence_->frames_.at(index);
	//	scene_->save(frame, anchoritem->text());
	//}

	scene_->clearImage();

	if (current ){
		index = ui->listWidget->row(current);
		frame = sequence_->frames_.at(index);
		qDebug() << frame->state;

		// 设置原始图像
		scene_->setImage(sequence_->frameSourceImage(frame), frame->state);
		
		QPointF pos = sequence_->frameAnchor(index, anchoritem->text());
		scene_->setAnchor(pos);

		// 设置碰撞网格
		scene_->setCollision(sequence_->frameCollision(index));
	}

}

void SpriteImageEditWindow::on_anchorlistWidget_itemSelectionChanged(){

	QListWidgetItem* frameItem = ui->listWidget->currentItem();
	if (frameItem == NULL)
		return;

	QListWidgetItem* item = ui->anchorlistWidget->currentItem();
	if (item){
		int index = ui->listWidget->row(frameItem);
		SequenceFrame* frame = sequence_->frames_.at(index);

		QPointF pos = sequence_->frameAnchor(index, item->text());
		scene_->setAnchor(pos);
	}
	
}

void SpriteImageEditWindow::on_anchorlistWidget_itemChanged(QListWidgetItem * item){
	if (item == NULL)
		return;

	// 修改anchor名字
	int index = ui->anchorlistWidget->row(item);
	QString oldName = sequence_->anchors_.at(index);
	sequence_->anchors_.replace(index, item->text());
	ResourceHash oldkey = hashString(oldName);
	ResourceHash newkey = hashString(item->text());

	// 替换所有帧中的锚点
	foreach(SequenceFrame* frame, sequence_->frames_.values()){
		QPointF pos = frame->anchors.take(oldkey);
		frame->anchors.insert(newkey, pos);
	}
}

void SpriteImageEditWindow::on_addpushButton_clicked(){
	// 创建唯一名
	static QString AnchorBaseName = UDQ_T("Anchor");

	int index = 1;
	bool found = true;
	QString name = AnchorBaseName + QString::number(index++);
	while (ui->anchorlistWidget->findItems(name, Qt::MatchExactly).size() > 0){
		name = AnchorBaseName + QString::number(index++);
	}

	QListWidgetItem* item = new QListWidgetItem();
	item->setIcon(anchorIcon_);
	item->setText(name);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	ui->anchorlistWidget->addItem(item);

	sequence_->anchors_.append(name);
}

void SpriteImageEditWindow::on_delpushButton_clicked(){
	QListWidgetItem* item = ui->anchorlistWidget->currentItem();
	if (item == NULL){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("请选择一个锚点"));
		return;
	}

	if (item->text() == ANCHOR_CENTER){
		QMessageBox::warning(this, windowTitle(), UDQ_TR("缺省Origin锚点不能删除"));
		return;
	}

}

void SpriteImageEditWindow::on_savepushButton_clicked(){
	// 保存当前的锚点和碰撞网格
	QListWidgetItem* item = ui->listWidget->currentItem();
	if (item == NULL)
		return;

	int index = ui->listWidget->row(item);
	SequenceFrame* frame = sequence_->frames_.at(index);

	 item = ui->anchorlistWidget->currentItem();
	Q_ASSERT(item != NULL);

	scene_->save(frame, item->text());
	qDebug() << frame->state;

	// 更新树列表图像
	ui->listWidget->item(index)->setIcon(QPixmap::fromImage(frame->buffer));

}

void SpriteImageEditWindow::on_pointerpushButton_clicked(){
	scene_->setMode(ImageScene::POM_POINTER);
	// 隐藏锚点和碰撞区
	scene_->setAnchorVisible(false);
	scene_->setCollisionVisible(false);
}

void SpriteImageEditWindow::on_anchorpushButton_clicked(){
	scene_->setMode(ImageScene::POM_ANCHOR);
	scene_->setAnchorVisible(true);
	scene_->setCollisionVisible(false);
}

void SpriteImageEditWindow::on_collisionpushButton_clicked(){
	scene_->setMode(ImageScene::POM_COLLISION);
	scene_->setAnchorVisible(false);
	scene_->setCollisionVisible(true);
}

void SpriteImageEditWindow::on_applypushButton_clicked(){

	if (QMessageBox::question(this, windowTitle(), UDQ_TR("确认应用到所有帧?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No){
		return;
	}

	// 保存
	// 保存当前的锚点和碰撞网格
	QListWidgetItem* item = ui->listWidget->currentItem();
	if (item == NULL)
		return;

	int index = ui->listWidget->row(item);
	SequenceFrame* frame = sequence_->frames_.at(index);
	item = ui->anchorlistWidget->currentItem();
	Q_ASSERT(item != NULL);
	scene_->save(frame, item->text());
	// 更新树列表图像
	ui->listWidget->item(index)->setIcon(QPixmap::fromImage(frame->buffer));

	if (scene_->getMode() == ImageScene::POM_POINTER){
		// 将旋转模式应用到其他帧
		foreach(SequenceFrame* f, sequence_->frames_.values()){
			if (f != frame){
				f->state = frame->state;
				// 更新缓存
				sequence_->refreshImage(f);
			}
		}

		// 更新所有树图像
		for (int i = 0; i < ui->listWidget->count(); i++){
			if ( i == index)
				continue;

			// 更新树列表图像
			ui->listWidget->item(i)->setIcon(QPixmap::fromImage(sequence_->frames_.at(i)->buffer));
		}
	}

	// 如果是锚点
	// 保存当前锚点，并应用给其他帧
	if (scene_->getMode() == ImageScene::POM_ANCHOR ){
		ResourceHash key = hashString(item->text());
		QPointF pos = frame->anchors.value(key);
		foreach(SequenceFrame* f, sequence_->frames_.values()){
			if (f != frame){
				f->anchors[key] = pos;
			}
		}
	}

	// 如果是碰撞
	// 保存碰撞顶点，应用给其他帧
	if (scene_->getMode() == ImageScene::POM_COLLISION ){
		QList<QPointF> pts = frame->collisionRegion;
		foreach(SequenceFrame* f, sequence_->frames_.values()){
			if (f != frame){
				f->collisionRegion = pts;
			}
		}
	}
}

void SpriteImageEditWindow::on_flippushButton_clicked(){
	QListWidgetItem* current = ui->listWidget->currentItem();
	if (current == NULL)
		return;

	QBitArray state = scene_->getStructure()->imageState();
	state.toggleBit(state[6] ? 5 : 4);
	scene_->getStructure()->updateState(state);

}

void SpriteImageEditWindow::on_revertpushButton_clicked(){
	QListWidgetItem* current = ui->listWidget->currentItem();
	if (current == NULL)
		return;

	QBitArray state = scene_->getStructure()->imageState();
	state.toggleBit(state[6] ? 4 : 5);
	scene_->getStructure()->updateState(state);

}

void SpriteImageEditWindow::on_ccwpushButton_clicked(){
	QListWidgetItem* current = ui->listWidget->currentItem();
	if (current == NULL)
		return;

	QBitArray state = scene_->getStructure()->imageState();
	if (!state[0]){
		state.toggleBit(5);
	}
	else{
		state.toggleBit(4);
	}
	state.toggleBit(0);
	state.toggleBit(6);

	scene_->getStructure()->updateState(state);
}

void SpriteImageEditWindow::on_cwpushButton_clicked(){
	QListWidgetItem* current = ui->listWidget->currentItem();
	if (current == NULL)
		return;

	QBitArray state = scene_->getStructure()->imageState();
	// 顺势针旋转，等于先水平翻转，然后对角翻转
	if (!state[0]){
		state.toggleBit(4);
	}
	else{
		state.toggleBit(5);
	}
	state.toggleBit(0);
	state.toggleBit(6);

	scene_->getStructure()->updateState(state);
}