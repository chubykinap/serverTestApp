#include "imageform.h"
#include "ui_imageform.h"

imageForm::imageForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::imageForm)
{
    ui->setupUi(this);
}

void imageForm::setImage(QPixmap map)
{
    ui->label->setMinimumWidth(map.width());
    ui->label->setMinimumHeight(map.height());
    ui->label->setPixmap(map);
}

imageForm::~imageForm()
{
    delete ui;
}
