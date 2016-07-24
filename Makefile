TARGET=libQSlidingWindow.so
INCLUDE+=../QSlidingWindowConsume
CC=g++

$(TARGET):QSlidingWindow.o
	$(CC) -shared -fPIC  -o $(TARGET) QSlidingWindow.o -L/usr/local/lib -lQSlidingWindowConsume

QSlidingWindow.o:QSlidingWindow.cpp QSlidingWindow.h
	$(CC) -I$(INCLUDE) -c QSlidingWindow.cpp QSlidingWindow.h

install:
	cp -f $(TARGET) /usr/local/lib

clean:
	rm -f *.o* *.so*
