/*
 * Created on 2014-10-01 00:15:42 BRT
 * License is GPLv3, see COPYING.txt for more details.
 * @author: Danilo de Jesus da Silva Bellini
 */
#include "hexd.h"
#include <math.h>

#define HEXD_LIBS -lm

typedef struct{ /* Ports (keeping manifest.ttl port index order) */
  float *in, *out, *gain;
  float old_gain, /* Old gain value in dB */
        old;      /* Same, but already linear (i.e., a multiplier) */
} Plugin;

static void activate(LV2_Handle instance){
  Plugin *plugin = (Plugin*)instance;
  plugin->old_gain = 0; /* 0dB gain ...           */
  plugin->old = 1;      /* ... or 1.0x multiplier */
}

static void run(LV2_Handle instance, uint32_t n){
  Plugin *plugin = (Plugin*)instance;
  uint32_t t;

  float old_gain = plugin->old_gain,
        new_gain = *(plugin->gain),
        old = plugin->old;

  if(old_gain != new_gain){ /* Should interpolate! */
    float new = pow(10, new_gain / 20);

    plugin->old = new;
    plugin->old_gain = new_gain;

    hexdCInterpolatorGlobals cg;
    hexdCInterpolator ci;
    HEXD_C_INTERPOLATOR_GLOBALS_INIT(cg, n);
    HEXD_C_INTERPOLATOR_INIT(ci, old, new);

    for(t = 0; t < n; t++)
      plugin->out[t] = plugin->in[t] * HEXD_C_INTERPOLATE(cg, ci, t);

  }else /* Constant gain, way simpler! */

    for(t = 0; t < n; t++)
      plugin->out[t] = plugin->in[t] * old;
}

/* Creates the remaining functions and exports the plugin descriptor */
HEXD_DEFAULT_INSTANTIATE_WITHOUT_RATE
HEXD_DEFAULT_CONNECT_PORT
HEXD_DEFAULT_DEACTIVATE
HEXD_DEFAULT_CLEANUP
HEXD_DEFAULT_EXTENSION_DATA
HEXD_FINISH
