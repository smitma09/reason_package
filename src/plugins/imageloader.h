/* -----------------------------------------------------------------

GEM - Graphics Environment for Multimedia

Load an image and return the frame(OS independant parent-class)

Copyright (c) 2011 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_IMAGELOADER_H_
#define INCLUDE_IMAGELOADER_H_

#include "Base/GemPixUtil.h"

#include <string>
#include "plugins/PluginFactory.h"

#include "Gem/Properties.h"


   /*-----------------------------------------------------------------
     -------------------------------------------------------------------
     CLASS
     imageloader
    
     parent class for the system- and library-dependent imageloader classes
    
     KEYWORDS
     pix load an image
    
     DESCRIPTION

     -----------------------------------------------------------------*/
   namespace gem { namespace plugins {
  class GEM_EXTERN imageloader
  {
  public:
  
    //////////
    // Constructor
  
    /* initialize the imageloader
     * set 'threadable' to FALSE if your implementation must NOT be used within
     * threads
     */
    imageloader(bool threadable=true);

    ////////
    // Destructor
    /* free what is apropriate */
    virtual ~imageloader();


    /* read a image
     *
     * props can be filled by the loader with additional information on the image
     * e.g. EXIF tags,...
     */
    /* returns TRUE if loading was successfull, FALSE otherwise */
    virtual bool load(std::string filename, imageStruct&result, gem::Properties&props) = 0;

    virtual bool isThreadable(void) { return m_threadable; }

  protected:
    /* used to store the "set" properties */
    gem::Properties m_properties;

  private:
    bool m_threadable;
  };

};}; // namespace gem


/**
 * \fn REGISTER_IMAGELOADERFACTORY(const char *id, Class imageloaderClass)
 * registers a new class "imageloaderClass" with the imageloader-factory
 *
 * \param id a symbolic (const char*) ID for the given class
 * \param imageloaderClass a class derived from "imageloader"
 */
#if 1
#define REGISTER_IMAGELOADERFACTORY(id, TYP) static gem::PluginFactoryRegistrar::registrar<TYP, gem::plugins::imageloader> fac_imageloader_ ## TYP (gensym(id)->s_name)
#else
#define REGISTER_IMAGELOADERFACTORY(id, TYP) static gem::PluginFactoryRegistrar::registrar<TYP, gem::plugins::imageloader> fac_imageloader_ ## TYP (id)
#endif


/**
 * \fn INIT_IMAGELOADERFACTORY()
 * initialized the factory
 * \note call this before any externals register themselves
 */
#define INIT_IMAGELOADERFACTORY()                                         \
  static gem::PluginFactoryRegistrar::dummy<gem::plugins::imageloader> fac_imageloaderdummy


#endif	// for header file
