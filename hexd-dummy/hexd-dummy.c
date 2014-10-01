/*
 * Created on 2014-09-30 23:16:27 BRT
 * License is GPLv3, see COPYING.txt for more details.
 * @author: Danilo de Jesus da Silva Bellini
 */
#include "lv2.h"
#include <stdlib.h>

#define URI "http://github.com/danilobellini/hexd-plugins-lv2/hexd-dummy"

typedef struct{ /* Ports (keeping manifest.ttl port index order) */
  float *in, *out;
} Plugin;

static LV2_Handle instantiate(const LV2_Descriptor *descr, double rate,
                              const char* bundle_path,
                              const LV2_Feature* const* features){
  return (LV2_Handle)malloc(sizeof(Plugin));
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data){
  ((float**)instance)[port] = (float*)data;
}

static void activate(LV2_Handle instance){
}

static void run(LV2_Handle instance, uint32_t n){
  Plugin *plugin = (Plugin*)instance;
  float *last, *in, *out;
  for(in = plugin->in, out=plugin->out, last=in + n; in <= last; in++, out++)
    *out = *in;
}

static void deactivate(LV2_Handle instance){
}

static void cleanup(LV2_Handle instance){
  free((Plugin*)instance);
}

static const void *extension_data(const char* uri){
  return NULL;
}

static const LV2_Descriptor PluginDescr = {URI, instantiate, connect_port,
                                           activate, run, deactivate, cleanup,
                                           extension_data};
LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t idx){
  return idx == 0 ? &PluginDescr : NULL;
}
