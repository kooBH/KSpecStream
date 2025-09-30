#include "KWavStream.h"

KWavStream::KWavStream(
  int _width = 640,
  int _height = 256,
  int _n_hop=256,
  int _n_disp = 400
  ){

  m_width = _width;
  m_height = _height;
  n_hop = _n_hop;
  n_disp = _n_disp;

  idx_buf = 0;
  center_y = int(m_height / 2);
  prev_y = center_y;

  printf("KWavStream : w %d h %d n %d d %d\n", m_width, m_height, n_hop,n_disp);

 // setAutoFillBackground(true);

  img = QImage(m_width, m_height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(m_width, m_height);

  sz_buf = n_hop + n_disp;
  buf_wav = new short[sz_buf];
  memset(buf_wav, 0, sizeof(short) * (n_hop + n_disp));

  refresh();

}



KWavStream::~KWavStream(){
  delete[] buf_wav;
}

#ifdef green_theme
void KWavStream::paintEvent(QPaintEvent* event) {

  pixmap_buf.convertFromImage(img);

  QPainter paint;

  paint.begin(this);
  paint.drawPixmap(0, 0, pixmap_buf.width(), pixmap_buf.height(), pixmap_buf);
  paint.end();

}
#else
void KWavStream::paintEvent(QPaintEvent* event) {

  pixmap_buf.convertFromImage(img);

  QPainter paint;
  paint.begin(this);
  paint.fillRect(rect(), color_bg_);
  paint.drawPixmap(rect(), pixmap_buf, pixmap_buf.rect());
  paint.end();
}
#endif

void KWavStream::slot_stream_wav(short* stft) {
  Stream(stft);
}


void KWavStream::Stream(short* buf) {
  int r, g, b;
  int idx = 0;
  int cnt = 0;
  double val = 0;

  memcpy(buf_wav + idx_buf, buf, sizeof(short) * n_hop);
  idx_buf += n_hop;

  //printf("Stream : %d | %d %d\n",idx_buf,n_hop,n_disp);

  while (idx_buf > n_disp) {
    QRegion exposed;
    pixmap_buf.scroll(-gap, 0, pixmap_buf.rect(), &exposed);
    img = pixmap_buf.toImage();

    QBrush brush_base(Qt::white);

    QPainter paint(&img);

  #ifdef green_theme
    paint.fillRect(m_width - gap , 0, gap, height,brush_base);

    //paint.setPen(QPen(Qt::blue, 1, Qt::DashDotLine, Qt::RoundCap));
    paint.setPen(QPen(Qt::blue, 2,Qt::SolidLine, Qt::RoundCap));
  #else
    paint.fillRect(m_width - gap , 0, gap, m_height, color_bg_);
    paint.setPen(QPen(color_wave_, 2, Qt::SolidLine, Qt::RoundCap));
  #endif  

    // display max point
    int val = 0;
    int t = 0;
    for (int i = 0; i < n_disp; i++) {
      t = std::abs(buf_wav[i]);
      if (t > val) {
        val = t;
        /*
        if (buf_wav[i] > 0)
          bool_pos = true;
        else
          bool_pos = false;
          */
      }
    }
    //val *= 3.0;

    bool_pos = !bool_pos;
    

    val = int(val * ((m_height/2) / (double)32767));

    if (bool_pos)
      val = center_y + val;
    else
      val = center_y - val;

    paint.drawLine(m_width - gap, m_height - prev_y, m_width -1, m_height - val);
    prev_y = val;

    //shift
    for (int i = n_disp; i < idx_buf; i++) {
      buf_wav[i - n_disp] = buf_wav[i];
    }
    idx_buf -= n_disp;

    if (cnt_vertical >= interval_vertical) {
    #ifdef green_theme
      paint.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
    #else
      paint.setPen(QPen(color_grid_, 2, Qt::SolidLine, Qt::RoundCap));
    #endif

      paint.drawLine(m_width - 1, 0, m_width - 1, m_height);

      cnt_vertical = 0;
    }
    else
      cnt_vertical++;

    paint.end();
  }
  update();
}

void KWavStream::resizeStream(QSize size) {
  //printf("kWavStream::resizeStream | %d %d\n",size.width(),size.height());
  
  m_width = size.width();
  m_height = size.height();
#ifndef green_theme
  resize(m_width, m_height);

  pixmap_buf = QPixmap(m_width, m_height);
  pixmap_buf.fill(color_bg_);
  img = pixmap_buf.toImage();

  center_y = m_height / 2;
  prev_y   = center_y;
  idx_buf  = 0;
  update();
#else
  center_y = int(height / 2);
  prev_y = center_y;

  img = QImage(width, height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(width, height);
  refresh();
#endif

}

#ifdef green_theme
void KWavStream::refresh() {
 //  printf("KWavStream::refresh() | %d %d\n",pixmap_buf.width(),pixmap_buf.height());
  //setFixedSize(width, height);
  resize(width, height);
   QPainter paint(&img);
   QBrush brush_base(Qt::white);
   paint.fillRect(0, 0, width, height,brush_base);

   paint.setPen(QPen(Qt::blue, 1,Qt::SolidLine, Qt::RoundCap));

   paint.drawLine(0, center_y, width, center_y);

   paint.end();

   update();
}
#else
void KWavStream::refresh() {
  const int w = this->width();
  const int h = this->height();
  if (w <= 0 || h <= 0) return;

  pixmap_buf = QPixmap(w, h);
  pixmap_buf.fill(color_bg_);
  img = pixmap_buf.toImage();

  center_y = h / 2;
  prev_y   = center_y;

  QPainter paint(&img);
  paint.setPen(QPen(color_center_line_, 1, Qt::SolidLine, Qt::RoundCap));

  paint.drawLine(0, center_y, w, center_y);
  paint.end();

  update();
}
#endif

#ifndef green_theme
void KWavStream::SetBackgroundColor(const QColor& c) {
  color_bg_ = c;
  resizeStream(size());
}
void KWavStream::SetGridColor(const QColor& c) {
  color_grid_ = c;
  update();
}
void KWavStream::SetPenColor(const QColor& c) {
  color_wave_ = c;
  update();
}
#endif