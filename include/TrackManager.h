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

#ifndef __TRACKMANAGERH__
#define __TRACKMANAGERH__

#include <OGRE/Ogre.h>
#include <Track.h>

// Clase encargada de gestionar recursos del tipo "Track".
// Funcionalidad heredada de Ogre::ResourceManager
// y Ogre::Singleton.
class TrackManager: public Ogre::ResourceManager,
                    public Ogre::Singleton<TrackManager> {
 public:
  TrackManager();
  virtual ~TrackManager();
  virtual TrackPtr load (const Ogre::String& name,
			 const Ogre::String& group = \
			 Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  static TrackManager& getSingleton ();
  static TrackManager* getSingletonPtr ();
  
 protected:
  Ogre::Resource* createImpl (const Ogre::String& name, Ogre::ResourceHandle handle,
			      const Ogre::String& group, bool isManual,
			      Ogre::ManualResourceLoader* loader,
			      const Ogre::NameValuePairList* createParams);
};

#endif
