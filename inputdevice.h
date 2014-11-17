#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QDialog>
#include <QMap>
#include <QAudioDeviceInfo>

namespace Ui {
class InputDevice;
}

class InputDevice : public QDialog
{
  Q_OBJECT

public:
  explicit InputDevice(QWidget *parent = 0);
  ~InputDevice();

public slots:

  void on_cmbDevice_currentIndexChanged(int index);

private:
  Ui::InputDevice *ui;

  QMap<int, QAudioDeviceInfo> m_indexToDevice;
};

#endif // INPUTDEVICE_H
