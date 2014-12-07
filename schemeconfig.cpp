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

#include "schemeconfig.h"

#include "tinyscheme/scheme-private.h"
#include "tinyscheme/scheme.h"

using namespace schemeconfig;

GetDataFromResource::GetDataFromResource(const char* file)
{
  QResource resource(file);
  Q_ASSERT(resource.isValid());
  if (resource.isCompressed())
    m_byteArray = qUncompress(resource.data(), resource.size());
  else
    m_byteArray = QByteArray(reinterpret_cast<const char*>(resource.data()),
                             resource.size());
}

const QByteArray& GetDataFromResource::byteArray() const { return m_byteArray; }

