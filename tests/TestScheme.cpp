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

#include "../schemeinterface.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(Scheme, HelloWorld)
{
  schemeinterface::SchemeInterface si;
  auto ptr = si.read_eval("\"hello world!\"");
  ASSERT_TRUE(ptr.is_string());
  EXPECT_EQ(std::string(ptr.string_value()),
            std::string("hello world!"));
}

TEST(Scheme, ParseError)
{
  schemeinterface::SchemeInterface si;
  ASSERT_THROW(si.read_eval(")"), // syntax error
               schemeinterface::SchemeException);

  try {
    si.read_eval(")");
  } catch (const schemeinterface::SchemeException& e) {
    EXPECT_EQ(e.error(), QLatin1String("(\"syntax error: illegal token\")"));
  }
}

TEST(Scheme, ErrorWithoutReporting)
{
  // *error-hook* is set up to call si/report-error so that
  // *schemeinterface knows about the error. If we bypass *error-hook*
  // *and call error directly, we should not segfault.

  schemeinterface::SchemeInterface si;

  const char CODE[] = "(error 123)";

  ASSERT_THROW(si.read_eval(CODE),
               schemeinterface::SchemeException);

  try {
    si.read_eval(CODE);
  } catch (const schemeinterface::SchemeException& e) {
    EXPECT_EQ(e.error(), QLatin1String(""));
  }
}

TEST(Scheme, MultipleErrorComponents)
{
  schemeinterface::SchemeInterface si;

  // undefined variable
  const char CODE[] = "asdf";

  ASSERT_THROW(si.read_eval(CODE),
               schemeinterface::SchemeException);

  try {
    si.read_eval(CODE);
  } catch (const schemeinterface::SchemeException& e) {
    EXPECT_EQ(e.error(), QLatin1String("(\"eval: unbound variable:\" asdf)"));
  }
}
