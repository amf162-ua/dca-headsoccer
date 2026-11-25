# ==========================================
# VARIABLES PRINCIPALES
# ==========================================
TARGET := bin/game

USE_CCACHE ?= 1
ifeq ($(USE_CCACHE),1)
    CCACHE := $(shell which ccache 2>/dev/null)
    ifneq ($(CCACHE),)
        CXX := ccache g++
    else
        CXX := g++
    endif
else
    CXX := g++
endif

CXXFLAGS := -Wall -Wextra -Wno-unused-parameter -std=c++17 -MMD -MP

# --- RUTAS DE LIBRERÍAS Y ENLAZADO ---
# Se puede cambiar esta ruta si Raylib esta instalada en otro directorio.
RAYLIB_LIB_DIR ?= vendor/lib
INC_LOCAL := -isystem vendor/include
LDFLAGS   := -L$(RAYLIB_LIB_DIR) -lraylib -lX11 -lm -lpthread -ldl -lrt

SRC_DIR   := src
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj

# El paquete usa su propia compilacion para fijar las rutas de /usr.
# Asi no se reutiliza un ejecutable compilado para instalar en /usr/local.
DIST_DIR := dist
DEB_BUILD_DIR := $(abspath $(BUILD_DIR)/deb-build)
DEB_ROOT := $(abspath $(BUILD_DIR)/deb-root)

# ==========================================
# RUTAS DE INSTALACIÓN (make install)
# ==========================================
# Siguen las convenciones GNU: se pueden cambiar desde la línea de comandos.
#   make install                                       -> instala en /usr/local (necesita sudo)
#   make install PREFIX=/usr DESTDIR=build/deb-root     -> instala dentro de una carpeta (para el .deb)
# Las rutas coinciden con las de install() en CMakeLists.txt.
APP_NAME := head-soccer-clash
# Mismo nombre que el ejecutable de CMake y que Exec= del .desktop
BIN_NAME ?= game

PREFIX  ?= /usr/local
DESTDIR ?=

BINDIR  ?= $(PREFIX)/bin
DATADIR ?= $(PREFIX)/share/$(APP_NAME)
APPDIR  ?= $(PREFIX)/share/applications
ICONDIR ?= $(PREFIX)/share/icons/hicolor/scalable/apps

# Ficheros que se instalan (los opcionales solo si existen en el repositorio)
ASSETS_DIR   ?= assets
PKG_SHARE    := packaging/debian/usr/share
DESKTOP_FILE ?= $(PKG_SHARE)/applications/$(APP_NAME).desktop
ICON_FILE    ?= $(PKG_SHARE)/icons/hicolor/scalable/apps/$(APP_NAME).svg

# ResourcesManager busca los recursos instalados en $(DATADIR)/assets.
# OJO: la ruta queda fijada al compilar. Si cambias PREFIX, haz antes "make clean".
CXXFLAGS += -DGAME_INSTALL_DATA_DIR='"$(DATADIR)"'

# packaging/debian contiene ficheros del repositorio (.desktop, icono): no se puede
# usar como DESTDIR, porque install los pisaría y uninstall los borraría.
ifeq ($(abspath $(DESTDIR)),$(abspath packaging/debian))
    $(error No uses DESTDIR=packaging/debian: usa una carpeta temporal, p. ej. DESTDIR=build/deb-root)
endif

# ==========================================
# DETECCIÓN AUTOMÁTICA DE CÓDIGO (.cpp)
# ==========================================
SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

INC_DIRS  := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) $(INC_LOCAL)

# ==========================================
# REGLAS DEL MAKEFILE
# ==========================================
.PHONY: all clean info install uninstall dist

all: $(TARGET)

# Regla de enlazado del ejecutable
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# Regla de compilación de objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) bin/

# Instala el ejecutable y, si existen, los recursos, el .desktop y el icono.
# install -D crea las carpetas intermedias; -m fija los permisos.
install: $(TARGET)
	install -Dm755 $(TARGET) "$(DESTDIR)$(BINDIR)/$(BIN_NAME)"
ifneq ($(wildcard $(ASSETS_DIR)),)
	mkdir -p "$(DESTDIR)$(DATADIR)/assets"
	cp -r $(ASSETS_DIR)/. "$(DESTDIR)$(DATADIR)/assets/"
	chmod -R u=rwX,go=rX "$(DESTDIR)$(DATADIR)"
endif
ifneq ($(wildcard $(DESKTOP_FILE)),)
	install -Dm644 $(DESKTOP_FILE) "$(DESTDIR)$(APPDIR)/$(APP_NAME).desktop"
endif
ifneq ($(wildcard $(ICON_FILE)),)
	install -Dm644 $(ICON_FILE) "$(DESTDIR)$(ICONDIR)/$(APP_NAME).svg"
endif

# Deshace lo que hace install, usando las mismas rutas.
uninstall:
	rm -f "$(DESTDIR)$(BINDIR)/$(BIN_NAME)"
	rm -rf "$(DESTDIR)$(DATADIR)"
	rm -f "$(DESTDIR)$(APPDIR)/$(APP_NAME).desktop"
	rm -f "$(DESTDIR)$(ICONDIR)/$(APP_NAME).svg"

# Genera el .deb sin instalar nada en el sistema.
# Se reutiliza install sobre una carpeta temporal, nunca sobre packaging/debian.
dist:
	rm -rf "$(DEB_ROOT)"
	$(MAKE) install PREFIX=/usr DESTDIR="$(DEB_ROOT)" \
		BUILD_DIR="$(DEB_BUILD_DIR)" TARGET="$(DEB_BUILD_DIR)/bin/game"
	install -Dm644 packaging/debian/DEBIAN/control "$(DEB_ROOT)/DEBIAN/control"
	mkdir -p "$(DIST_DIR)"
	dpkg-deb --build --root-owner-group "$(DEB_ROOT)" "$(DIST_DIR)/"

info:
	@echo "Ficheros .cpp detectados:"
	@echo "$(SRCS)" | tr ' ' '\n'
	@echo "---------------------------------"
	@echo "Rutas de inclusión (-I):"
	@echo "$(INC_FLAGS)"
