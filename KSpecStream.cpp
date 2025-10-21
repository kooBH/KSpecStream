#include "KSpecStream.h"

KSpecStream::KSpecStream(
  int _width = 640
  ,int _height = 256,
  int _n_fft=256){

  m_width = _width;
  m_height = _height;
  n_fft = _n_fft;
  n_hfft = n_fft / 2 + 1;

 // printf("KSpecStream : w %d h %d n %d\n", width, height, n_fft);

  //setFixedSize(width, height);
  //setAutoFillBackground(true);

  buf_pix = new double[n_hfft];
  buf_stft = new double[n_fft+2];
  stft = new STFT(1,n_fft,n_fft/4);

#ifdef green_theme
  img = QImage(m_width, m_height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(m_width, m_height);
#else
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#endif

  refresh();
}

KSpecStream::~KSpecStream(){
	
  delete[] buf_pix;
  delete[] buf_stft;
}


#ifdef green_theme
void KSpecStream::paintEvent(QPaintEvent* event) {

  pixmap_buf.convertFromImage(img);

  QPainter paint;
  paint.begin(this);
  paint.drawPixmap(0, 0, pixmap_buf.width(), pixmap_buf.height(), pixmap_buf);
  paint.end();

}
#else
void KSpecStream::paintEvent(QPaintEvent* event) {

  // Ensure the backing pixmap fits the widget; if not, reallocate now.
  if (pixmap_buf.width()  != width() ||
      pixmap_buf.height() != height()) {
    refresh();
  }

  // Draw the pixmap scaled to the widget rect (usually 1:1 if refresh() ran)
  QPainter p(this);
  p.drawPixmap(rect(), pixmap_buf, pixmap_buf.rect());

  // pixmap_buf.convertFromImage(img);

  // QPainter paint;
  // paint.begin(this);
  // paint.fillRect(rect(), color_bg_);
  // paint.drawPixmap(rect(), pixmap_buf, pixmap_buf.rect());
  // paint.end();

}
#endif

void KSpecStream::slot_stream_stft(double* stft) {
  StreamSTFT(stft);
}

#ifdef green_theme
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
#else
void KSpecStream::stft2logspec(double* src, double* des) {
  const double eps = 1e-12;            
  for (int i = 0; i < n_hfft * 2; i += 2) {
    double re = src[i + 0];
    double im = src[i + 1];
    double pwr = re*re + im*im;
    //printf("buf[%d] (%d %d) = %lf : %lf %lf\n", i / 2, re,im, pwr,src[re],src[im]);
    des[i >> 1] = 10.0 * std::log10(pwr + eps);
    //printf("INFO::log_spec::buf[%d] : %lf\n",i/2,buf[i/2]);
  }
}
#endif

  void KSpecStream::inferno_color(double x, int*r, int*g, int*b){


    if(x > color_max) x = color_max;
    if(x < color_min) x = color_min;
    float normalizedValue = (x - color_min) / static_cast<float>(color_max - color_min);

    int idx_color = (int)(normalizedValue * 1023);
  
    *r = inferno_1024[idx_color][0];
    *g = inferno_1024[idx_color][1];
    *b = inferno_1024[idx_color][2];
  }

#ifdef green_theme
  void KSpecStream::jet_color(double x, int* r, int* g, int* b) {


    if (x > color_max) x = color_max;
    if (x < color_min) x = color_min;
    float normalizedValue = (x - color_min) / static_cast<float>(color_max - color_min);

    int idx_color = (int)(normalizedValue * 1023);

    *r = static_cast<int>(jet_1024[idx_color][0])*255;
    *g = static_cast<int>(jet_1024[idx_color][1]*255);
    *b = static_cast<int>(jet_1024[idx_color][2]*255);
  }
#else
  void KSpecStream::jet_color(double x, int* r, int* g, int* b) {
    if (x > color_max) x = color_max;
    if (x < color_min) x = color_min;
    float normalizedValue = (x - color_min) / float(color_max - color_min);
    int idx_color = int(normalizedValue * 1023);
    
    *r = int(jet_1024[idx_color][0] * 255.0f);
    *g = int(jet_1024[idx_color][1] * 255.0f);
    *b = int(jet_1024[idx_color][2] * 255.0f);
  }
#endif

  void KSpecStream::StreamSTFT(double* stft) {
    //printf("KSpecStream::StreamSTFT\n");
    stft2logspec(stft, buf_pix);
    Stream(buf_pix);
  
  }

#ifdef green_theme
void KSpecStream::Stream(double* buf) {
  int r, g, b;
  int prev = 0;
  int idx = 0;
  int cnt = 0;
  double val = 0;

  QRegion exposed;
  pixmap_buf.scroll(-1, 0, pixmap_buf.rect(), &exposed);
  img = pixmap_buf.toImage();
  
  for (int i = 0; i < n_hfft; i++) {
    idx = int(i * (m_height / (double)n_hfft));
    // update
    if (idx != prev) {
      val /= cnt;
      switch (type_colormap) {
      case 0 :
        jet_color(val, &r, &g, &b);
        break;
      case 1:
        inferno_color(val, &r, &g, &b);
        break;
      }

      for (int j = prev; j < idx; j++) {
        img.setPixelColor(m_width-1, m_height-j-1,QColor(r,g,b) );
      }
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
#else // dark theme
void KSpecStream::Stream(double* buf) {
  if (pixmap_buf.width()  != width() ||
      pixmap_buf.height() != height()) {
    refresh();
  }

  int r = 0, g = 0, b = 0;
  int prev = 0, idx = 0, cnt = 0;
  double val = 0.0;

  // 1) Scroll on QPixmap and clear the exposed region
  QRegion exposed;
  pixmap_buf.scroll(-1, 0, pixmap_buf.rect(), &exposed);
  {
    QPainter p(&pixmap_buf);
    for (const QRect& rc : exposed) {
      p.fillRect(rc, color_bg_);
    }
  }

  // 2) Work on QImage view of the pixmap
  img = pixmap_buf.toImage();
  const int w = img.width();
  const int h = img.height();
  if (w <= 0 || h <= 0 || n_hfft <= 0) return;

  const double scale = double(h) / double(n_hfft);

  // 3) Paint the new rightmost column (x = w - 1)
  for (int i = 0; i < n_hfft; ++i) {
    idx = int(i * scale);
    if (idx >= h) idx = h - 1;

    if (idx != prev) {
      if (cnt > 0) val /= cnt; else val = 0.0;

      switch (type_colormap) {
        case 0: jet_color(val, &r, &g, &b);     break;
        case 1: inferno_color(val, &r, &g, &b); break;
        default: jet_color(val, &r, &g, &b);    break;
      }

      for (int j = prev; j < idx; ++j) {
        const int x = w - 1;
        const int y = h - 1 - j;
        // if ((unsigned)x < (unsigned)w && (unsigned)y < (unsigned)h)
          img.setPixelColor(x, y, QColor(r, g, b));
      }
      cnt = 0; val = 0.0; prev = idx;
    }
    val += buf[i];
    ++cnt;
  }

  // 4) Flush the last run [prev .. h-1]
  if (cnt > 0) {
    val /= cnt;
    switch (type_colormap) {
      case 0: jet_color(val, &r, &g, &b);     break;
      case 1: inferno_color(val, &r, &g, &b); break;
      default: jet_color(val, &r, &g, &b);    break;
    }
    for (int j = prev; j < h; ++j) {
      const int x = w - 1;
      const int y = h - 1 - j;
      // if ((unsigned)y < (unsigned)h)
        img.setPixelColor(x, y, QColor(r, g, b));
    }
  }

  // 5) Sync back to pixmap so next frame scroll includes the new column
  pixmap_buf.convertFromImage(img);

  // 6) Update policy
  if (++cnt_update >= interval_update) {
    update();
    cnt_update = 0;
  }
}
#endif


void KSpecStream::Stream(short* buf) {
  stft->stft(buf, buf_stft);  
  stft2logspec(buf_stft, buf_pix);
  Stream(buf_pix);
}

void KSpecStream::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  // Recreate buffers to exactly match the new widget size
  refresh();
  // Paint immediately for visual responsiveness
  update();
}

void KSpecStream::resizeStream(QSize size){
  m_width = size.width();
  m_height = size.height();

  img = QImage(m_width, m_height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(m_width, m_height);
  refresh();
}

#ifdef green_theme
void KSpecStream::refresh() {
  resize(width, height);
  QPainter paint(&img);

  QBrush brush_white(Qt::white);
  paint.fillRect(0, 0, width, height, brush_white);

  paint.fillRect(0, 0, width, height, QColor("#161A22")); // dark

  paint.end();

  update();
}
#else
void KSpecStream::refresh() {
  const int w = std::max(1, this->width());
  const int h = std::max(1, this->height());

  img        = QImage(w, h, QImage::Format_RGB16);
  pixmap_buf = QPixmap(w, h);
  pixmap_buf.fill(color_bg_);

  // Reset paint cadence so the next Stream() repaints immediately
  cnt_update = 0;

  QPainter paint(&img);
  paint.fillRect(0, 0, w, h, color_bg_); // dark
  paint.end();

  update();
}
#endif

#ifdef green_theme
void KSpecStream::slot_set_colormap(int val){
  if (val > 0 && val < 2) {
    type_colormap = val;
  }
}
#else
void KSpecStream::slot_set_colormap(int val){
  type_colormap = val;

  pixmap_buf.fill(color_bg_);
  img = pixmap_buf.toImage();
  cnt_update = 0;
  update();
}
#endif

#ifndef green_theme
void KSpecStream::SetBackgroundColor(const QColor& c) {
  color_bg_ = c;
  refresh();
}
#endif