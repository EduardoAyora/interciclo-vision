#include "header.h"

// Mat frame;
// Mat frameRecortado;

// void showFrames()
// {
//   Mat ventana(Size(frame.cols * 3, frame.rows), frame.type());

//   putText(frame, "Original", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

//   frame.copyTo(ventana(Range(0, frame.rows), Range(0, frame.cols)));
//   frameRecortado.copyTo(ventana(Range(0, frame.rows), Range(frame.cols, frame.cols * 2)));
//   imshow("Video", ventana);
// }

// void eventoTrack(int porcentaje, void *data)
// {
// }

// int main(int argc, char *argv[])
// {
//   namedWindow("Video", WINDOW_AUTOSIZE);

//   // createTrackbar("Ruido de sal", "Video", &porcentajeRuido, 100, eventoTrackRuidoSal, NULL);

//   VideoCapture video(0);
//   if (!video.isOpened())
//     return 0;

//   while (true)
//   {
//     video >> frame;
//     resize(frame, frame, Size(), 0.70, 0.70);
//     frameRecortado = frame.clone();

//     showFrames();

//     if (waitKey(23) == 27)
//       break;
//   }

//   destroyAllWindows();
//   return 0;
// }

bool esPuntoVacio(Point punto)
{
  if (punto.x == 0 && punto.y == 0)
    return true;
  return false;
}

bool sonClicsPermitidos = true;
Mat img;
Mat areaSeleccionada;
Point primerPunto;
Point puntoAnterior;

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
  Mat frameRecortado = convertirEnImagenNegra(img, true);
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
    if (!pasoSegundaLinea) pasoSegundaLinea = true;
    else pasoSegundaLinea = false;
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
        Vec3b color = img.at<Vec3b>(i, j);
        frameRecortado.at<Vec3b>(i, j) = color;
      }
    }
  }
  return frameRecortado;
}

int thickness = 1;

void mouse_call(int event, int x, int y, int, void *)
{
  if (!sonClicsPermitidos)
    return;
  Point puntoActual = Point(x, y);
  if (event == EVENT_LBUTTONDOWN)
  {
    if (puntoAnterior.x != 0 && puntoAnterior.y != 0)
    {
      line(img, puntoActual, puntoAnterior, Scalar(0, 0, 255), thickness, LINE_AA);
      line(areaSeleccionada, puntoActual, puntoAnterior, 255, thickness, LINE_AA);
    }
    else
    {
      primerPunto = puntoActual;
    }
    circle(img, puntoActual, 0, Scalar(0, 0, 255), 1, 8, 0);
    puntoAnterior = puntoActual;
  }

  if (event == EVENT_RBUTTONDOWN)
  {
    line(img, primerPunto, puntoAnterior, Scalar(0, 0, 255), thickness, LINE_AA);
    line(areaSeleccionada, primerPunto, puntoAnterior, 255, thickness, LINE_AA);
    namedWindow("Recortada");
    imshow("Recortada", obtenerImagenRecortada());
    sonClicsPermitidos = false;
  }
  imshow("Original", img);
  imshow("Area seleccionada", areaSeleccionada);
}

int main()
{
  img = imread("../detector_movimiento/mario.webp");
  cvtColor(img, areaSeleccionada, COLOR_BGR2GRAY);

  // convertir area seleccionada en imagen negra
  areaSeleccionada = convertirEnImagenNegra(areaSeleccionada, false);

  namedWindow("Original");
  namedWindow("Area seleccionada");
  imshow("Original", img);
  imshow("Area seleccionada", areaSeleccionada);

  setMouseCallback("Original", mouse_call);

  while (char(waitKey(1) != 'q'))
  {
  }
  return 0;
}