/*
 * Created on 2014-10-01 08:48:37 BRT
 * License is GPLv3, see COPYING.txt for more details.
 * @author: Danilo de Jesus da Silva Bellini
 */
#include "lv2.h"
#include <stdlib.h>

#define TWO_PI 6.2831853071795864769252867665590057683943387987502

/*
 * Boilerplate for "static void" functions
 */
#define HEXD_VOID(name, stmts)  static void name(LV2_Handle instance){ stmts }
#define HEXD_DEFAULT_ACTIVATE   HEXD_VOID(activate,)
#define HEXD_DEFAULT_DEACTIVATE HEXD_VOID(deactivate,)
#define HEXD_DEFAULT_CLEANUP    HEXD_VOID(cleanup, free((Plugin*)instance);)


/*
 * "instantiate" function boilerplate
 */
#define HEXD_INSTANTIATE_PARAMS const LV2_Descriptor *descr, double rate, \
                                const char* bundle_path,                  \
                                const LV2_Feature* const* features

#define HEXD_DEFAULT_INSTANTIATE_WITHOUT_RATE             \
  static LV2_Handle instantiate(HEXD_INSTANTIATE_PARAMS){ \
    return (LV2_Handle)malloc(sizeof(Plugin));            \
  }

#define HEXD_DEFAULT_INSTANTIATE_RATE_HZ                              \
  static LV2_Handle instantiate(HEXD_INSTANTIATE_PARAMS){             \
    Plugin *plugin = (Plugin*)malloc(sizeof(Plugin));                 \
    plugin->rate = rate;                                              \
    plugin->Hz = TWO_PI / rate; /* Hz to rad/sample unit converter */ \
    return (LV2_Handle)plugin;                                        \
  }


/*
 * "connect_port" function boilerplate. In order to use this one, the Plugin
 * struct MUST always begin with the port pointers, keeping the same
 * ordering used by the manifest.ttl index values
 */
#define HEXD_DEFAULT_CONNECT_PORT                                           \
  static void connect_port(LV2_Handle instance, uint32_t port, void *data){ \
    ((float**)instance)[port] = (float*)data;                               \
  }


/*
 * "extension_data" function boilerplate
 */
#define HEXD_DEFAULT_EXTENSION_DATA \
  static const void *extension_data(const char* uri){ return NULL; }


/*
 * LV2 plugin symbol export boilerplate
 */
#define HEXD_FINISH static const LV2_Descriptor PluginDescription = {        \
  PLUGIN_URI, instantiate, connect_port, activate, run, deactivate, cleanup, \
  extension_data                                                             \
};                                                                           \
LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index){      \
  return index == 0 ? &PluginDescription : NULL;                             \
}


/*
 * Cubic interpolator. The design is better explained in the Python script
 * "control_interpolator.py", using the Sympy CAS to get the equations below
 */
typedef struct{ uint32_t m3, mmm; } hexdCInterpolatorGlobals;
typedef struct{ float old, delta; } hexdCInterpolator;

#define HEXD_C_INTERPOLATOR_GLOBALS_INIT(cg, n) { uint32_t m = (n) - 1;   \
                                                  (cg).m3 = 3 * m;        \
                                                  (cg).mmm = m * m * m; }

#define HEXD_C_INTERPOLATOR_INIT(ci, old_, new_) \
  { (ci).old = (old_);                           \
    (ci).delta = (new_) - (old_); }

#define HEXD_C_INTERPOLATE(cg, ci, t) \
  ( (ci).old + (ci).delta * ( (cg).m3 - ((t)<<1) ) * ((t)*(t)) / (cg).mmm )

