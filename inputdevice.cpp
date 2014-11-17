#include "inputdevice.h"
#include "ui_inputdevice.h"

#include <QAudioInput> // not final
#include <QDebug> // not final

namespace {
template <class IndexMapType, class ItemType, class Callable>
static void insertWithDefault(QComboBox* comboBox, IndexMapType& map,
                              QList<ItemType> items, ItemType defaultItem,
                              Callable stringGenerator)
{
  int itemPosition = 0;

  comboBox->clear();
  map.clear();

  {
    map[itemPosition] = defaultItem;

    comboBox->insertItem(itemPosition++,
                         "Default: " + stringGenerator(defaultItem));
  }

  comboBox->insertSeparator(itemPosition++);

  for (auto item : items) {
    map[itemPosition] = item;

    comboBox->insertItem(itemPosition++, stringGenerator(item));
  }
}
}  // namespace anonymous

InputDevice::InputDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDevice)
{
  ui->setupUi(this);

  insertWithDefault(ui->cmbDevice, m_indexToDevice,
                    QAudioDeviceInfo::availableDevices(QAudio::AudioInput),
                    QAudioDeviceInfo::defaultInputDevice(),
                    [](QAudioDeviceInfo& audioDeviceInfo) {
    return audioDeviceInfo.deviceName();
  });

}

InputDevice::~InputDevice()
{
    delete ui;
}

void InputDevice::on_cmbDevice_currentIndexChanged(int index)
{
  if (index == -1) return; // If combo box gets cleared

  auto device = m_indexToDevice[index];
  auto preferredAudioFormat = device.preferredFormat();

  insertWithDefault(ui->cmbSampleRate, m_indexToSampleRate,
                    device.supportedSampleRates(),
                    preferredAudioFormat.sampleRate(), [](int sampleRate) {
		      return QString::number(sampleRate);
		    });

  insertWithDefault(ui->cmbSampleSize, m_indexToSampleSize,
                    device.supportedSampleSizes(),
                    preferredAudioFormat.sampleSize(), [](int sampleSize) {
		      return QString::number(sampleSize);
		    });
}
