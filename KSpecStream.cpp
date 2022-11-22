#include "KSpecStream.h"

KSpecStream::KSpecStream(int _width = 640, int _height = 480){

  width = _width;
  height = _height;

  setFixedSize(width, height);
  setAutoFillBackground(true);

  img = QImage(width, height, QImage::Format_RGB16);
  buf = QPixmap(width, height);
  buf_pix = new double[height];

  QBrush brush_semi_white(QColor(255, 255, 255, 128), Qt::Dense4Pattern);

  QPainter paint(&buf);
  paint.fillRect(0, 0, width, height, brush_semi_white);

}

KSpecStream::~KSpecStream(){
	
  delete[] buf_pix;
}

void KSpecStream::paintEvent(QPaintEvent* event) {

  buf.convertFromImage(img);


  //  printf("%d %d\n",id,pos);
  QPainter paint;
  paint.begin(this);
  paint.drawPixmap(0, 0, buf.width(), buf.height(), buf);
  paint.end();

}

void KSpecStream::slot_stream_stft(double* stft) {
  StreamSTFT(stft);
}


void KSpecStream::stft2logspec(double* src, double* des) {
  int re;
  int im;
  double tmp = 0;
  for (int i = 0; i < height * 2; i += 2) {
    re = i;
    im = i + 1;
    tmp = std::pow(src[re], 2) + std::pow(src[im], 2);
   // printf("buf[%d] (%d %d) = %lf : %lf %lf\n", i / 2, re,im, tmp,src[re],src[im]);
    des[i / 2] = 10 * std::log10(tmp);
    //printf("INFO::log_spec::buf[%d] : %lf\n",i/2,buf[i/2]);
  }
}

  void KSpecStream::jet_color(double x, int* r, int* g, int* b){
    double t_r = 0, t_g = 0, t_b = 0;

    double t1, t2;
    t1 = 0.75;
    t2 = 0.25;
    // [-1,-0.75]
    if (x < -t1) {
      t_r = 0;
      t_g = 0;
      t_b = 2.5 + 2 * x;
      // [-0.75,-0.25]
    }
    else if (x < -t2) {
      t_r = 0;
      t_g = 1.5 + 2 * x;
      t_b = 1;
      // [-0.25,0.25]
    }
    else if (x < t2) {
      t_r = 0.5 + 2 * x;
      t_g = 1;
      t_b = 0.5 - 2 * x;
      // [0.25,0.75]
    }
    else if (x < t1) {
      t_r = 1;
      t_g = 1.5 - 2 * x;
      t_b = 0;
      // [0.75,1]
    }
    else {
      t_r = 2.5 - 2 * x;
      t_g = 0;
      t_b = 0;
    }

   // printf("%lf %lf %lf %lf\n",x,t_r,t_g,t_b);

    if (t_r > 1)t_r = 1;
    if (t_g > 1)t_g = 1;
    if (t_b > 1)t_b = 1;

    if (t_r < 1)t_r = 0;
    if (t_g < 1)t_g = 0;
    if (t_b < 1)t_b = 0;


    *r = (int)(t_r * 255);
    *g = (int)(t_g * 255);
    *b = (int)(t_b * 255);
}

  void KSpecStream::StreamSTFT(double* stft) {
    stft2logspec(stft, buf_pix);
    Stream(buf_pix);
  
  }

void KSpecStream::Stream(double* buf) {
  int r, g, b;
    for (int w = 0; w < width - 1; w++) {
      for (int h = 0; h < height; h++) {
        img.setPixelColor( w,h,img.pixel(w+1, h ));
      }
    }
    for (int h = 0; h < height; h++) {
      jet_color(buf[h], &r, &g, &b);
      img.setPixelColor(width-1, h,QColor(r,g,b) );

    }
    update();
}