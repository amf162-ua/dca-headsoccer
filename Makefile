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
.PHONY: all clean info

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

info:
	@echo "Ficheros .cpp detectados:"
	@echo "$(SRCS)" | tr ' ' '\n'
	@echo "---------------------------------"
	@echo "Rutas de inclusión (-I):"
	@echo "$(INC_FLAGS)"