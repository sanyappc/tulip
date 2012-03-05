/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#ifndef TULIP_PLUGINLISTER_H
#define TULIP_PLUGINLISTER_H

#include <list>
#include <string>
#include <set>

#include <tulip/Plugin.h>
#include <tulip/PluginLoader.h>
#include <tulip/Iterator.h>
#include <tulip/StlIterator.h>
#include <tulip/TlpTools.h>
#include <tulip/PluginContext.h>
#include <tulip/PluginLibraryLoader.h>
#include <tulip/PluginContext.h>
#include <tulip/WithParameter.h>

namespace tlp {

/** @addtogroup plugins
 * @brief The Tulip plug-in system allows plug-ins to be loaded dynamically at run-time, and can check dependencies on other plug-ins, as well as multiple definitions.
 *
 * The Tulip plug-in system can be decomposed in 4 layers:
 *
 * 1: Tulip, who knows about PluginListers
 *
 * 2: PluginListers, who know all the plugins of a certain type (e.g. AlgorithmLister knows all 'Algorithm' plugins).
 *
 * 3: Factories, who only know about one plugin (those are generated by the Tulip macros, you never have to write it)
 *
 * 4: The plugin itself, a subclass of Algorithm (more likely IntegerAlgorithm, DoubleAlgorithm, ...), View, ControllerViewsManager, or any other type Tulip knows about.
 * If Tulip does not know about the plugin type, it will sucessfully load, but will not be shown in the interface, and Tulip will in no way be able to use it.
 */

/*@{*/

class FactoryInterface;

/**
 * @brief This template class takes 3 parameters :
 *
 * * The type of factory that it will list,
 *
 * * The type of object said factories build,
 *
 * * The type of object to pass as parameter to the objects when building them.
 *
 * When constructed it registers itself into the factories map automatically.
 **/
class PluginLister {
private:
  struct PluginDescription {
    FactoryInterface* factory;
    ParameterList parameters;
    std::list<tlp::Dependency> dependencies;
    std::string library;
  };

public:
  static PluginLoader *currentLoader;

  /**
   * @brief Checks if all registered plug-ins have their dependencies met.
   *
   * @param loader If there are errors, the loader is informed about them so they can be displayed.
   * @return void
   **/
  static void checkLoadedPluginsDependencies(tlp::PluginLoader* loader);

  /**
   * @brief Gets the static instance of this class. If not already done, creates it beforehand.
   *
   * @return PluginLister< ObjectType, Context >* The only instance of this object that exists in the whole program.
   **/
  static tlp::PluginLister* instance() {
    if(_instance == NULL) {
      _instance = new PluginLister();
    }

    return _instance;
  }

  /**
   * @brief Constructs a plug-in.
   *
   * @param name The name of the plug-in to instantiate.
   * @param p The context to give to the plug-in.
   * @return ObjectType* The newly constructed plug-in.
   **/
  static tlp::Plugin* getPluginObject(const std::string& name, tlp::PluginContext* context);

  template<typename PluginType>
  PluginType* getPluginObject(const std::string& name, tlp::PluginContext* context) {
    return dynamic_cast<PluginType*>(getPluginObject(name, context));
  }
  

  /**
   * @brief Gets the list of plug-ins that registered themselves in this factory.
   *
   * @return :Iterator< std::string >* An iterator over the names of the plug-ins registered in this factory.
   **/
  static Iterator<std::string>* availablePlugins();

  /**
   * @brief Gets more detailed informations about one specific plug-in.
   *
   * @param name The name of the plugin to retrieve informations for.
   * @return :Plugin* The informations on the plugin.
   **/
  static const Plugin* pluginInformations(const std::string& name);

  /**
   * @brief Checks if a given name is registered in this factory.
   *
   * @param pluginName The name of the plug-in to look for.
   * @return bool Whether there is a plug-in with the given name registered in this factory.
   **/
  static bool pluginExists(const std::string& pluginName);

  /**
   * @brief Gets the list of parameters for the given plug-in.
   *
   * @param name The name of the plug-in to retrieve the parameters of.
   * @return :ParameterList The parameters of the plug-in.
   **/  
  static const ParameterList getPluginParameters(std::string name);

  /**
   * @brief Gets the dependencies of a plug-in.
   *
   * @param name The name of the plug-in to retrieve the dependencies of.
   * @return :list< tlp::Dependency, std::allocator< tlp::Dependency > > The list of dependencies of the plug-in.
   **/  
  static std::list<tlp::Dependency> getPluginDependencies(std::string name);

  /**
   * @brief Gets the library from which a plug-in has been loaded.
   *
   * @param name The name of the plug-in to retrieve the library of.
   * @return std::string The library from which the plug-in was loaded.
   **/
  static std::string getPluginLibrary(const std::string& name);

  /**
   * @brief Removes a plug-in from this factory.
   * This is usefull when a plug-in has unmet dependencies, or appears more than once.
   *
   * @param name The name of the plug-in to remove.
   * @return void
   **/
  static void removePlugin(const std::string &name);

  static void registerPlugin(FactoryInterface* objectFactory);

protected:
  static PluginLister* _instance;

  /**
   * @brief Stores the the factory, dependencies, and parameters of all the plugins that register into this factory.
   **/
  static std::map<std::string , PluginDescription> plugins;

  /**
   * @brief Gets the release number of the given plug-in.
   *
   * @param name The name of the plug-in to retrieve the version number of.
   * @return :string The version number, ussually formatted as X[.Y], where X is the major, and Y the minor.
   **/
  static std::string getPluginRelease(std::string name);
};

/*@}*/

}

#endif //TULIP_PLUGINLISTER_H
