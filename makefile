CXX=//home/data/zizhuo/anaconda3/bin/x86_64-conda-linux-gnu-g++ -std=c++11
OPT=-O3

INC=-I/home/data/zizhuo/anaconda3/include

1st_Work: main.o data_cleaning.o data_preparation.o deletion_operation.o eta_update.o simulation.o update.o -lpthread -lboost_system -lboost_thread /home/data/zizhuo/anaconda3/lib/libstdc++.so.6
	$(CXX) -g -o 1st_Work main.o data_cleaning.o data_preparation.o deletion_operation.o eta_update.o simulation.o update.o -lpthread /home/data/zizhuo/anaconda3/lib/libboost_system.so /home/data/zizhuo/anaconda3/lib/libboost_thread.so

main.o:main.cpp
	$(CXX) -g -c $(OPT) $(INC) main.cpp
data_cleaning.o: data_cleaning.cpp
	$(CXX) -g -c $(OPT) $(INC) data_cleaning.cpp
data_preparation.o: data_preparation.cpp
	$(CXX) -g -c $(OPT) $(INC) data_preparation.cpp
deletion_operation.o: deletion_operation.cpp
	$(CXX) -g -c $(OPT) $(INC) deletion_operation.cpp
eta_update.o: eta_update.cpp
	$(CXX) -g -c $(OPT) $(INC) eta_update.cpp
simulation.o: simulation.cpp
	$(CXX) -g -c $(OPT) $(INC) simulation.cpp
update.o: update.cpp
	$(CXX) -g -c $(OPT) $(INC) update.cpp

clean:
	rm *.o
	rm 1st_Work
