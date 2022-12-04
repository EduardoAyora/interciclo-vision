#include "header.h"

Mat frame;
Mat areaSeleccionada;
Mat frameRecortado;
Mat frameTracker;
Mat captura;
Mat video1;
Mat final;
Mat recorte;
Mat lienzo;
int area = 0;
bool calculoArea = true;
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
int valor = 0;
string filename = "Videoresultante.avi";
double fps = 30.0;

void eventoTrack(int v, void *data)
{
}

int main(int argc, char *argv[])
{
  VideoCapture video(0);
  double fps2 = video.get(cv::CAP_PROP_FPS);
  VideoCapture videocargado("fondo1.mp4");
  VideoCapture videocargado2("fondo2.mp4");
  VideoCapture videocargado3("fondo3.mp4");
  if (!video.isOpened())
    return 0;

  video >> captura;

  VideoWriter writer;
  bool esVentanaAreaSeleccionadaIniciada = false;
  bool isColor = (captura.type() == CV_8UC3);
  int codec = writer.fourcc('M', 'J', 'P', 'G');
  writer.open(filename, codec, fps, captura.size(), isColor);
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

    Mat ventana(Size(frame.cols * 3, frame.rows * 2), frame.type());
    namedWindow("Video");
    putText(frame, "FPS:" + to_string(int(fps2)), cv::Point(20, 50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
    putText(frame, "AREA:" + to_string(int(area)), cv::Point(20, 80), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 255), 2, false);
    frame.copyTo(ventana(Range(0, frame.rows), Range(0, frame.cols)));
    areaSeleccionadaColor.copyTo(ventana(Range(0, frame.rows), Range(frame.cols, frame.cols * 2)));
    frameRecortado.copyTo(ventana(Range(0, frame.rows), Range(frame.cols * 2, frame.cols * 3)));

    createTrackbar("Ecualizador", "Video", &valor, 3, eventoTrack, NULL);
    if (valor == 1)
    {

      videocargado >> video1;
      resize(video1, video1, frame.size());
      video1.copyTo(ventana(Range(frame.rows, frame.rows * 2), Range(frame.cols, frame.cols * 2)));
    }
    if (valor == 2)
    {

      videocargado2 >> video1;
      resize(video1, video1, frame.size());
      video1.copyTo(ventana(Range(frame.rows, frame.rows * 2), Range(frame.cols, frame.cols * 2)));
    }
    if (valor == 3)
    {

      videocargado3 >> video1;
      resize(video1, video1, frame.size());
      video1.copyTo(ventana(Range(frame.rows, frame.rows * 2), Range(frame.cols, frame.cols * 2)));
    }
    imshow("Video", ventana);

    if (!sonClicsPermitidos)
    {

      recorte = obtenerImagenRecortada();

      lienzo = obtenerImagenRecortadaRellena();
      recorte.copyTo(ventana(Range(frame.rows, frame.rows * 2), Range(0, frame.cols)));

      // Calculo del area y tmb operacion de bitwise para combinar imagenes
      for (int i = 0; i < lienzo.rows; i++)
      {
        for (int j = 0; j < lienzo.cols; j++)
        {
          Vec3b pixel = lienzo.at<Vec3b>(i, j);
          if (pixel[0] > 0 && pixel[1] > 0 && pixel[2] > 0)
          {
            if (calculoArea == true)
            {
              area += 1;
            }
          }
        }
      }
      calculoArea = false;
      resize(video1, video1, recorte.size());
      // imshow("ed", recorteBlanco);
      bitwise_not(lienzo, lienzo);
      bitwise_and(video1, lienzo, final);
      add(final, recorte, final);
      final.copyTo(ventana(Range(frame.rows, frame.rows * 2), Range(frame.cols * 2, frame.cols * 3)));

      writer.write(final);
    }

    setMouseCallback("Video", mouse_call);

    if (waitKey(23) == 27)
      break;
  }
  video.release();
  destroyAllWindows();
  return 0;
}