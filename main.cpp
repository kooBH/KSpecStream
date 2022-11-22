
#include <QApplication>
#include <QCoreApplication>

#include "KSpecStream.h"

#include <thread>

#include <random>
#include <windows.h>

void test_stream(KSpecStream* widget) {
  std::random_device rd;
  std::mt19937 gen(rd());
  //std::normal_distribution<> dis(1, 0.5);
  std::uniform_real_distribution<> dis(-1, 1);
   
  int width = 640;
  int height = 480;

  double* stft = new double[height * 2];

  for (int i = 0; i < 3000; i++) {
  
    for (int j = 0; j < height*2; j++) {

      stft[j] = dis(gen);
    }
  
    widget->StreamSTFT(stft);

    Sleep(8);
   }

  delete[] stft;

}

int main(int argc, char* argv[]) {

  QApplication app(argc, argv);
  QCoreApplication::addLibraryPath("./lib");

  KSpecStream widget(640,480);

  widget.show();

  std::thread thread_process(test_stream, &widget);
  thread_process.detach();


  return app.exec();
}