build:
	g++ -pthread tester.cpp barrier.cpp BinarySearch.cpp main.cpp -g -o main
	g++ tester.cpp qs.cpp -g -o qs

