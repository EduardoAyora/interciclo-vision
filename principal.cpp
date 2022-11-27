#include "header.h"

Mat frame;
Mat areaSeleccionada;
Mat frameRecortado;
Point primerPunto;
Point puntoAnterior;

bool sonClicsPermitidos = true;
int thickness = 1;

Mat convertirEnImagenNegra(Mat imagen, bool esImagenConColores)
{
  Mat imagenTmp = imagen.clone();
  for (int i = 0; i < imagenTmp.rows; i++)
  {
    for (int j = 0; j < imagenTmp.cols; j++)
    {
      if (esImagenConColores)
      {
        Vec3b color = imagenTmp.at<Vec3b>(i, j);
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
        imagenTmp.at<Vec3b>(i, j) = color;
      }
      else
      {
        imagenTmp.at<uchar>(i, j) = 0;
      }
    }
  }
  return imagenTmp;
}

Mat obtenerImagenRecortada()
{
  Mat frameRecortado = convertirEnImagenNegra(frame, true);
  for (int i = 0; i < areaSeleccionada.rows; i++)
  {
    // Verificar si existen dos lineas
    bool pasoPrimeraLinea = false;
    bool pasoSegundaLinea = false;
    bool esDentroDeFigura = false;
    for (int j = 0; j < areaSeleccionada.cols; j++)
    {
      if ((int)areaSeleccionada.at<uchar>(i, j) > 0)
        pasoPrimeraLinea = true;
      if ((int)areaSeleccionada.at<uchar>(i, j) == 0 && pasoPrimeraLinea)
        esDentroDeFigura = true;
      if ((int)areaSeleccionada.at<uchar>(i, j) > 0 && pasoPrimeraLinea && esDentroDeFigura)
        pasoSegundaLinea = true;
    }

    // Recuperar solo pixeles dentro del área
    pasoPrimeraLinea = false;
    esDentroDeFigura = false;
    if (!pasoSegundaLinea)
      pasoSegundaLinea = true;
    else
      pasoSegundaLinea = false;
    for (int j = 0; j < areaSeleccionada.cols; j++)
    {
      if ((int)areaSeleccionada.at<uchar>(i, j) > 0)
        pasoPrimeraLinea = true;
      if ((int)areaSeleccionada.at<uchar>(i, j) == 0 && pasoPrimeraLinea)
        esDentroDeFigura = true;
      if ((int)areaSeleccionada.at<uchar>(i, j) > 0 && pasoPrimeraLinea && esDentroDeFigura)
        pasoSegundaLinea = true;
      if (pasoSegundaLinea)
        esDentroDeFigura = false;
      if (esDentroDeFigura)
      {
        Vec3b color = frame.at<Vec3b>(i, j);
        frameRecortado.at<Vec3b>(i, j) = color;
      }
    }
  }
  return frameRecortado;
}

void mouse_call(int event, int x, int y, int, void *)
{
  if (!sonClicsPermitidos)
    return;
  Point puntoActual = Point(x, y);
  if (event == EVENT_LBUTTONDOWN)
  {
    if (puntoAnterior.x != 0 && puntoAnterior.y != 0)
    {
      line(frame, puntoActual, puntoAnterior, Scalar(0, 0, 255), thickness, LINE_AA);
      line(areaSeleccionada, puntoActual, puntoAnterior, 255, thickness, LINE_AA);
    }
    else
    {
      primerPunto = puntoActual;
    }
    circle(frame, puntoActual, 0, Scalar(0, 0, 255), 1, 8, 0);
    puntoAnterior = puntoActual;
  }

  if (event == EVENT_RBUTTONDOWN)
  {
    line(frame, primerPunto, puntoAnterior, Scalar(0, 0, 255), thickness, LINE_AA);
    line(areaSeleccionada, primerPunto, puntoAnterior, 255, thickness, LINE_AA);
    sonClicsPermitidos = false;
  }
  imshow("Original", frame);
  imshow("Area seleccionada", areaSeleccionada);
}

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
    frameRecortado = frame.clone();

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
    imshow("Original", frame);
    imshow("Area seleccionada", areaSeleccionada);
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