#ifndef SHININESS_H
#define SHININESS_H

#include <QDialog>

namespace Ui {
class Shininess;
}

class Shininess : public QDialog
{
    Q_OBJECT
    
public:
    explicit Shininess(QWidget *parent = 0);
    ~Shininess();
    
private:
    Ui::Shininess *ui;
};

#endif // SHININESS_H
