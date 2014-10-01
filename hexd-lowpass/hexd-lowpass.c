/*
 * Created on 2014-10-01 08:23:38 BRT
 * License is GPLv3, see COPYING.txt for more details.
 * @author: Danilo de Jesus da Silva Bellini
 */
#include "hexd.h"
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

static void activate(LV2_Handle instance){
  Plugin *plugin = (Plugin*)instance;
  float x = FILTER_X(DEFAULT_CUTOFF * plugin->Hz);
  plugin->old_cutoff = DEFAULT_CUTOFF;
  plugin->radius = FILTER_R(x);
  plugin->mem = 0;
}

static void run(LV2_Handle instance, uint32_t n){
  Plugin *plugin = (Plugin*)instance;
  uint32_t t;

  float old_cutoff = plugin->old_cutoff,
        new_cutoff = *(plugin->cutoff),
        old_radius = plugin->radius,
        x, radius, new_radius, gain;

  if(old_cutoff != new_cutoff){ /* Should interpolate! */
    x = FILTER_X(new_cutoff * plugin->Hz);
    new_radius = FILTER_R(x);

    plugin->radius = new_radius;
    plugin->old_cutoff = new_cutoff;

    hexdCInterpolatorGlobals cg;
    hexdCInterpolator ci;
    HEXD_C_INTERPOLATOR_GLOBALS_INIT(cg, n);
    HEXD_C_INTERPOLATOR_INIT(ci, old_radius, new_radius);

    for(t = 0; t < n; t++){
      radius = HEXD_C_INTERPOLATE(cg, ci, t);
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

/* Creates the remaining functions and exports the plugin descriptor */
HEXD_DEFAULT_INSTANTIATE_RATE_HZ
HEXD_DEFAULT_CONNECT_PORT
HEXD_DEFAULT_DEACTIVATE
HEXD_DEFAULT_CLEANUP
HEXD_DEFAULT_EXTENSION_DATA
HEXD_FINISH
