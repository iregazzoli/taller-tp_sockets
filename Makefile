CXX = g++
CXXFLAGS = -std=c++17 -ggdb -O0 -pedantic -Wall -D _POSIX_C_SOURCE=200809L

# Ejecutables
all: client server

client: client_main.o Socket.o Client.o Server.o Game.o
	$(CXX) $(CXXFLAGS) client_main.o Socket.o Client.o Server.o Game.o -o client

server: server_main.o Socket.o Client.o Server.o Game.o
	$(CXX) $(CXXFLAGS) server_main.o Socket.o Client.o Server.o Game.o -o server

# Reglas para objetos
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

client_main.o: client_main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

server_main.o: server_main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f *.o client server
