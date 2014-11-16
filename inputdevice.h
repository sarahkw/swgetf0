#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QDialog>

namespace Ui {
class InputDevice;
}

class InputDevice : public QDialog
{
    Q_OBJECT

public:
    explicit InputDevice(QWidget *parent = 0);
    ~InputDevice();

private:
    Ui::InputDevice *ui;
};

#endif // INPUTDEVICE_H
