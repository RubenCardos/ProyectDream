/*********************************************************************
 * Módulo 1. Curso de Experto en Desarrollo de Videojuegos
 * Autor: David Vallejo Fernández    David.Vallejo@uclm.es
 * Autor: Carlos González Morcillo  Carlos.Gonzalez@uclm.es
 *
 * Código modificado a partir del código de IberOgre
 * http://www.ogre3d.org/tikiwiki/Managing+Game+States+with+OGRE
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#ifndef __SOUNDFXMANAGERH__
#define __SOUNDFXMANAGERH__

#include <OGRE/Ogre.h>
#include "SoundFX.h"

// Clase encargada de gestionar recursos del tipo "SoundFX".
// Funcionalidad heredada de Ogre::ResourceManager
// y Ogre::Singleton.
class SoundFXManager: public Ogre::ResourceManager,
                      public Ogre::Singleton<SoundFXManager> {
 public:
  SoundFXManager();
  virtual ~SoundFXManager();

  virtual SoundFXPtr load(const Ogre::String& name,
			  const Ogre::String& group = \
			  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

  static SoundFXManager& getSingleton();
  static SoundFXManager* getSingletonPtr();

  static int getAvailableChannels();
  
 protected:
  Ogre::Resource* createImpl(const Ogre::String& name, Ogre::ResourceHandle handle,
			     const Ogre::String& group, bool isManual,
			     Ogre::ManualResourceLoader* loader,
			     const Ogre::NameValuePairList* createParams);
    
 private:
  static int _numChannels;
};

#endif
