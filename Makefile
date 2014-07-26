OBJS = StatsProgram.o ChannelCollection.o ChannelCollectionWriter.o FileParser.o Config.o Time.o LineParser.o Tokeniser.o UserCollection.o UserCollectionWriter.o User.o UserPairCharacteristic.o WordCategories.o
LFLAGS = -Wall
CFLAGS = -Wall -c -O3
CC = g++
COMPILE = $(CC) $(CFLAGS)

ircst: $(OBJS)
	$(CC) -o ircst $(LFLAGS) $(OBJS)

StatsProgram.o: StatsProgram.h StatsProgram.cpp FileParser.h ChannelCollection.h UserCollection.h Comparators.h UserComparators.h Definitions.h User.h Tokeniser.h WordCategories.h
	$(COMPILE) StatsProgram.cpp

ChannelCollection.o: ChannelCollection.h ChannelCollection.cpp UserCollection.h Comparators.h UserComparators.h Definitions.h User.h Tokeniser.h WordCategories.h
	$(COMPILE) ChannelCollection.cpp

ChannelCollectionWriter.o: ChannelCollection.h ChannelCollectionWriter.cpp UserCollection.h Comparators.h UserComparators.h Definitions.h User.h Tokeniser.h WordCategories.h
	$(COMPILE) ChannelCollectionWriter.cpp

Comparators.h:

UserComparators.h: User.h Definitions.h Tokeniser.h WordCategories.h

UserPairComparators.h: UserPairCharacteristic.h User.h Tokeniser.h WordCategories.h

Definitions.h:

Config.o: Config.h Config.cpp
	$(COMPILE) Config.cpp

FileParser.o: FileParser.h FileParser.cpp LineParser.h ChannelCollection.h UserCollection.h Comparators.h UserComparators.h Definitions.h User.h Tokeniser.h WordCategories.h
	$(COMPILE) FileParser.cpp

Time.o: Time.h Time.cpp
	$(COMPILE) Time.cpp

LineParser.o: LineParser.h LineParser.cpp Time.h Definitions.h
	$(COMPILE) LineParser.cpp

Tokeniser.o: Tokeniser.h Tokeniser.cpp
	$(COMPILE) Tokeniser.cpp

UserCollection.o: UserCollection.h UserCollection.cpp Comparators.h UserComparators.h UserPairComparators.h Definitions.h User.h Tokeniser.h WordCategories.h
	$(COMPILE) UserCollection.cpp

UserCollectionWriter.o: UserCollection.h UserCollectionWriter.cpp Comparators.h UserComparators.h UserPairComparators.h Definitions.h User.h Tokeniser.h WordCategories.h
	$(COMPILE) UserCollectionWriter.cpp

User.o: User.h User.cpp Definitions.h Tokeniser.h WordCategories.h Comparators.h
	$(COMPILE) User.cpp

UserPairCharacteristic.o: UserPairCharacteristic.h UserPairCharacteristic.cpp User.h Definitions.h Tokeniser.h WordCategories.h Comparators.h
	$(COMPILE) UserPairCharacteristic.cpp

WordCategories.o: WordCategories.h WordCategories.cpp
	$(COMPILE) WordCategories.cpp

clean:
	rm *.o ircst

