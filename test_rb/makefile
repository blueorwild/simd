CC=g++
CFLAGS= -O2 #-g
TARGET=test_rb1 test_rb2 test_rb3

default: $(TARGET)

test_rb1: test_rb.cpp
	$(CC) -o $@ $(CFLAGS) -DRB1 $^

test_rb2: test_rb.cpp
	$(CC) -o $@ $(CFLAGS) -DRB2 $^

test_rb3: test_rb.cpp
	$(CC) -o $@ $(CFLAGS) -DRB3 $^

clean:
	@rm -f $(TARGET)