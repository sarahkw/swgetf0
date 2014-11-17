#include "inputdevice.h"
#include "ui_inputdevice.h"

#include <QAudioInput> // not final
#include <QDebug> // not final
#include <QMessageBox>

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

bool validateAudioDevice(QString& errorMessage,
                         QAudioDeviceInfo& audioDeviceInfo)
{
#define CONDITION(cond) [](QAudioDeviceInfo& adi) { return cond; }

  static const struct {
    std::function<bool(QAudioDeviceInfo& adi)> test;
    const char* failureMessage;
  } tests[] = {

        {CONDITION(adi.supportedByteOrders().contains(
             static_cast<QAudioFormat::Endian>(QSysInfo::ByteOrder))),
         "Audio device does not support system byte order."},

        {CONDITION(adi.supportedChannelCounts().contains(1)),
         "Audio device does not support mono input."},

        {CONDITION(adi.supportedCodecs().contains("audio/pcm")),
         "Audio device does not support PCM codec."},

        {CONDITION(
             adi.supportedSampleTypes().contains(QAudioFormat::UnSignedInt)),
         "Audio device does not support unsigned int."},

	/*
        {CONDITION(false), "Failure test."},
        {CONDITION(false), "Failure test 2."},
	*/

    };

#undef CONDITION

  bool good = true;
  for (auto t : tests) {
    if (!t.test(audioDeviceInfo)) {
      if (!good) { errorMessage.append("\n"); }
      errorMessage.append(t.failureMessage);
      good = false;
    }
  }

  return good;
}

} // namespace anonymous

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

  QString errorMessage;
  if (!validateAudioDevice(errorMessage, device)) {
    // TODO: Put this on message queue, since it won't make much sense
    // if this is shown before the config popup.
    QMessageBox::critical(this, tr("Unsupported input device"), errorMessage);
  }
}
