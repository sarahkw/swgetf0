#include "inputdevice.h"
#include "ui_inputdevice.h"

#include <QAudioInput> // not final
#include <QAudioDeviceInfo> // not final
#include <QDebug> // not final

InputDevice::InputDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDevice)
{
  ui->setupUi(this);

  int itemPosition = 0;

  ui->cmbDevice->insertItem(
      itemPosition++,
      "Default: " + QAudioDeviceInfo::defaultInputDevice().deviceName());

  ui->cmbDevice->insertSeparator(itemPosition++);

  for (auto recordDevice :
       QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
    ui->cmbDevice->insertItem(itemPosition++, recordDevice.deviceName());
  }
}

InputDevice::~InputDevice()
{
    delete ui;
}
