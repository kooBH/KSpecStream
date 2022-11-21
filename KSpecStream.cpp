#include "KSpecStream.h"

KSpecStream::KSpecStream(){

  setFixedSize(width, height);
  setAutoFillBackground(true);

  img = QImage(width, height, QImage::Format_RGB16);
  buf = QPixmap(width, height);

  QBrush brush_semi_white(QColor(255, 255, 255, 128), Qt::Dense4Pattern);

  QPainter paint(&buf);
  paint.fillRect(0, 0, width, height, brush_semi_white);

  thread_process = new std::thread([=] {this->Stream(); });
  thread_process->detach();

}

KSpecStream::~KSpecStream(){
	
	
}

void KSpecStream::paintEvent(QPaintEvent* event) {

  buf.convertFromImage(img);


  //  printf("%d %d\n",id,pos);
  QPainter paint;
  paint.begin(this);
  paint.drawPixmap(0, 0, buf.width(), buf.height(), buf);
  paint.end();

}

void KSpecStream::Stream() {
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(0, 255);


  while(true){
    for (int w = 0; w < width - 1; w++) {
      for (int h = 0; h < height; h++) {
        img.setPixelColor( w,h,img.pixel(w+1, h ));
      }
    }
    for (int h = 0; h < height; h++) {
       
      int r = dis(gen);
      int g = dis(gen);
      int b = dis(gen);
      img.setPixelColor(width-1, h,QColor(r,g,b) );

    }
    Sleep(8);
    update();
  }

}