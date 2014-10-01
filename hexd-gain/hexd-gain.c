/*
 * Created on 2014-10-01 00:15:42 BRT
 * License is GPLv3, see COPYING.txt for more details.
 * @author: Danilo de Jesus da Silva Bellini
 */
#include "lv2.h"
#include <stdlib.h>
#include <math.h>

#define URI "http://github.com/danilobellini/hexd-plugins-lv2/hexd-gain"

typedef struct{ /* Ports (keeping manifest.ttl port index order) */
  float *in, *out, *gain;
  float old;
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
  ((Plugin *)instance)->old = 1;
}

static void run(LV2_Handle instance, uint32_t n){
  Plugin *plugin = (Plugin*)instance;

  uint32_t m = n - 1,
           m3 = 3 * m, 
           mmm = m * m * m, 
           t;

  float old = plugin->old,
        new = pow(10, *(plugin->gain) / 20),
        delta = new - old;

  plugin->old = new;

  for(t = 0; t < n; t++)
    plugin->out[t] = plugin->in[t] * (
                       old + delta * ((m3 - (t << 1)) * (t * t)) / mmm
                     );
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
