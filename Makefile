all:
	g++ -std=c++11 -o hola.sh principal.cpp utils.cpp -I /usr/local/Cellar/opencv/4.6.0_1/include/opencv4/ -L /usr/local/Cellar/opencv/4.6.0_1/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_video -lopencv_videoio
	# g++ -o hola.sh principal.cpp `pkg-config --cflags --libs opencv`
run:
	./hola.sh