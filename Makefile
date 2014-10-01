# hexD LV2 plugins Makefile
# License is GPLv3, see COPYING.txt for more details.
# Created on 2014-09-30 19:30:05 BRT
# by Danilo J. S. Bellini

# File and plugin names
NAMES = $(shell ls -d hexd-*/ | tr -d /)
PLUGINS = $(NAMES:=.so)
URI_BASE = http://github.com/danilobellini/hexd-plugins-lv2/

# LV2 information
LV2PREFIX = $(shell pkg-config lv2 --variable=prefix)

# Install path, with many locations possible. Uses $(DESTDIR) and $(prefix)
# conventions. Installs globally when running with sudo, and locally otherwise
PREFIX = $(or $(prefix), $(LV2PREFIX), /usr/local)
DEFAULT_PATH = $(if $(filter $(USER), root),$(PREFIX)/lib/lv2,$(HOME)/.lv2)
INSTALL_PATH = $(or $(DESTDIR), $(DEFAULT_PATH))

# Install/uninstall targets naming information
INSTALL_TARGET_PREFIX = install-
INSTALL_NAMES = $(foreach name, $(NAMES), $(INSTALL_TARGET_PREFIX)$(name))
UNINSTALL_TARGET_PREFIX = uninstall-
UNINSTALL_NAMES = $(foreach name, $(NAMES), $(UNINSTALL_TARGET_PREFIX)$(name))

# Compiler/linker parameters
SOURCE_EXT = c
CC = gcc
CFLAGS = -Ofast -Wall -march=native -mtune=native -fPIC
LDFLAGS = -shared

# Helper function for getting a "#define" line using the C Preprocessor. Calls
#   $(call get_define,a,B)
# gets from file a/a.$(SOURCE_EXT) the "#define B Some value here" contents
# (i.e., only the "Some value here" string in that example)
get_define = $(shell cpp -dM $(1)/$(1).$(SOURCE_EXT) \
                   | grep ^.define.$(2) \
                   | cut -d" " -f1,2 --complement)

#
# Compiler targets
#
all: $(NAMES)

$(NAMES): %: %.so

define so_to_obj_targets_template

$(1).so: HEXD_LIBS = $(call get_define,$(1),HEXD_LIBS)
$(1).so: $(1)/$(1).o
	$(CC) -o $$@ $(LDFLAGS) $$< $$(HEXD_LIBS)

$(1)/$(1).o: PLUGIN_URI = "\"$(URI_BASE)$(1)\""
$(1)/$(1).o: CFILE = $(1)/$(1).$(SOURCE_EXT)
$(1)/$(1).o:
	$(CC) -c $(CFLAGS) -DPLUGIN_URI=$$(PLUGIN_URI) -o $$@ $$(CFILE)

endef

$(foreach name, $(NAMES), $(eval $(call so_to_obj_targets_template,$(name))))


#
# Cleanup target
#
clean: O_FILES = $(foreach name, $(NAMES), $(name)/$(name).o)
clean:
	rm -f $(O_FILES) $(PLUGINS)


#
# Install targets
#
install: $(INSTALL_NAMES)

$(INSTALL_NAMES): PLUGIN_NAME = $<
$(INSTALL_NAMES): PLUGIN_PATH = $(INSTALL_PATH)/$(PLUGIN_NAME).lv2
$(INSTALL_NAMES): $(INSTALL_TARGET_PREFIX)%: %
	mkdir -p $(PLUGIN_PATH)
	cp $(PLUGIN_NAME).so $(PLUGIN_PATH)
	cp $(PLUGIN_NAME)/manifest.ttl $(PLUGIN_PATH)


#
# Uninstall targets
#
uninstall: $(UNINSTALL_NAMES)

define do_uninstall
	rm -f $(PLUGIN_PATH)/$(PLUGIN_NAME).so
	rm -f $(PLUGIN_PATH)/manifest.ttl
	rmdir --ignore-fail-on-non-empty $(PLUGIN_PATH)
endef

define cant_uninstall
	@echo "Can't uninstall '$(PLUGIN_PATH)' as it doesn't exist"
endef

$(UNINSTALL_NAMES): PLUGIN_NAME = $(@:$(UNINSTALL_TARGET_PREFIX)%=%)
$(UNINSTALL_NAMES): PLUGIN_PATH = $(INSTALL_PATH)/$(PLUGIN_NAME).lv2
$(UNINSTALL_NAMES): PLUGIN_PATH_EXISTS = $(wildcard $(PLUGIN_PATH))
$(UNINSTALL_NAMES):
	$(if $(PLUGIN_PATH_EXISTS), $(do_uninstall), $(cant_uninstall))


# Information about targets that aren't filenames
.PHONY: all clean install $(INSTALL_NAMES) uninstall $(UNINSTALL_NAMES)

