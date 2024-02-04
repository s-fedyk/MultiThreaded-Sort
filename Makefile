build:
	g++ -pthread util/*.cpp  psrs.cpp -g -o psrs -O3
	g++ util/Comparator.cpp util/Tester.cpp qs.cpp -g -o qs -O3

