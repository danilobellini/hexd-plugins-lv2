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
  ((Plugin *)instance)->old_gain = 0; /* 0dB gain ...           */
  ((Plugin *)instance)->old = 1;      /* ... or 1.0x multiplier */
}

static void run(LV2_Handle instance, uint32_t n){
  Plugin *plugin = (Plugin*)instance;
  uint32_t m, m3, mmm, t;

  float old_gain = plugin->old_gain,
        new_gain = *(plugin->gain),
        old, new, delta;

  if(old_gain != new_gain){ /* Should interpolate! */

    old = plugin->old;
    new = pow(10, new_gain / 20);
    delta = new - old;

    plugin->old = new;
    plugin->old_gain = new_gain;

    m = n - 1; /* Interpolator design is in the control_interpolator.py */
    m3 = 3 * m;
    mmm = m * m * m;

    for(t = 0; t < n; t++)
      plugin->out[t] = plugin->in[t] * (
                         old + delta * ((m3 - (t << 1)) * (t * t)) / mmm
                       );

  }else{ /* Constant gain, way simpler! */
    old = plugin->old;
    for(t = 0; t < n; t++)
      plugin->out[t] = plugin->in[t] * old;
  }
}

/* Creates the remaining functions and exports the plugin descriptor */
HEXD_DEFAULT_INSTANTIATE_WITHOUT_RATE
HEXD_DEFAULT_CONNECT_PORT
HEXD_DEFAULT_DEACTIVATE
HEXD_DEFAULT_CLEANUP
HEXD_DEFAULT_EXTENSION_DATA
HEXD_FINISH
