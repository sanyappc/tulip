/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify  *
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tulip/PropertyManager.h"
#include "tulip/Graph.h"
#include "tulip/AbstractProperty.h"

using namespace std;
using namespace tlp;

//==============================================================
PropertyManagerImpl::PropertyManagerImpl(Graph *spGr) {
  graph=spGr;
}
//==============================================================
PropertyManagerImpl::~PropertyManagerImpl() {
  map<string,PropertyInterface*>::const_iterator itP;
  for (itP=propertyProxyMap.begin();itP!=propertyProxyMap.end();++itP) {
    PropertyInterface *prop = (*itP).second;
    prop->graph = NULL;
    delete prop;
  }
}
//==============================================================
bool PropertyManagerImpl::existProperty(const string &str) {
  if (existLocalProperty(str)) 
    return true;
  else {
    Graph* super = graph->getSuperGraph();
    if (super == graph)
      return false;
    else 
      return (super->existProperty(str));
  }
}
//==============================================================
bool PropertyManagerImpl::existLocalProperty(const string &str) {
  return (propertyProxyMap.find(str)!=propertyProxyMap.end());
}
//==============================================================
void PropertyManagerImpl::setLocalProxy(const string &str, PropertyInterface *p) {
  if (existLocalProperty(str))
    delete propertyProxyMap[str];
  propertyProxyMap[str]=p;
}
//==============================================================
PropertyInterface* PropertyManagerImpl::getProperty(const string &str) {
  assert(existProperty(str));
  //  cerr << "Get Proxy :" << str << endl;
  if (existLocalProperty(str))
    return this->getLocalProperty(str);
  else {
    Graph* super = graph->getSuperGraph();
    if (super != graph)
      return (super->getProperty(str));
  }
  return NULL;
}
//==============================================================
PropertyInterface* PropertyManagerImpl::getLocalProperty(const string &str) {
  assert(existLocalProperty(str));
  return (propertyProxyMap[str]);
}
//==============================================================
PropertyInterface* PropertyManagerImpl::delLocalProperty(const string &str) {
  map<string,PropertyInterface *>::iterator it;
  it=propertyProxyMap.find(str);
  if (it!=propertyProxyMap.end()) {
    PropertyInterface* prop = (*it).second;
    propertyProxyMap.erase(it);
    return prop;
  }
  return NULL;
}
Iterator<string>*  PropertyManagerImpl::getLocalProperties() {
  return (new LocalPropertiesIterator(this));
}
Iterator<string>*  PropertyManagerImpl::getInheritedProperties() {
  return (new InheritedPropertiesIterator(this));
}
//==============================================================
LocalPropertiesIterator::LocalPropertiesIterator(PropertyManagerImpl *ppc) {
  this->ppc=ppc;
  it=ppc->propertyProxyMap.begin();
  itEnd=ppc->propertyProxyMap.end();
}
string LocalPropertiesIterator::next() {
  string tmp=(*it).first;
  ++it;
  return tmp;
}
bool LocalPropertiesIterator::hasNext() {
  return (it!=itEnd);
}
//===============================================================
InheritedPropertiesIterator::InheritedPropertiesIterator(PropertyManager *ppc) {
  this->ppc=ppc; 
  if (ppc->graph->getSuperGraph()!=ppc->graph) {
    //R�cup�ration des propri�t�es locale du p�re.
    Iterator<string> *itS=ppc->graph->getSuperGraph()->getLocalProperties();
    for (;itS->hasNext();) {
      string tmp=itS->next();
      if (!ppc->existLocalProperty(tmp)) {
	inhList.insert(tmp);
      }
    } delete itS;
    //R�cup�ration des propri�t�es h�rit�es du p�re.
    itS=ppc->graph->getSuperGraph()->getInheritedProperties();
    for (;itS->hasNext();) {
      string tmp=itS->next();
      if (!ppc->existLocalProperty(tmp)) {
	inhList.insert(tmp);
      }
    }
    delete itS;  
  }
  it=inhList.begin();
  itEnd=inhList.end();
}
string InheritedPropertiesIterator::next() {
  string tmp=(*it);
  ++it;
  return tmp;
}
bool InheritedPropertiesIterator::hasNext() {
  return (it!=itEnd);
}
//===============================================================
void PropertyManagerImpl::erase(const node n) {
  map<string,PropertyInterface*>::iterator itP;
  for (itP=propertyProxyMap.begin();itP!=propertyProxyMap.end();++itP) {
    itP->second->erase(n);
  }
}
//===============================================================
void PropertyManagerImpl::erase(const edge e) {
  map<string,PropertyInterface*>::iterator itP;
  for (itP=propertyProxyMap.begin();itP!=propertyProxyMap.end();++itP) {
    itP->second->erase(e);
  }
}
