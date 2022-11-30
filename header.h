#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <sstream>
#include <random>
#include <chrono>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/tracking.hpp>

using namespace std;
using namespace std::chrono;
using namespace cv;

#ifndef HEADER
#define HEADER

extern Mat frame;
extern Mat areaSeleccionada;
extern Mat frameRecortado;
extern Mat frameTracker;
extern Point primerPunto;
extern Point puntoAnterior;
extern int pixelMasALaDerecha;
extern int pixelMasALaIzquierda;
extern int pixelMasArriba;
extern int pixelMasAbajo;
extern int anchoDeSeleccion;
extern int altoDeSeleccion;
extern Rect roi;
extern Ptr<Tracker> tracker;
extern bool sonClicsPermitidos;
extern int thickness;

Mat convertirEnImagenNegra(Mat imagen, bool esImagenConColores);
Mat obtenerImagenRecortada();
void mouse_call(int event, int x, int y, int, void *);

#endif