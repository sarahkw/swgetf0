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

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>

#include <portaudiocpp/AutoSystem.hxx>
#include <portaudiocpp/BlockingStream.hxx>

#include <iostream>

#include "config.h"
#include "configurationdialog.h"
#include "configurationgeneric.h"
#include "f0thread.h"
#include "mainwindow.h"

int startGui(QApplication& app, const QString* configFileName)
{
  portaudio::AutoSystem autoSys;

  F0Thread f0;

  ConfigurationGeneric configGeneric(f0.f0());

  if (configFileName == NULL) {
      // GUI
      ConfigurationDialog* configDialog =
          new ConfigurationDialog(configGeneric);
      int result = configDialog->exec();
      if (result == QDialog::Rejected) {
          delete configDialog;
          return 0;
      }
      delete configDialog;
  } else {
      // Text
      QByteArray configFileData;
      {
          QFile configFile(*configFileName);
          if (!configFile.open(QIODevice::ReadOnly)) {
              // TODO More information on failure
              qCritical("Cannot open file");
              return 1;
          }
          configFileData = configFile.readAll();

          // Going to pass configFileData as a C-string. Probably not
          // ideal, but it's the easiest.
          configFileData.append(static_cast<char>(0));
      }

      portaudio::Device& defaultDevice =
          portaudio::System::instance().defaultInputDevice();

      try {
          configGeneric.configure(configFileData.data(), defaultDevice);
      } catch (const schemeinterface::SchemeException& e) {
          qCritical() << "Error loading configuration: "
                      << e.error();
          return 1;
      } catch (const ConfigurationGeneric::AudioConfigurationNotSupported& e) {
          qCritical() << "Audio configuration not supported.";
          return 1;
      } catch (const GetF0::ParameterError& e) {
          qCritical() << "Invalid ESPS config: " << e.what();
          return 1;
      }
  }

  const config::Config& config = configGeneric.config();

  portaudio::BlockingStream* blockingStream =
      new portaudio::BlockingStream(configGeneric.streamParameters());

  f0.f0().setStream(blockingStream);
  f0.f0().init(config.audioConfig.sample_rate);

  MainWindow mainWindow(config, f0);
  mainWindow.show();

  f0.start();

  app.exec();

  /*TODO

    We recommend that you connect clean-up code to the aboutToQuit()
    signal, instead of putting it in your application's main()
    function because on some platforms the QCoreApplication::exec()
    call may not return. For example, on Windows when the user logs
    off, the system terminates the process after Qt closes all
    top-level windows. Hence, there is no guarantee that the
    application will have time to exit its event loop and execute code
    at the end of the main() function after the
    QCoreApplication::exec() call.
   */

  f0.f0().stop();
  f0.wait();

  return 0;
}

struct CommandLineArguments {
    QString configFileName;
};

enum class CommandLineParseResult {
    GuiConfig,
    FileConfig,
    DumpDefaultConfig,
    HelpRequested,
    Error
};

CommandLineParseResult parseCommandLine(QCommandLineParser& parser,
                                        CommandLineArguments* args,
                                        QString* errorMessage)
{
    const QCommandLineOption dumpDefaultConfigOption(
        "dump-default-config", "Writes default configuration to stdout");
    parser.addOption(dumpDefaultConfigOption);
    const QCommandLineOption configFileOption(
        "config-file",
        "Use provided configuration file. Uses default audio devices.", "file");
    parser.addOption(configFileOption);
    const QCommandLineOption helpOption = parser.addHelpOption();

    if (!parser.parse(QCoreApplication::arguments())) {
        *errorMessage = parser.errorText();
        return CommandLineParseResult::Error;
    }

    if (parser.isSet(dumpDefaultConfigOption)) {
        return CommandLineParseResult::DumpDefaultConfig;
    }

    if (parser.isSet(configFileOption)) {
        args->configFileName = parser.value(configFileOption);
        return CommandLineParseResult::FileConfig;
    }

    if (parser.isSet(helpOption)) {
        return CommandLineParseResult::HelpRequested;
    }

    return CommandLineParseResult::GuiConfig;
}

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QCommandLineParser parser;
  parser.setApplicationDescription("swgetf0");

  CommandLineArguments parsedArgs;
  QString errorMessage;

  switch (parseCommandLine(parser, &parsedArgs, &errorMessage)) {
  case CommandLineParseResult::GuiConfig:
      return startGui(app, NULL);
  case CommandLineParseResult::FileConfig:
      return startGui(app, &parsedArgs.configFileName);
  case CommandLineParseResult::DumpDefaultConfig:
      QTextStream(stdout) << ConfigurationGeneric::defaultConfiguration()
                          << "\n";
      return 0;
  case CommandLineParseResult::HelpRequested:
      parser.showHelp();
      return 0;
  case CommandLineParseResult::Error:
      QTextStream(stderr) << errorMessage << "\n";
      return 1;
  }

  Q_UNREACHABLE();
}
