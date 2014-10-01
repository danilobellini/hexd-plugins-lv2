/*
 * Created on 2014-10-01 08:23:38 BRT
 * License is GPLv3, see COPYING.txt for more details.
 * @author: Danilo de Jesus da Silva Bellini
 */
#include "lv2.h"
#include <stdlib.h>
#include <math.h>

#define HEXD_LIBS -lm

#define TWO_PI 6.2831853071795864769252867665590057683943387987502
#define DEFAULT_CUTOFF 400

/* Equations taken from the default AudioLazy lowpass filter: */
/*   H(z) = (1 - r) / (1 - r * z^-1)                          */
/* see more at https://github.com/danilobellini/audiolazy     */
#define FILTER_X(freq) ( 2 - cos(freq)       )
#define FILTER_R(x)    ( x - sqrt(x * x - 1) )
#define FILTER_GAIN(r) ( 1 - r               )

typedef struct{ /* Ports (keeping manifest.ttl port index order) */
  float *in, *out, *cutoff;
  float old_cutoff, radius, mem;
  double rate, Hz;
} Plugin;

static LV2_Handle instantiate(const LV2_Descriptor *descr, double rate,
                              const char* bundle_path,
                              const LV2_Feature* const* features){
  Plugin *plugin = (Plugin*)malloc(sizeof(Plugin));
  plugin->rate = rate;
  plugin->Hz = TWO_PI / rate; /* Hz to rad/sample unit converter */
  return (LV2_Handle)plugin;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data){
  ((float**)instance)[port] = (float*)data;
}

static void activate(LV2_Handle instance){
  Plugin *plugin = (Plugin*)instance;
  float x = FILTER_X(DEFAULT_CUTOFF * plugin->Hz);
  plugin->old_cutoff = DEFAULT_CUTOFF;
  plugin->radius = FILTER_R(x);
  plugin->mem = 0;
}

static void run(LV2_Handle instance, uint32_t n){
  Plugin *plugin = (Plugin*)instance;
  uint32_t m, m3, mmm, t;

  float old_cutoff = plugin->old_cutoff,
        new_cutoff = *(plugin->cutoff),
        old_radius = plugin->radius,
        x, radius, new_radius, gain, delta;

  if(old_cutoff != new_cutoff){ /* Should interpolate! */
    x = FILTER_X(new_cutoff * plugin->Hz);
    new_radius = FILTER_R(x);

    plugin->radius = new_radius;
    plugin->old_cutoff = new_cutoff;

    m = n - 1; /* Interpolator design is in the control_interpolator.py */
    m3 = 3 * m;
    mmm = m * m * m;
    delta = new_radius - old_radius;

    for(t = 0; t < n; t++){
      radius = old_radius + delta * ((m3 - (t << 1)) * (t * t)) / mmm;
      plugin->mem = plugin->out[t] = plugin->in[t] * FILTER_GAIN(radius)
                                   + plugin->mem * radius;
    }

  }else{ /* Keeping the same radius from before! */
    gain = FILTER_GAIN(old_radius);
    for(t = 0; t < n; t++)
      plugin->mem = plugin->out[t] = plugin->in[t] * gain
                                   + plugin->mem * old_radius;
  }
}

static void deactivate(LV2_Handle instance){
}

static void cleanup(LV2_Handle instance){
  free((Plugin*)instance);
}

static const void *extension_data(const char* uri){
  return NULL;
}

static const LV2_Descriptor Descr = {PLUGIN_URI, instantiate, connect_port,
                                     activate, run, deactivate, cleanup,
                                     extension_data};
LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t idx){
  return idx == 0 ? &Descr : NULL;
}
