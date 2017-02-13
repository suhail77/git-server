MYSQL_FLAGS := $(shell mysql_config --cflags)
MYSQL_LIBS := $(shell mysql_config --libs)
OBJSREAD = db.o readserver.o
OBJSWRITE = db.o writeserver.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)

executables : $(OBJSREAD) $(OBJSWRITE)
	$(CC) $(MYSQL_CFLAGS) $(MYSQL_LIBS) $(OBJSREAD) -o readefeedback
	$(CC) $(MYSQL_CFLAGS) $(MYSQL_LIBS) $(OBJSWRITE) -o writeefeedback

writeefeedback : $(OBJSWRITE)
	$(CC) $(MYSQL_CFLAGS) $(MYSQL_LIBS) $(OBJSWRITE) -o writeefeedback

db.o : db.cpp db.h
	$(CC) $(CFLAGS) db.cpp

readserver.o : readserver.cpp readserver.h db.h
	$(CC) $(CFLAGS) readserver.cpp

writeserver.o : writeserver.cpp writeserver.h db.h
	$(CC) $(CFLAGS) writeserver.cpp

clean:
	\rm *.o readefeedback writeefeedback

runread:
	./readefeedback

runwrite:
	./writeefeedback

run:
	./readefeedback
	./writeefeedback

kill:
	sudo killall readefeedback
	sudo killall writeefeedback
	\rm *.txt

#db.o: db.cpp
#	g++ -Wall -c db.cpp  $(MYSQL_FLAGS) $(MYSQL_LIBS)
#g++ -o db db.cpp $(MYSQL_FLAGS) $(MYSQL_LIBS)
#g++ -o db db.cpp 'mysql_config --cfgags' 'mysql_config --libs'
