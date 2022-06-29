SRC = ./FUENTES/src
INC = ./FUENTES/include
OBJ = ./FUENTES/obj
BIN = ./BIN
CXX = g++
CPPFLAGS = -g -O2 -I$(INC) -c

all: $(BIN)/practica2

# ************ Generación de programas ******************
$(BIN)/practica2: $(OBJ)/main.o $(OBJ)/objetivo.o $(OBJ)/individual.o $(OBJ)/genetico.o $(OBJ)/operacionesVector.o $(OBJ)/random.o
	$(CXX) $^ -o $@

# ************ Generación de modulos ******************
$(OBJ)/main.o: $(SRC)/main.cpp $(INC)/objetivo.h $(INC)/genetico.h $(INC)/individual.h $(INC)/operacionesVector.h
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/objetivo.o: $(SRC)/objetivo.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/genetico.o: $(SRC)/genetico.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/individual.o: $(SRC)/individual.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/operacionesVector.o: $(SRC)/operacionesVector.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/random.o: $(SRC)/random.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

# ************ Limpieza ************
clean :
	-rm $(OBJ)/*

mrproper : clean
	-rm $(BIN)/practica2