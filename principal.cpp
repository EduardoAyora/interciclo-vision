#include "header.h"

Mat frame;
Mat frameRecortado;

void showFrames()
{
  Mat ventana(Size(frame.cols * 3, frame.rows), frame.type());

  putText(frame, "Original", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

  frame.copyTo(ventana(Range(0, frame.rows), Range(0, frame.cols)));
  frameRecortado.copyTo(ventana(Range(0, frame.rows), Range(frame.cols, frame.cols * 2)));
  imshow("Video", ventana);
}

void eventoTrack(int porcentaje, void *data)
{
}

int main(int argc, char *argv[])
{
  namedWindow("Video", WINDOW_AUTOSIZE);

  // createTrackbar("Ruido de sal", "Video", &porcentajeRuido, 100, eventoTrackRuidoSal, NULL);

  VideoCapture video(0);
  if (!video.isOpened())
    return 0;

  while (true)
  {
    video >> frame;
    resize(frame, frame, Size(), 0.70, 0.70);
    frameRecortado = frame.clone();

    showFrames();

    if (waitKey(23) == 27)
      break;
  }

  destroyAllWindows();
  return 0;
}