////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.at
//
// Implementation file
//
//    Copyright (c) 2009 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
// load settings from a file (or get them via env-variables)
//
/////////////////////////////////////////////////////////

#include "GemSettings.h"

#include <map>
#include <stdlib.h>
#include <errno.h>

class GemSettings_data {
public:
  std::map <t_symbol*, t_atom*> data;
};


GemSettings*GemSettings::s_defaultconfig=NULL;

#define GEM_RTCONFIG_FILE "gem.conf"

const char*GemSettings::s_configfile[] = { 
#ifdef __linux__
/*
  $(pwd)/gem.conf
  ~/pd/gem.conf
  /etc/pd/gem.conf
*/
  "/etc/pd",
  "~/.pd", 
#elif defined __APPLE__
  "/Library/Pd",
  "~/Library/Pd",
/*

*/
#elif defined  _WIN32
  "%CommonProgramFiles%\\Pd",
  "%AppData%\\Pd",
/*

*/
#endif
  0};

GemSettings::GemSettings(void) :
  m_data(NULL)
{
  m_data = new GemSettings_data;

  int i=0;
  while(s_configfile[i]) {
    open(GEM_RTCONFIG_FILE, s_configfile[i]);
    i++;
  }
  open(GEM_RTCONFIG_FILE);

  print();
}

GemSettings::~GemSettings(void) {
  delete m_data;
  m_data=NULL;
}

bool GemSettings::open(const char*filename, const char*dirname) {
  t_binbuf*bb=binbuf_new();
  int r=0;

  if(dirname) {
    r=binbuf_read(bb, (char*)filename, (char*)dirname, 1);
  } else {
    r=binbuf_read_via_path(bb, (char*)filename, (char*)".", 1);
  }

  if(r){
    binbuf_free(bb);
    return false;
  }

  int ac=binbuf_getnatom(bb);
  t_atom*av=binbuf_getvec(bb);

  t_symbol*s=NULL;
  t_atom*a;
  int state=0; /* 0=(next is ID); 1=(next is value); 2=(next is ignored) */


  while(ac--) {
    if (av->a_type == A_SEMI) {
      // done
      if(NULL!=s) {
        setValue(s, a);
      }
      state=0;
      s=NULL;
    } else {
      switch (state) {
      case 0:
        s=atom_getsymbol(av);
        state=1;
        break;
      case 1:
        a=av;
        state=2;
        break;
      default:
        break;
      }
    }
    av++;
  }


  binbuf_free(bb);
  return false;
}

void GemSettings::print(void) {
  // std::map<parameters>::iterator iterator_name;
  std::map <t_symbol*, t_atom*>::iterator it;

  for(it = m_data->data.begin(); 
        it != m_data->data.end();
        it++)
    {
      startpost("key ['%s']: ", it->first->s_name);
      postatom(1, it->second);
      endpost();
    }
}


t_atom*GemSettings::getValue(t_symbol*s) {
  return m_data->data[s];
}

void GemSettings::setValue(t_symbol*s, t_atom*v) {
  m_data->data[s]=v;
}


/* static members */

GemSettings*GemSettings::init() {
  GemSettings::s_defaultconfig=new GemSettings(); 
  return GemSettings::s_defaultconfig;
}

t_atom*GemSettings::getEnv(t_symbol*s) {
  /* LATER prepend "GEM_" and uppercase everything */

  char*env=getenv(s->s_name);
  if(env) {
    t_atom*a=(t_atom*)getbytes(sizeof(t_atom));
    errno=0;
    long l=strtol(env, NULL, 0);
    if(0==errno) {
      SETFLOAT(a, l);
      return a;
    }
    double d=strtod(env, NULL);
    if(0==errno) {
      SETFLOAT(a, d);
      return a;
    }
    SETSYMBOL(a, gensym(env));
    return a;
  }
}


t_atom*GemSettings::get(t_symbol*s) {
  t_atom*ap=getEnv(s);
  if(ap)
    return ap;
  if(GemSettings::s_defaultconfig)
    return GemSettings::s_defaultconfig->getValue(s);
  return NULL;
}

void GemSettings::set(t_symbol*s, t_atom*v) {
  if(GemSettings::s_defaultconfig)
    GemSettings::s_defaultconfig->setValue(s, v);
}