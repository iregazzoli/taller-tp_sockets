CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

# Objetos
OBJ = Socket.o Client.o Server.o Game.o
HEADERS = Socket.h Client.h Server.h Game.h

# Ejecutables
all: client server

client: client_main.o $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

server: server_main.o $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Reglas para objetos
Socket.o: Socket.cpp Socket.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

Client.o: Client.cpp Client.h Socket.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

Server.o: Server.cpp Server.h Socket.h Game.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

Game.o: Game.cpp Game.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

client_main.o: client_main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

server_main.o: server_main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f *.o client server
