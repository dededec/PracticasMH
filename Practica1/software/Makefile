SRC = FUENTES/src
INC = FUENTES/include
OBJ = FUENTES/obj
BIN = BIN
CXX = g++
CPPFLAGS = -g -O3  -I$(INC) -c

all: $(BIN)/practica1

# ************ Generación de programas ******************
$(BIN)/practica1: $(OBJ)/practica1.o $(OBJ)/random.o $(OBJ)/objetivo.o $(OBJ)/busquedaLocal.o $(OBJ)/greedy.o $(OBJ)/operacionesVector.o
	$(CXX) $^ -o $@

# ************ Generación de modulos ******************
$(OBJ)/practica1.o: $(SRC)/practica1.cpp $(INC)/random.h $(INC)/objetivo.h $(INC)/busquedaLocal.h $(INC)/greedy.h
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/random.o: $(SRC)/random.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/objetivo.o: $(SRC)/objetivo.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/busquedaLocal.o: $(SRC)/busquedaLocal.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/greedy.o: $(SRC)/greedy.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(OBJ)/operacionesVector.o: $(SRC)/operacionesVector.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

# ************ Limpieza ************
clean :
	-rm $(OBJ)/*

mrproper : clean
	-rm $(BIN)/practica1 