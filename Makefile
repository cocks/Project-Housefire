ifeq ($(shell uname), Darwin)
	P3DINCLUDEDIR := /Developer/Panda3D/include
	P3DLIBDIR := /Developer/Panda3D/lib
else
	P3DINCLUDEDIR := /usr/include/panda3d
	P3DLIBDIR := /usr/lib/panda3d
endif

COMPILEFLAGS := -fPIC -O2 -I/usr/include/python2.6 -I$(P3DINCLUDEDIR) -arch i386
OBJECTFILES := main.o application.o gameObject.o gameObjectManager.o gameObjectController.o 

ProjectHousefire: main.o application.o gameObject.o gameObjectManager.o gameObjectController.o
	g++ $(OBJECTFILES) -o ProjectHousefire -fPIC -L$(P3DLIBDIR) -lp3framework -lpanda -lpandafx -lpandaexpress -lp3dtoolconfig -lp3dtool -lp3pystub -lp3direct -arch i386
main.o: src/main.cpp
	g++ -c src/main.cpp -o main.o $(COMPILEFLAGS)
application.o: src/application.cpp
	g++ -c src/application.cpp -o application.o $(COMPILEFLAGS)
gameObject.o: src/gameObject.cpp
	g++ -c src/gameObject.cpp -o gameObject.o $(COMPILEFLAGS)
gameObjectManager.o: src/gameObjectManager.cpp
	g++ -c src/gameObjectManager.cpp -o gameObjectManager.o $(COMPILEFLAGS)
gameObjectController.o: src/gameObjectController.cpp
	g++ -c src/gameObjectController.cpp -o gameObjectController.o $(COMPILEFLAGS)
clean:
	rm $(OBJECTFILES) ProjectHousefire
