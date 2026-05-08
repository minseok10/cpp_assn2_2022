CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
TARGET := coup_game
SOURCES := main.cpp Card.cpp Deck.cpp Player.cpp Prtstr.cpp Run.cpp randoms.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
