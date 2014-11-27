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

#include "inputdevice.h"
#include "ui_inputdevice.h"

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

} // namespace anonymous

InputDevice::InputDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDevice)
{
  ui->setupUi(this);
}

InputDevice::~InputDevice()
{
    delete ui;
}


void InputDevice::on_cmbDevice_currentIndexChanged(int index)
{
}
