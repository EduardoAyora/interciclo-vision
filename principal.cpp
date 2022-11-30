#include "header.h"

Mat frame;
Mat areaSeleccionada;
Mat frameRecortado;
Mat frameTracker;
Point primerPunto;
Point puntoAnterior;
int pixelMasALaDerecha;
int pixelMasALaIzquierda;
int pixelMasArriba;
int pixelMasAbajo;
int anchoDeSeleccion;
int altoDeSeleccion;
Rect roi;
Ptr<Tracker> tracker;
bool sonClicsPermitidos = true;
int thickness = 1;

// void showFrames()
// {
//   Mat ventana(Size(frame.cols * 3, frame.rows), frame.type());

//   putText(frame, "Original", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

//   Mat areaSeleccionadaColor;
//   cvtColor(areaSeleccionada, areaSeleccionadaColor, COLOR_GRAY2BGR);

//   frame.copyTo(ventana(Range(0, frame.rows), Range(0, frame.cols)));
//   areaSeleccionadaColor.copyTo(ventana(Range(0, frame.rows), Range(frame.cols, frame.cols * 2)));
//   frameRecortado.copyTo(ventana(Range(0, frame.rows), Range(frame.cols * 2, frame.cols * 3)));
//   imshow("Video", ventana);
// }

int main(int argc, char *argv[])
{
  VideoCapture video(0);
  if (!video.isOpened())
    return 0;

  bool esVentanaAreaSeleccionadaIniciada = false;
  while (true)
  {
    video >> frame;
    resize(frame, frame, Size(), 0.70, 0.70);
    flip(frame, frame, 1);
    frameRecortado = frame.clone();
    frameTracker = frame.clone();

    // Tracker
    if (tracker != NULL)
    {
      tracker->update(frameTracker, roi);
      rectangle(frameTracker, roi, Scalar(255, 0, 0), 2, 1);
    }

    if (!esVentanaAreaSeleccionadaIniciada)
    {
      cvtColor(frame, areaSeleccionada, COLOR_BGR2GRAY);
      // convertir area seleccionada en imagen negra
      areaSeleccionada = convertirEnImagenNegra(areaSeleccionada, false);
      esVentanaAreaSeleccionadaIniciada = true;
    }
    Mat areaSeleccionadaColor;
    cvtColor(areaSeleccionada, areaSeleccionadaColor, COLOR_GRAY2BGR);
    bitwise_or(areaSeleccionadaColor, frame, frame);

    namedWindow("Original");
    namedWindow("Area seleccionada");
    namedWindow("Tracker");
    imshow("Original", frame);
    imshow("Area seleccionada", areaSeleccionada);
    imshow("Tracker", frameTracker);
    if (!sonClicsPermitidos)
    {
      namedWindow("Recortada");
      imshow("Recortada", obtenerImagenRecortada());
    }

    setMouseCallback("Original", mouse_call);

    if (waitKey(23) == 27)
      break;
  }

  destroyAllWindows();
  return 0;
}