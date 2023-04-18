#ifndef IMAGEFORM_H
#define IMAGEFORM_H

#include <QPainter>
#include <QWidget>

namespace Ui {
class imageForm;
}

class imageForm : public QWidget {
    Q_OBJECT

public:
    explicit imageForm(QWidget* parent = nullptr);
    ~imageForm();
public slots:
    void setImage(const QPixmap& map);

private slots:
    void paintEvent(QPaintEvent* event);

private:
    Ui::imageForm* ui;
    QPixmap image;
};

#endif // IMAGEFORM_H
