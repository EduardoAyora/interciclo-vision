#include "header.h"

Mat ponerRuido(Mat imagen, int porcentajeRuido, bool esRuidoSal)
{
  int total = (imagen.rows * imagen.cols) * porcentajeRuido / 100;
  int cont = 0;
  int i = 0, j = 0;

  while (cont < total)
  {
    i = rand() % imagen.rows;
    j = rand() % imagen.cols;
    Vec3b color = imagen.at<Vec3b>(i, j);
    if (esRuidoSal)
    {
      color[0] = 255;
      color[1] = 255;
      color[2] = 255;
    }
    else
    {
      color[0] = 0;
      color[1] = 0;
      color[2] = 0;
    }
    imagen.at<Vec3b>(i, j) = color;
    cont++;
  }
  return imagen;
}