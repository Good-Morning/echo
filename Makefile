all:
	g++ -std=c++14 utils.cpp server.cpp -o server -lpthread
	g++ -std=c++14 utils.cpp client.cpp -o client
