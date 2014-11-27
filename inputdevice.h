/*
  Copyright 2014 Sarah Wong

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <QDialog>
#include <QMap>

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

  QMap<int, int> m_indexToSampleRate;
};

#endif // INPUTDEVICE_H
