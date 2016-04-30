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

#ifndef __SOUNDFXH__
#define __SOUNDFXH__

#include <OGRE/Ogre.h>
#include <OGRE/OgreResourceManager.h>
#include <OGRE/OgreSharedPtr.h>
#include <SDL2/SDL_mixer.h>

// Clase para mezclar efectos de sonido.
// Delega en SDL mixer.
class SoundFX: public Ogre::Resource {
 public:
  // Constructor (ver Ogre::Resource).
  SoundFX(Ogre::ResourceManager* creator, const Ogre::String& resource_name,
	  Ogre::ResourceHandle handle, const Ogre::String& resource_group,
	  bool isManual = false, Ogre::ManualResourceLoader* loader = 0);  
  ~SoundFX();

  int play(int loop = 0);
        
 protected:
  void loadImpl();
  void unloadImpl();
  size_t calculateSize() const;
  
 private:
  Mix_Chunk* _pSound; // Info sobre el efecto de sonido.
  Ogre::String _path; // Ruta completa al efecto de sonido.
  size_t _size;       // Tamaño del efecto (bytes).
};

// Puntero inteligente a SoundFX.
typedef Ogre::SharedPtr<SoundFX> SoundFXPtr;

#endif
