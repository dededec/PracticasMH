SRC = ./FUENTES/src
INC = ./FUENTES/include
OBJ = ./FUENTES/obj
BIN = ./BIN
CXX = g++
CPPFLAGS = -g -O2 -I$(INC) -c

all: $(BIN)/practica3

# ************ Generación de programas ******************
$(BIN)/practica3: $(OBJ)/main.o $(OBJ)/objetivo.o $(OBJ)/individual.o $(OBJ)/trayectorias.o $(OBJ)/operacionesVector.o $(OBJ)/random.o $(OBJ)/busquedaLocal.o
	$(CXX) $^ -o $@

# ************ Generación de modulos ******************
$(OBJ)/main.o: $(SRC)/main.cpp $(INC)/objetivo.h $(INC)/individual.h $(INC)/trayectorias.h $(INC)/operacionesVector.h
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/objetivo.o: $(SRC)/objetivo.cpp $(INC)/operacionesVector.h
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/trayectorias.o: $(SRC)/trayectorias.cpp $(INC)/individual.h $(INC)/operacionesVector.h
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/individual.o: $(SRC)/individual.cpp $(INC)/objetivo.h $(INC)/operacionesVector.h
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/operacionesVector.o: $(SRC)/operacionesVector.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/random.o: $(SRC)/random.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/busquedaLocal.o: $(SRC)/busquedaLocal.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

# ************ Limpieza ************
clean :
	-rm $(OBJ)/*

mrproper : clean
	-rm $(BIN)/practica3