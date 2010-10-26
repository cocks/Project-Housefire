ifeq ($(shell uname), Darwin)
	P3DINCLUDEDIR := /Developer/Panda3D/include
	P3DLIBDIR := /Developer/Panda3D/lib
else
	P3DINCLUDEDIR := /usr/include/panda3d
	P3DLIBDIR := /usr/lib/panda3d
endif

BINDIR := bin

COMPILEFLAGS := -Wall -fPIC -O2 -I/usr/include/python2.6 -I$(P3DINCLUDEDIR)
OBJECTFILES := $(BINDIR)/main.o $(BINDIR)/application.o $(BINDIR)/gameObject.o $(BINDIR)/gameObjectManager.o $(BINDIR)/gameObjectController.o

ProjectHousefire: $(OBJECTFILES)
	g++ $(OBJECTFILES) -o $(BINDIR)/ProjectHousefire -fPIC -L$(P3DLIBDIR) -lp3framework -lpanda -lpandafx -lpandaexpress -lp3dtoolconfig -lp3dtool -lp3pystub -lp3direct
$(BINDIR)/main.o: src/main.cpp
	mkdir -p $(BINDIR)
	g++ -c src/main.cpp -o $(BINDIR)/main.o $(COMPILEFLAGS)
$(BINDIR)/application.o: src/application.cpp
	g++ -c src/application.cpp -o $(BINDIR)/application.o $(COMPILEFLAGS)
$(BINDIR)/gameObject.o: src/gameObject.cpp
	g++ -c src/gameObject.cpp -o $(BINDIR)/gameObject.o $(COMPILEFLAGS)
$(BINDIR)/gameObjectManager.o: src/gameObjectManager.cpp
	g++ -c src/gameObjectManager.cpp -o $(BINDIR)/gameObjectManager.o $(COMPILEFLAGS)
$(BINDIR)/gameObjectController.o: src/gameObjectController.cpp
	g++ -c src/gameObjectController.cpp -o $(BINDIR)/gameObjectController.o $(COMPILEFLAGS)
clean:
	rmdir $(BINDIR)
	rm $(OBJECTFILES) $(BINDIR)/ProjectHousefire 
