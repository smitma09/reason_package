/*
 *  GEM - Graphics Environment for Multimedia
 *
 *  ripple.cpp
 *  gem_darwin
 *
 *  Created by Jamie Tittle on Sun Jan 19 2003.
 *  Copyright (c) 2003 tigital. All rights reserved.
 *    For information on usage and redistribution, and for a DISCLAIMER OF ALL
 *    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
 *
 */

#include "ripple.h"
#include "Base/GemState.h"
#include <string.h>
#include <math.h>
#include <Base/GemMan.h>

CPPEXTERN_NEW_WITH_TWO_ARGS(ripple, t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT)

/////////////////////////////////////////////////////////
//
// ripple
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
ripple :: ripple( t_floatarg width, t_floatarg height )
    	     : GemShape(width), m_height(height), m_size(0),
               alreadyInit(0)
{
    if (m_height == 0.f)
            m_height = 1.f;

    // the height inlet
    m_inletH = inlet_new(this->x_obj, &this->x_obj->ob_pd, &s_float, gensym("Ht"));
    inletcX = inlet_new(this->x_obj, &this->x_obj->ob_pd, &s_float, gensym("cX"));
    inletcY = inlet_new(this->x_obj, &this->x_obj->ob_pd, &s_float, gensym("cY"));
    inletfov = inlet_new(this->x_obj, &this->x_obj->ob_pd, &s_float, gensym("fov"));
    //m_drawType = GL_QUADS;
    m_drawType = GL_TRIANGLE_STRIP;
    alreadyInit = 0;
    xsize = 0.f;
    ysize = 0.f;
    grab = -1;
    precalc_ripple_amp();
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
ripple :: ~ripple()
{
	 inlet_free(m_inletH);
         inlet_free(inletcX);
         inlet_free(inletcY);
         inlet_free(inletfov);
         alreadyInit = 0;
}
/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void ripple :: render(GemState *state)
{
    int i, j;
#ifdef __APPLE__
    if (xsize != state->texCoords[1].s)
#else
    if (xsize != state->texCoords[2].s)
#endif
        alreadyInit = 0;
    
    if (!alreadyInit)
    {
#ifdef __APPLE__
        xsize = state->texCoords[1].s;
        ysize = state->texCoords[1].t;
#else
        xsize = state->texCoords[2].s;
        ysize = state->texCoords[2].t;
#endif
        win_size_x = GemMan::m_width;
        win_size_y = GemMan::m_height;
        xyratio = win_size_x/win_size_y;
        ripple_init();
        precalc_ripple_vector();
        alreadyInit = 1;
    }
    //glOrtho(-0.5, win_size_x - 0.5, -0.5, win_size_y - 0.5, CLIP_NEAR, CLIP_FAR);
    //glOrtho( 0, win_size_x, 0, win_size_y, CLIP_NEAR, CLIP_FAR);
    //glMatrixMode( GL_PROJECTION );
    //glLoadIdentity();
    //gluPerspective( fov, xyratio, 0.001, 1000000.0 );
    glNormal3f(0.0f, 0.0f, 1.0f);
    for (i = 0; i < GRID_SIZE_X - 1; i++)
    {
        for (j = 0; j < GRID_SIZE_Y - 1; j++)
        {
            glBegin(GL_POLYGON);
            glTexCoord2fv(ripple_vertex[i][j].t);
            glVertex2fv(ripple_vertex[i][j].x);
            glTexCoord2fv(ripple_vertex[i][j + 1].t);
            glVertex2fv(ripple_vertex[i][j + 1].x);
            glTexCoord2fv(ripple_vertex[i + 1][j + 1].t);
            glVertex2fv(ripple_vertex[i + 1][j + 1].x);
            glTexCoord2fv(ripple_vertex[i + 1][j].t);
            glVertex2fv(ripple_vertex[i + 1][j].x);
            glEnd();     
        }
            /*post("ripple_vertex[%d][%d].x[0] = %f",i,j,ripple_vertex[i][j].x[0]);
            post("ripple_vertex[%d][%d].x[1] = %f",i,j,ripple_vertex[i][j].x[1]);
            post("ripple_vertex[%d][%d].dt[0] = %f",i,j,ripple_vertex[i][j].dt[0]);
            post("ripple_vertex[%d][%d].dt[1] = %f",i,j,ripple_vertex[i][j].dt[1]);*/
    }
    ripple_dynamics();
}
/////////////////////////////////////////////////////////
//
//	ripple_init
//	Initialize ripple location and age information.
//
//	Also, precompute the vertex coordinates and the default texture
//	coordinates assigned to them.
/////////////////////////////////////////////////////////

void ripple :: ripple_init()
{
  int i, j;
  float x, y;

  glDisable(GL_DEPTH_TEST);

  //ripple_max = (int)sqrt(win_size_x*win_size_y+win_size_x*win_size_x);
  ripple_max = (int)sqrt(xsize * ysize + xsize * xsize);

  for (i = 0; i < RIPPLE_COUNT; i++)
  {
    t[i] = ripple_max + RIPPLE_LENGTH;
    cx[i] = 0;
    cy[i] = 0;
    max[i] = 0;
  }

  for (i = 0; i < GRID_SIZE_X; i++)
    for (j = 0; j < GRID_SIZE_Y; j++)
   {

      ripple_vertex[i][j].x[0] = (i/(GRID_SIZE_X - 1.0 ))-0.5;
      ripple_vertex[i][j].x[1] = (j/(GRID_SIZE_Y - 1.0 ))-0.5;
      ripple_vertex[i][j].dt[0] = xsize*(i/(GRID_SIZE_X - 1.0 ));
      ripple_vertex[i][j].dt[1] = ysize*(j/(GRID_SIZE_Y - 1.0 ));
      
      /*post("ripple_vertex[%d][%d].x[0] = %f",i,j,ripple_vertex[i][j].x[0]);
      post("ripple_vertex[%d][%d].x[1] = %f",i,j,ripple_vertex[i][j].x[1]);
      post("ripple_vertex[%d][%d].dt[0] = %f",i,j,ripple_vertex[i][j].dt[0]);
      post("ripple_vertex[%d][%d].dt[1] = %f",i,j,ripple_vertex[i][j].dt[1]);*/
    }
}

/////////////////////////////////////////////////////////
//	Precompute ripple displacement vectors.
/////////////////////////////////////////////////////////

void ripple :: precalc_ripple_vector()
{
  int i, j, z;
  float x, y, l;

  //post("RIPPLE_VECTOR ripple_vector[GRID_SIZE_X][GRID_SIZE_Y] =");
  //post("{");

  for (i = 0; i < GRID_SIZE_X; i++)
  {
    for (j = 0; j < GRID_SIZE_Y; j++)
    {
      x = (float) i/(GRID_SIZE_X - 1);
      y = (float) j/(GRID_SIZE_Y - 1);

      l = (float) sqrt(x*x + y*y);
      if (l == 0.0)
      {
	x = 0.0f;
	y = 0.0f;
      }
      else
      {
	x /= l;
	y /= l;
      }
      z = (int)(l*xsize*2);
      ripple_vector[i][j].dx[0] = x*xsize;
      ripple_vector[i][j].dx[1] = y*ysize;
      ripple_vector[i][j].r = z;
      
      //post("  %g, %g, %d", x, y, z);

/*     if (i == GRID_SIZE_X - 1 && j == GRID_SIZE_Y - 1)
	post("");
      else
	post(",");
*/
    }
  }

  //post("};");
}

/////////////////////////////////////////////////////////
//	Precompute ripple amplitude decay.
/////////////////////////////////////////////////////////

void ripple :: precalc_ripple_amp()
{
  int i;
  double t;
  double a;

  //post("RIPPLE_AMP ripple_amp[RIPPLE_LENGTH] =");
  //post("{");

  for (i = 0; i < RIPPLE_LENGTH; i++)
  {
    t = 1.0 - i/(RIPPLE_LENGTH - 1.0);
    a = (-cos(t*2.0*3.1428571*RIPPLE_CYCLES)*0.5 + 0.5)
      *RIPPLE_AMPLITUDE*t*t*t*t*t*t*t*t;
    if (i == 0)
      a = 0.0;
    ripple_amp[i].amplitude = a;

    //post("  %g", a);

/*    if (i == RIPPLE_LENGTH - 1)
      post("");
    else
      post(",");
*/
  }

  //post("};\n");
}

/////////////////////////////////////////////////////////
//
//	ripple_dynamics
//	Advance one time step and compute new texture coordinates
//	for the next frame of animation.
/////////////////////////////////////////////////////////

void ripple :: ripple_dynamics()
{
  int i, j, k;
  int x, y;
  int mi, mj;
  int r;
  float sx, sy;
  float amp;

  for (i = 0; i < RIPPLE_COUNT; i++)
    t[i] += RIPPLE_STEP;

  for (i = 0; i < GRID_SIZE_X; i++)
    for (j = 0; j < GRID_SIZE_Y; j++)
    {
      ripple_vertex[i][j].t[0] = ripple_vertex[i][j].dt[0];
      ripple_vertex[i][j].t[1] = ripple_vertex[i][j].dt[1];

      for (k = 0; k < RIPPLE_COUNT; k++)
      {
	x = i - cx[k];
	y = j - cy[k];
	if (x < 0)
	{
	  x *= -1;
	  sx = -1.0;
	}
	else
	  sx = 1.0;
	if (y < 0)
	{
	  y *= -1;
	  sy = -1.0;
	}
	else
	  sy = 1.0;
	mi = x;
	mj = y;
	
	r = t[k] - ripple_vector[mi][mj].r;
	
	if (r < 0)
	  r = 0;
	if (r > RIPPLE_LENGTH - 1)
	  r = RIPPLE_LENGTH - 1;

	amp = 1.0 - 1.0*t[k]/RIPPLE_LENGTH;
	amp *= amp;
	if (amp < 0.0)
	  amp = 0.0;
	
	ripple_vertex[i][j].t[0]
	  += ripple_vector[mi][mj].dx[0]*sx*ripple_amp[r].amplitude*amp;
	ripple_vertex[i][j].t[1]
	  += ripple_vector[mi][mj].dx[1]*sy*ripple_amp[r].amplitude*amp;
      }
    }
}
/////////////////////////////////////////////////////////
//
//	ripple_distance
//
//	Calculate the distance between two points.
//
/////////////////////////////////////////////////////////
float ripple :: ripple_distance(int gx, int gy, int cx, int cy)
{
  return sqrt(1.0*(gx - cx)*(gx - cx) + 1.0*(gy - cy)*(gy - cy));
}

/////////////////////////////////////////////////////////
//
//	ripple_max_distance
//
//	Compute the distance of the given window coordinate
//	to the nearest window corner, in pixels.
/////////////////////////////////////////////////////////
int ripple :: ripple_max_distance(int gx, int gy)
{
  float d;
  float temp_d;

  d = ripple_distance(gx, gy, 0, 0);
  temp_d = ripple_distance(gx, gy, GRID_SIZE_X, 0);
  if (temp_d > d)
    d = temp_d;
  temp_d = ripple_distance(gx, gy, GRID_SIZE_X, GRID_SIZE_Y);
  if (temp_d > d)
    d = temp_d;
  temp_d = ripple_distance(gx, gy, 0, GRID_SIZE_Y);
  if (temp_d > d)
    d = temp_d;

  return (int)((d/GRID_SIZE_X)*win_size_x + RIPPLE_LENGTH/6);
}
/////////////////////////////////////////////////////////
//	ripple_grab
//
//	Generate a new ripple when the mouse is pressed.  There's
//	a limit on the number of ripples that can be simultaneously
//	generated.
/////////////////////////////////////////////////////////
void ripple :: ripple_grab()
{
    int index = 0;
    
    while (t[index] < max[index] && index < RIPPLE_COUNT)
      index++;
    
    if (index < RIPPLE_COUNT)
    {
      cx[index] = 1.0*ctrX/xsize*GRID_SIZE_X;
      cy[index] = 1.0*ctrY/ysize*GRID_SIZE_Y;
      t[index] = 4*RIPPLE_STEP;
      max[index] = ripple_max_distance(cx[index], cy[index]);
      
      //post("cx[%d] = %d",index, cx[index]);
      //post("cy[%d] = %d",index, cy[index]);
      //post("max[%d] = %d",index, max[index]);
    }
  
}

/////////////////////////////////////////////////////////
// bangMess
//
/////////////////////////////////////////////////////////
void ripple :: ripple_bang()
{
    if ( grab == -1 ){ 
        //grab = ripple_grab();
        post("ripple: grab = %d\n",grab);
    }else{
        grab = -1;
    }
}

/////////////////////////////////////////////////////////
// heightMess
//
/////////////////////////////////////////////////////////
void ripple :: heightMess(float height)
{
    m_height = height;
    setModified();
}
/////////////////////////////////////////////////////////
// ctrXMess
//
/////////////////////////////////////////////////////////
void ripple :: ctrXMess(float center)
{
    ctrX = (short)center;
    //post("ripple: ctrX = %d\n",ctrX);
    setModified();
}
/////////////////////////////////////////////////////////
// ctrYMess
//
/////////////////////////////////////////////////////////
void ripple :: ctrYMess(float center)
{
    ctrY = (short)center;
    //post("ripple: ctrY = %d\n",ctrY);
    setModified();
}

/////////////////////////////////////////////////////////
// typeMess
//
/////////////////////////////////////////////////////////
void ripple :: typeMess(t_symbol *type)
{
    if (!strcmp(type->s_name, "line")) 
	    m_drawType = GL_LINE_LOOP;
    else if (!strcmp(type->s_name, "fill")) 
	    m_drawType = GL_TRIANGLE_STRIP;
    else if (!strcmp(type->s_name, "point"))
	    m_drawType = GL_POINTS;
    else
    {
	    error ("GEM: square draw style");
	    return;
    }
    setModified();
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void ripple :: obj_setupCallback(t_class *classPtr)
{
    class_addbang(classPtr, (t_method)&ripple::bangMessCallback);
    class_addmethod(classPtr, (t_method)&ripple::heightMessCallback,
    	    gensym("Ht"), A_FLOAT, A_NULL);
    class_addmethod(classPtr, (t_method)&ripple::ctrXMessCallback,
    	    gensym("cX"), A_FLOAT, A_NULL);
    class_addmethod(classPtr, (t_method)&ripple::ctrYMessCallback,
    	    gensym("cY"), A_FLOAT, A_NULL);
    class_addmethod(classPtr, (t_method)&ripple::fovMessCallback,
    	    gensym("fov"), A_FLOAT, A_NULL);
}

void ripple :: bangMessCallback(void *data)
{
    GetMyClass(data)->ripple_grab();
}
void ripple :: heightMessCallback(void *data, t_floatarg height)
{
    GetMyClass(data)->heightMess((float)height);
}
void ripple :: ctrXMessCallback(void *data, t_floatarg center)
{
    GetMyClass(data)->ctrXMess((float)center);
}
void ripple :: ctrYMessCallback(void *data, t_floatarg center)
{
    GetMyClass(data)->ctrYMess((float)center);
}
void ripple :: fovMessCallback(void *data, t_floatarg size)
{
    GetMyClass(data)->fov=((float)size);
}