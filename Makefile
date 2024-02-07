build:
	g++ -pthread util/*.cpp  psrs.cpp -o psrs -O3
	g++ util/Comparator.cpp util/Tester.cpp qs.cpp -o qs -O3

