build:
	g++ Main.cpp `pkg-config gtkmm-3.0 --cflags --libs` -o editor -O2
run:
	./editor
clean:
	rm editor
