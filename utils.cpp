#include "header.h"

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

std::tuple<Mat, Mat> obtenerImagenRecortada()
{
  Mat frameRecortado = convertirEnImagenNegra(frame, true);
  Mat frameRecortadoRelleno = frameRecortado.clone();

  // Seguimiento con tracker
  Mat areaSeleccionadaTmp;
  float traslacionDeRoiX = roi.x - pixelMasALaIzquierda;
  float traslacionDeRoiY = roi.y - pixelMasArriba;
  float warp_values[] = {1.0, 0.0, traslacionDeRoiX, 0.0, 1.0, traslacionDeRoiY};
  Mat translation_matrix = Mat(2, 3, CV_32F, warp_values);
  warpAffine(areaSeleccionada, areaSeleccionadaTmp, translation_matrix, areaSeleccionada.size());

  for (int i = 0; i < areaSeleccionadaTmp.rows; i++)
  {
    // Verificar si existen dos lineas
    bool pasoPrimeraLinea = false;
    bool pasoSegundaLinea = false;
    bool esDentroDeFigura = false;
    for (int j = 0; j < areaSeleccionadaTmp.cols; j++)
    {
      if ((int)areaSeleccionadaTmp.at<uchar>(i, j) > 0)
        pasoPrimeraLinea = true;
      if ((int)areaSeleccionadaTmp.at<uchar>(i, j) == 0 && pasoPrimeraLinea)
        esDentroDeFigura = true;
      if ((int)areaSeleccionadaTmp.at<uchar>(i, j) > 0 && pasoPrimeraLinea && esDentroDeFigura)
        pasoSegundaLinea = true;
    }

    // Recuperar solo pixeles dentro del área
    pasoPrimeraLinea = false;
    esDentroDeFigura = false;
    if (!pasoSegundaLinea)
      pasoSegundaLinea = true;
    else
      pasoSegundaLinea = false;
    for (int j = 0; j < areaSeleccionadaTmp.cols; j++)
    {
      if ((int)areaSeleccionadaTmp.at<uchar>(i, j) > 0)
        pasoPrimeraLinea = true;
      if ((int)areaSeleccionadaTmp.at<uchar>(i, j) == 0 && pasoPrimeraLinea)
        esDentroDeFigura = true;
      if ((int)areaSeleccionadaTmp.at<uchar>(i, j) > 0 && pasoPrimeraLinea && esDentroDeFigura)
        pasoSegundaLinea = true;
      if (pasoSegundaLinea)
        esDentroDeFigura = false;
      if (esDentroDeFigura)
      {
        Vec3b color = frame.at<Vec3b>(i, j);
        frameRecortado.at<Vec3b>(i, j) = color;
        Vec3b colorBlanco;
        colorBlanco[0] = 255;
        colorBlanco[1] = 255;
        colorBlanco[2] = 255;
        frameRecortadoRelleno.at<Vec3b>(i, j) = colorBlanco;
      }
    }
  }
  Mat structuringElement = getStructuringElement(MORPH_CROSS, Size(3, 3));
  morphologyEx(frameRecortado, frameRecortado, MORPH_ERODE, structuringElement, Point(-1,-1), 1);

  return make_tuple(frameRecortado, frameRecortadoRelleno);
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
      line(areaSeleccionada, puntoActual, puntoAnterior, 255, thickness, LINE_AA);

      if (pixelMasAbajo < puntoActual.y)
        pixelMasAbajo = puntoActual.y;
      if (pixelMasArriba > puntoActual.y)
        pixelMasArriba = puntoActual.y;
      if (pixelMasALaDerecha < puntoActual.x)
        pixelMasALaDerecha = puntoActual.x;
      if (pixelMasALaIzquierda > puntoActual.x)
        pixelMasALaIzquierda = puntoActual.x;
    }
    else
    {
      primerPunto = puntoActual;

      pixelMasAbajo = puntoActual.y;
      pixelMasArriba = puntoActual.y;
      pixelMasALaDerecha = puntoActual.x;
      pixelMasALaIzquierda = puntoActual.x;
    }
    circle(frame, puntoActual, 0, Scalar(0, 0, 255), 1, 8, 0);
    puntoAnterior = puntoActual;
  }

  if (event == EVENT_RBUTTONDOWN)
  {
    line(areaSeleccionada, primerPunto, puntoAnterior, 255, thickness, LINE_AA);

    // Iniciar Tracker
    anchoDeSeleccion = pixelMasALaDerecha - pixelMasALaIzquierda;
    altoDeSeleccion = pixelMasAbajo - pixelMasArriba;
    roi = cv::Rect(pixelMasALaIzquierda, pixelMasArriba, anchoDeSeleccion, altoDeSeleccion);
    tracker = TrackerKCF::create();
    tracker->init(frameTracker, roi);

    sonClicsPermitidos = false;
  }

  //imshow("Original", frame);
  //imshow("Area seleccionada", areaSeleccionada);
}