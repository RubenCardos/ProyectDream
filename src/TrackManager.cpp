#include <TrackManager.h>

template<> TrackManager* Ogre::Singleton<TrackManager>::msSingleton = 0;

TrackManager::TrackManager() {
  // Tipo de recurso.
  mResourceType = "Track";
  // Prioridad.
  mLoadOrder = 30.f;
  // Registro del gestor en el sistema.
  Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
}

TrackManager::~TrackManager() {
  // Eliminación del registro.
  Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
}

TrackPtr TrackManager::load(const Ogre::String& name, const Ogre::String& group) {
  // Crea o recupera el recurso
  TrackPtr trackPtr = createOrRetrieve(name, group, false, 0, 0).first.staticCast<Track>();
 
  // Carga explícita del recurso.
  trackPtr->load();
 
  return trackPtr;
}

TrackManager& TrackManager::getSingleton() {
  assert(msSingleton);
  return (*msSingleton);
}
 
TrackManager* TrackManager::getSingletonPtr() {
  assert(msSingleton);
  return msSingleton;
}

// Creación de un nuevo recurso.
// No se llama a load().
Ogre::Resource* TrackManager::createImpl(const Ogre::String& resource_name,
					 Ogre::ResourceHandle handle,
					 const Ogre::String& resource_group,
					 bool isManual,
					 Ogre::ManualResourceLoader* loader,
					 const Ogre::NameValuePairList* createParams) {
  return new Track(this, resource_name, handle, resource_group, isManual, loader);
}
