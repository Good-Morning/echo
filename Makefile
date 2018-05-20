all:
	g++ -std=c++14 server.cpp -o server -lpthread
	g++ -std=c++14 client.cpp -o client
