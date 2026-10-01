all:
	g++ -Wall -Wextra -pedantic uart.cpp -o uart

run:
	./uart

clean:
	rm uart
