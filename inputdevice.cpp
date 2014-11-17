#include "inputdevice.h"
#include "ui_inputdevice.h"

#include <QAudioInput> // not final
#include <QDebug> // not final

InputDevice::InputDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDevice)
{
  ui->setupUi(this);

  int itemPosition = 0;

  {
    auto recordDevice = QAudioDeviceInfo::defaultInputDevice();

    m_indexToDevice[itemPosition] = recordDevice;

    ui->cmbDevice->insertItem(
        itemPosition++,
        "Default: " + recordDevice.deviceName());
  }

  ui->cmbDevice->insertSeparator(itemPosition++);

  for (auto recordDevice :
       QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {

    m_indexToDevice[itemPosition] = recordDevice;

    ui->cmbDevice->insertItem(itemPosition++, recordDevice.deviceName());
  }
}

InputDevice::~InputDevice()
{
    delete ui;
}

void InputDevice::on_cmbDevice_currentIndexChanged(int index)
{
  if (index != -1)
    qDebug() << m_indexToDevice[index].deviceName();
}
