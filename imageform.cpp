#include "imageform.h"
#include "ui_imageform.h"

imageForm::imageForm(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::imageForm)
{
    ui->setupUi(this);
}

void imageForm::setImage(const QPixmap& map)
{
    image = map;
    this->resize(image.width(), image.height());
}

void imageForm::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    if (image.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QSize pixSize = image.size();
    pixSize.scale(this->width(), this->height(), Qt::IgnoreAspectRatio);

    QPixmap scaledPix = image.scaled(pixSize,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation);

    painter.drawPixmap(QPoint(), scaledPix);
}

imageForm::~imageForm()
{
    delete ui;
}
