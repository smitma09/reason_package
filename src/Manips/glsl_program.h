/*-----------------------------------------------------------------

GEM - Graphics Environment for Multimedia

Link a GLSL program/shader
 
 *  Created by tigital on 11/13/2005.
 *  Copyright 2005 tigital.
 
Copyright (c) 1997-1999 Mark Danks. mark@danks.org
Copyright (c) G�nther Geiger. geiger@epy.co.at
Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM. zmoelnig@iem.kug.ac.at
For information on usage and redistribution, and for a DISCLAIMER OF ALL
WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_GLSL_PROGRAM_H_
#define INCLUDE_GLSL_PROGRAM_H_

#include "Base/GemBase.h"

#define MAX_NUM_SHADERS 32

typedef union {
  GLuint  i;
  t_float f;
} t_floatuint;


/*-----------------------------------------------------------------
  -------------------------------------------------------------------
  CLASS
  glsl_program
    
  Link in a GLSL program/shader
    
  KEYWORDS
  
    
  DESCRIPTION

  -----------------------------------------------------------------*/
class GEM_EXTERN glsl_program : public GemBase
{
  CPPEXTERN_HEADER(glsl_program, GemBase)
    
    public:
  
  //////////
  // Constructor
  glsl_program(void);

 protected:
    
  //////////
  // Destructor
  virtual ~glsl_program(void);


  //////////
  // check openGL-extensions
  virtual bool 	isRunnable(void);
    	
  //////////
  // Do the rendering
  virtual void 	renderGL2(void);
  virtual void 	renderARB(void);
  virtual void 	render(GemState *state);
  
  //////////
  // Clean up after rendering
  virtual void 	postrender(GemState *state);

  //////////
  // parameters to the glsl-program
  virtual void  paramMess(t_symbol*s, int argc, t_atom*argv);
  
  //////////
  // shader message
  virtual void 	shaderMess(int argc, t_atom *argv);
  
  //////////
  // Do the linking
  virtual bool 	LinkGL2(void);
  virtual bool 	LinkARB(void);
  virtual void 	LinkProgram(void);
  
  //////////
  // What can we play with?
  virtual void 	getVariables(void);

  void createArrays(void);
  void destroyArrays(void);
  
  //////////
  // Print Info about Hardware limits
  virtual void printInfo(void);

  GLuint		m_program;
  GLuint		m_shaderObj[MAX_NUM_SHADERS];

  GLhandleARB		m_programARB;
  GLhandleARB		m_shaderObjARB[MAX_NUM_SHADERS];

  GLint			m_maxLength;
  
  //////////
  // Variables for the, uh, variables
  GLint			m_uniformCount;
  t_symbol  **m_symname;
  GLint			*m_size;
  GLenum		*m_type;
  GLint       *m_loc;
  float			 **m_param;
  int			 *m_flag;
  
  GLint		m_linked;
  bool        m_wantLink;
  int		  m_num;
  
  t_outlet	 *m_outProgramID;

  ////////
  // for geometry shaders we need a bit more...
  virtual void intypeMess(GLuint intype);
  GLuint m_geoInType;
  virtual void outtypeMess(GLuint outtype);
  GLuint m_geoOutType;
  virtual void outverticesMess(GLint);
  GLint  m_geoOutVertices;
 

 private:
	
  //////////
  // static member functions
  static void paramMessCallback (void *data, t_symbol *, int, t_atom*);
  static void shaderMessCallback (void *data, t_symbol *, int, t_atom*);
  static void openMessCallback   (void *data, t_symbol *filename);
  static void linkCallback  (void *, t_symbol*, int, t_atom*);
  static void printMessCallback  (void *);

  static void intypeMessCallback  (void *, t_symbol*, int, t_atom*);
  static void outtypeMessCallback  (void *, t_symbol*, int, t_atom*);
  static void typeMessCallback  (void *, t_symbol*, int, t_atom*);
  static void outverticesMessCallback  (void *, t_floatarg);

};

#endif	// for header file