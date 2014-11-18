/*
  Copyright 2014 Sarah Wong
*/

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

  /*! Get the QAudioDeviceInfo selected by the user */
  QAudioDeviceInfo getAudioDeviceInfo() const;

  /*! Get the QAudioFormat with what's required for the getf0
      algorithm, mixed with some options configurable by the user. */
  QAudioFormat getAudioFormat() const;

public slots:

  void on_cmbDevice_currentIndexChanged(int index);

private:

  Ui::InputDevice *ui;

  QMap<int, QAudioDeviceInfo> m_indexToDevice;
  QMap<int, int> m_indexToSampleRate;
  QMap<int, int> m_indexToSampleSize;
};

#endif // INPUTDEVICE_H
