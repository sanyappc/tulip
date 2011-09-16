#include <tulip/PluginManager.h>
#include <tulip/PluginLister.h>

#include <QtXml/QDomDocument>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <tulip/DownloadManager.h>
#include <tulip/TulipSettings.h>
#include <tulip/QuaZIPFacade.h>
#include <tulip/SimplePluginProgress.h>
#include <tulip/TlpQtTools.h>

using namespace std;
using namespace tlp;

QMap<QString, LocationPlugins> PluginManager::_remoteLocations;
QMap<QNetworkReply*, QString> PluginManager::replyLocations;
PluginManager* PluginManager::_instance = NULL;

PluginManager::PluginManager() {
}

PluginManager* PluginManager::getInstance() {
  if(_instance == NULL) {
    _instance = new PluginManager();
  }

  return _instance;
}

QList<tlp::PluginInformations*> PluginManager::pluginsList(Location list) {
  QMap<QString, tlp::PluginInformations*> result;

  if(list.testFlag(Local)) {
    for(std::map<std::string, PluginListerInterface*>::const_iterator it = PluginListerInterface::allFactories->begin(); it != PluginListerInterface::allFactories->end(); ++it) {
      PluginListerInterface* currentLister = it->second;
      Iterator<string>* plugins = currentLister->availablePlugins();

      while(plugins->hasNext()) {
        string pluginName = plugins->next();
        const AbstractPluginInfo& info = currentLister->pluginInformations(pluginName);
        PluginInformations* localinfo = new PluginInformations(info, currentLister->getPluginsClassName(), currentLister->getPluginLibrary(pluginName));
        result[pluginName.c_str()] = localinfo;
      }

      delete plugins;
    }
  }

  if(list.testFlag(Remote)) {
    for(QMap<QString, LocationPlugins>::const_iterator it = _remoteLocations.begin(); it != _remoteLocations.end(); ++it) {
      for(LocationPlugins::const_iterator locationIt = it.value().begin(); locationIt != it.value().end(); ++locationIt) {
        QMap<QString, tlp::PluginInformations*>::const_iterator current = result.find(locationIt.key());

        if(current == result.end()) {
          DistantPluginInfo* pluginInfo = locationIt.value();
          PluginInformations* pluginInformations = new PluginInformations(*pluginInfo, pluginInfo->getType(), pluginInfo->getLocation(), pluginInfo->getRemotePluginName());
          result[locationIt.key()] = pluginInformations;
        }
        else {
          current.value()->AddPluginInformations(locationIt.value());
        }
      }
    }
  }

  return result.values();
}

LocationPlugins PluginManager::parseDescription(const QString& xmlDescription, const QString& location) {
  QDomDocument description;
  description.setContent(xmlDescription);
  QDomElement elm = description.documentElement();

  LocationPlugins remotePlugins;
  QDomNodeList pluginNodes = description.elementsByTagName("plugin");

  for(int i = 0; i < pluginNodes.count(); ++i) {
    const QDomNode& child = pluginNodes.at(i);
    QDomElement childElement = child.toElement();
    const QString name = childElement.attribute("name");
    const QString type = childElement.attribute("type");
    const std::string author = childElement.attribute("author").toStdString();
    const std::string date = childElement.attribute("date").toStdString();
    const std::string info = childElement.attribute("info").toStdString();
    const std::string fileName = childElement.attribute("fileName").toStdString();
    const std::string release = childElement.attribute("release").toStdString();
    const std::string group = childElement.attribute("group").toStdString();
    const std::string tulipRelease = childElement.attribute("tulipRelease").toStdString();
    const QString folder = childElement.attribute("folder");

    std::list<tlp::Dependency> dependencies;

    for(QDomNode n = child.firstChild(); !n.isNull(); n = n.nextSibling()) {
      QDomElement dependencyElement = n.toElement();
      tlp::Dependency dep(dependencyElement.attribute("name").toStdString(), dependencyElement.attribute("type").toStdString(), dependencyElement.attribute("version").toStdString());
      dependencies.push_back(dep);
    }

    tlp::DistantPluginInfo* pluginInfo = new DistantPluginInfo(author, date, group, name.toStdString(), info, release, tulipRelease, dependencies, type, location, folder);

    remotePlugins[pluginInfo->getName().c_str()] = pluginInfo;
  }

  return remotePlugins;
}

void PluginManager::addRemoteLocation(const QString& location) {
  QNetworkAccessManager* manager = DownloadManager::getInstance();
  QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(location + "/serverDescription.xml")));
  replyLocations[reply] = location;
  connect(manager, SIGNAL(finished(QNetworkReply*)), getInstance(), SLOT(serverDescriptionDownloaded(QNetworkReply*)));
}

void PluginManager::serverDescriptionDownloaded(QNetworkReply* reply) {
  //only perform these actions if the reply concerns a pluginserver description download
  if(replyLocations.contains(reply)) {
    const QString location = replyLocations[reply];

    if(reply->error() != QNetworkReply::NoError)
      emit errorAddRemoteLocation(reply->error(),reply->errorString());

    else {
      QString content(reply->readAll());
      QString xmlDocument(content);
      QDomDocument description;
      description.setContent(xmlDocument);
      QDomElement elm = description.documentElement();
      _remoteLocations[location] = parseDescription(xmlDocument, location);

      replyLocations.remove(reply);
      reply->deleteLater();
      emit(remoteLocationAdded(location));
    }
  }
}

void PluginManager::removeRemoteLocation(const QString& location) {
  _remoteLocations.remove(location);
}

void PluginManager::removePlugins() {
  foreach(const QString& plugin, TulipSettings::instance().pluginsToRemove()) {
    if(QFile::remove(plugin)) {
      TulipSettings::instance().unmarkPluginForRemoval(plugin);
    }
    else {
      //TODO proper error reporting
      std::cout << plugin.toStdString() << " could not be removed" << std::endl;
    }
  }
}

void PluginManager::unpackPlugins(const QString& inputFolder) {
  QDir input(inputFolder);
  PluginProgress* progress = new SimplePluginProgress();
  QStringList filters;
  filters << "*.zip";
  foreach(const QFileInfo& pluginArchive, input.entryInfoList(filters)) {
    QuaZIPFacade::unzip(tlp::getPluginLocalInstallationDir(), pluginArchive.absoluteFilePath(), progress);

    if(!progress->getError().empty()) {
      //TODO proper error reporting
      std::cout << progress->getError() << std::endl;
    }
    else {
      QFile::remove(pluginArchive.absoluteFilePath());
    }
  }
  delete progress;
}
