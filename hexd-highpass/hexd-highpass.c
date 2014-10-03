/*
 * Created on 2014-10-02 22:40:32 BRT
 * License is GPLv3, see COPYING.txt for more details.
 * @author: Danilo de Jesus da Silva Bellini
 */
#include "hexd.h"
#include <math.h>

#define HEXD_LIBS -lm

#define DEFAULT_CUTOFF 200 /* From the manifest.ttl */

/* Equations taken from the default AudioLazy highpass filter: */
/*   H(z) = ((1 + r) / 2) * (1 - z^-1) / (1 - r * z^-1)        */
/* see more at https://github.com/danilobellini/audiolazy      */
#define FILTER_R(w, cos_w) ( cos_w ? ( (1 - sin(w)) / cos_w ) : 0 )
#define FILTER_GAIN(r)     ( (1 + r) * 0.5                        )

typedef struct{ /* Ports (keeping manifest.ttl port index order) */
  float *in, *out, *cutoff;
  float old_cutoff, r, mem, mem_in;
  double rate, Hz;
} Plugin;

static void activate(LV2_Handle instance){
  Plugin *plugin = (Plugin*)instance;
  float w = DEFAULT_CUTOFF * plugin->Hz;
  plugin->old_cutoff = DEFAULT_CUTOFF;
  plugin->r = FILTER_R(w, cos(w));
  plugin->mem = plugin->mem_in = 0;
}

static void run(LV2_Handle instance, uint32_t n){
  Plugin *plugin = (Plugin*)instance;
  uint32_t t;

  float old_cutoff = plugin->old_cutoff,
        new_cutoff = *(plugin->cutoff),
        old_r = plugin->r,
        w, r, new_r, gain;

  if(old_cutoff != new_cutoff){ /* Should interpolate! */
    w = new_cutoff * plugin->Hz;
    new_r = FILTER_R(w, cos(w));

    plugin->r = new_r;
    plugin->old_cutoff = new_cutoff;

    hexdCInterpolatorGlobals cg;
    hexdCInterpolator ci;
    HEXD_C_INTERPOLATOR_GLOBALS_INIT(cg, n);
    HEXD_C_INTERPOLATOR_INIT(ci, old_r, new_r);

    for(t = 0; t < n; t++){
      r = HEXD_C_INTERPOLATE(cg, ci, t);
      gain = FILTER_GAIN(r);
      plugin->mem = plugin->out[t] = (plugin->in[t] - plugin->mem_in) * gain
                                   + plugin->mem * r;
      plugin->mem_in = plugin->in[t];
    }

  }else{ /* Keeping filter data from before */
    gain = FILTER_GAIN(old_r);
    for(t = 0; t < n; t++){
      plugin->mem = plugin->out[t] = (plugin->in[t] - plugin->mem_in) * gain
                                   + plugin->mem * old_r;
      plugin->mem_in = plugin->in[t];
    }
  }
}

/* Creates the remaining functions and exports the plugin descriptor */
HEXD_DEFAULT_INSTANTIATE_RATE_HZ
HEXD_DEFAULT_CONNECT_PORT
HEXD_DEFAULT_DEACTIVATE
HEXD_DEFAULT_CLEANUP
HEXD_DEFAULT_EXTENSION_DATA
HEXD_FINISH
