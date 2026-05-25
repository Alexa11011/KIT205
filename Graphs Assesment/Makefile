CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = graph_assessment
SOURCES = main.c graph.c algorithms.c tests.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) tech-internet-as.mtx

clean:
	rm -f $(TARGET) comparison_results.csv

.PHONY: all run clean
