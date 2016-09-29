hexD LV2 plugins
================

These are simple LV2 plugins written in C based on the ``hexd.h`` macros.

They're simple, optimized, and based on a cubic interpolator for joining
the control input values where they differ. A CAS proof for the interpolator
equation coefficients is given in `scripts/control_interpolator.py`_\ ,
(requires Sympy, Python).

For some plugins (e.g. ``hexd-lowpass`` and ``hexd-highpass``), the goal
was to keep it exact directly with a digital filter design, instead of
an approximation from an analog filter design.

.. _`scripts/control_interpolator.py`: scripts/control_interpolator.py


Plugins
-------

For now, the plugins available are:

=============== ===================================================
Directory       Plugin name
=============== ===================================================
hexd-distwire   hexD DistWire distortion
hexd-dummy      hexD Dummy I/O 'wire' eternal sameness
hexd-gain       hexD Gain
hexd-highpass   hexD Lowpass filter (single pole, exact)
hexd-lowpass    hexD Highpass filter (one zero and one pole, exact)
=============== ===================================================


Installing
----------

Just type::

  make
  make install

If you want to install for all users, do the same with sudo.

The ``Makefile`` was highly customized to detect all plugins, and every
plugin can be made separately by name, e.g.::

  make hexd-distwire
  make install-hexd-distwire

You can also uninstall by name::

  make uninstall-hexd-distwire

Or uninstall every plugin with ``make uninstall``.

If you have bash completion for make commands, the given Makefile will
show/auto-complete the plugins by name.


Curiosity
---------

The "DistWire" distortion was created when I was advising/teaching
some DSP basics to some few students at Poli-USP (Brazil), and one of
them asked me for an example of a memoryless system. The example I've
created at that time is exactly the non-linear DISTWIRE macro in
the ``hexd-distwire.c`` file, and the distortion name was discussed
during the class, among the students. They said a sinusoid looked
like a barbed wire / barbwire after processing with this distortion,
hence the name "DistWire".


----

Copyright (C) 2014-2016 Danilo de Jesus da Silva Bellini
