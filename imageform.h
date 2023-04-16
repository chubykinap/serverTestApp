#ifndef IMAGEFORM_H
#define IMAGEFORM_H

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
    void setImage(QPixmap map);

private:
    Ui::imageForm* ui;
};

#endif // IMAGEFORM_H
