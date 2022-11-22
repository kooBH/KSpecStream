#include "KSpecStream.h"

KSpecStream::KSpecStream(
  int _width = 640
  ,int _height = 256,
  int _n_hfft=256){

  width = _width;
  height = _height;
  n_hfft = _n_hfft;

  printf("KSpecStream : w %d h %d n %d\n", width, height, n_hfft);

  setFixedSize(width, height);
  setAutoFillBackground(true);

  img = QImage(width, height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(width, height);
  buf_pix = new double[n_hfft];

  QBrush brush_semi_white(QColor(255, 255, 255, 128), Qt::Dense4Pattern);

  QPainter paint(&pixmap_buf);
  paint.fillRect(0, 0, width, height, brush_semi_white);

}

KSpecStream::~KSpecStream(){
	
  delete[] buf_pix;
}

void KSpecStream::paintEvent(QPaintEvent* event) {

  pixmap_buf.convertFromImage(img);

  QPainter paint;
  paint.begin(this);
  paint.drawPixmap(0, 0, pixmap_buf.width(), pixmap_buf.height(), pixmap_buf);
  paint.end();

}

void KSpecStream::slot_stream_stft(double* stft) {
  StreamSTFT(stft);
}


void KSpecStream::stft2logspec(double* src, double* des) {
  int re;
  int im;
  double tmp = 0;
  for (int i = 0; i < n_hfft * 2; i += 2) {
    re = i;
    im = i + 1;
    tmp = std::pow(src[re], 2) + std::pow(src[im], 2);
    //printf("buf[%d] (%d %d) = %lf : %lf %lf\n", i / 2, re,im, tmp,src[re],src[im]);
    des[i / 2] = 10 * std::log10(tmp);
    //printf("INFO::log_spec::buf[%d] : %lf\n",i/2,buf[i/2]);
  }
}

  void KSpecStream::jet_color(double x, int* r, int* g, int* b){
    double t_r = 0, t_g = 0, t_b = 0;
    x /= 100;
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

  // printf("Color %lf %lf %lf %lf\n",x,t_r,t_g,t_b);

    *r = (int)(t_r * 255);
    *g = (int)(t_g * 255);
    *b = (int)(t_b * 255);

    if (*r < 0)*r = 0;
    if (*g < 0)*g = 0;
    if (*b < 0)*b = 0;

    if (*r > 255)*r = 255;
    if (*g > 255)*g = 255;
    if (*b > 255)*b = 255;

}

  void KSpecStream::StreamSTFT(double* stft) {
    //printf("KSpecStream::StreamSTFT\n");
    stft2logspec(stft, buf_pix);
    Stream(buf_pix);
  
  }

void KSpecStream::Stream(double* buf) {
  int r, g, b;
  int prev = 0;
  int idx = 0;
  int cnt = 0;
  double val = 0;
  /*
  for (int w = 0; w < width - 1; w++) {
    for (int h = 0; h < height; h++) {
      img.setPixelColor( w,h,img.pixel(w+1, h ));
    }
  }
  */
  QRegion exposed;
  pixmap_buf.scroll(-1, 0, pixmap_buf.rect(), &exposed);
  img = pixmap_buf.toImage();
  
  for (int i = 0; i < n_hfft; i++) {
    idx = int(i * (height / (double)n_hfft));
    // update
    if (idx != prev) {
      val /= cnt;
      jet_color(val, &r, &g, &b);
      img.setPixelColor(width-1, height-prev-1,QColor(r,g,b) );
     // printf("i %d idx %d prev %d\n",i,idx,prev);
      //printf("%d %d %d\n",r,g,b);

      cnt = 0;
      val = 0;
      prev = idx;
    }
    val += buf[i];
    cnt++;

  }
  cnt_update++;

  if (cnt_update == interval_update) {
    update();
    cnt_update = 0;
  }
}