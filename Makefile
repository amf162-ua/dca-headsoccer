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
# -Lvendor/lib le dice al compilador que busque libraylib.a en nuestra carpeta local
INC_LOCAL := -isystem vendor/include
LDFLAGS   := -Lvendor/lib -lraylib -lX11 -lm -lpthread -ldl -lrt

SRC_DIR   := src
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj

# ==========================================
# RUTAS DE INSTALACIÓN (make install)
# ==========================================
# Siguen las convenciones GNU: se pueden cambiar desde la línea de comandos.
#   make install                                  -> instala en /usr/local (necesita sudo)
#   make install PREFIX=/usr DESTDIR=paquete/raiz  -> instala dentro de una carpeta (para el .deb)
APP_NAME := head-soccer-clash

PREFIX  ?= /usr/local
DESTDIR ?=

BINDIR  ?= $(PREFIX)/games
DATADIR ?= $(PREFIX)/share/$(APP_NAME)
APPDIR  ?= $(PREFIX)/share/applications
ICONDIR ?= $(PREFIX)/share/pixmaps

# Ficheros opcionales: solo se instalan si existen en el repositorio
ASSETS_DIR   ?= assets
DESKTOP_FILE ?= packaging/$(APP_NAME).desktop
ICON_FILE    ?= packaging/$(APP_NAME).png

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
.PHONY: all clean info install uninstall

all: $(TARGET)

# Regla de enlazado del ejecutable
$(TARGET): $(OBJS) | bin
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# Regla de compilación de objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c $< -o $@

bin $(OBJ_DIR):
	mkdir -p $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) bin/

# Instala el ejecutable y, si existen, los recursos, el .desktop y el icono.
# install -D crea las carpetas intermedias; -m fija los permisos.
install: $(TARGET)
	install -Dm755 $(TARGET) "$(DESTDIR)$(BINDIR)/$(APP_NAME)"
ifneq ($(wildcard $(ASSETS_DIR)),)
	mkdir -p "$(DESTDIR)$(DATADIR)"
	cp -r $(ASSETS_DIR)/. "$(DESTDIR)$(DATADIR)/"
	chmod -R u=rwX,go=rX "$(DESTDIR)$(DATADIR)"
endif
ifneq ($(wildcard $(DESKTOP_FILE)),)
	install -Dm644 $(DESKTOP_FILE) "$(DESTDIR)$(APPDIR)/$(APP_NAME).desktop"
endif
ifneq ($(wildcard $(ICON_FILE)),)
	install -Dm644 $(ICON_FILE) "$(DESTDIR)$(ICONDIR)/$(APP_NAME).png"
endif

# Deshace lo que hace install, usando las mismas rutas.
uninstall:
	rm -f "$(DESTDIR)$(BINDIR)/$(APP_NAME)"
	rm -rf "$(DESTDIR)$(DATADIR)"
	rm -f "$(DESTDIR)$(APPDIR)/$(APP_NAME).desktop"
	rm -f "$(DESTDIR)$(ICONDIR)/$(APP_NAME).png"

info:
	@echo "Ficheros .cpp detectados:"
	@echo "$(SRCS)" | tr ' ' '\n'
	@echo "---------------------------------"
	@echo "Rutas de inclusión (-I):"
	@echo "$(INC_FLAGS)"