#include "KWavStream.h"

KWavStream::KWavStream(
  int _width = 640,
  int _height = 256,
  int _n_hop=256,
  int _n_disp = 400
  ){

  width = _width;
  height = _height;
  n_hop = _n_hop;
  n_disp = _n_disp;

  idx_buf = 0;
  center_y = int(height / 2);
  prev_y = center_y;

  printf("KWavStream : w %d h %d n %d d %d\n", width, height, n_hop,n_disp);

 // setAutoFillBackground(true);

  img = QImage(width, height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(width, height);

  sz_buf = n_hop + n_disp;
  buf_wav = new short[sz_buf];
  memset(buf_wav, 0, sizeof(short) * (n_hop + n_disp));

  refresh();

}



KWavStream::~KWavStream(){
  delete[] buf_wav;
}

void KWavStream::paintEvent(QPaintEvent* event) {

  pixmap_buf.convertFromImage(img);

  QPainter paint;
  paint.begin(this);
  paint.drawPixmap(0, 0, pixmap_buf.width(), pixmap_buf.height(), pixmap_buf);
  paint.end();

}

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

    paint.fillRect(width - gap , 0, gap, height,brush_base);

    //paint.setPen(QPen(Qt::blue, 1, Qt::DashDotLine, Qt::RoundCap));
    paint.setPen(QPen(Qt::blue, 2,Qt::SolidLine, Qt::RoundCap));

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
    val *= 3.0;

    bool_pos = !bool_pos;
    

    val = int(val * ((height/2) / (double)32767));

    if (bool_pos)
      val = center_y + val;
    else
      val = center_y - val;

    paint.drawLine(width - gap, height - prev_y, width -1, height - val);
    prev_y = val;

    //shift
    for (int i = n_disp; i < idx_buf; i++) {
      buf_wav[i - n_disp] = buf_wav[i];
    }
    idx_buf -= n_disp;

    if (cnt_vertical >= interval_vertical) {
      paint.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap));
      paint.drawLine(width - 1, 0, width - 1, height);

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
  
  width = size.width();
  height = size.height();

  center_y = int(height / 2);
  prev_y = center_y;

  img = QImage(width, height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(width, height);

  refresh();

}

void KWavStream::refresh() {
 //  printf("KWavStream::refresh() | %d %d\n",pixmap_buf.width(),pixmap_buf.height());
  //setFixedSize(width, height);
  resize(width, height);

   QBrush brush_base(Qt::white);
   QPainter paint(&img);
   paint.fillRect(0, 0, width, height,brush_base);

   paint.setPen(QPen(Qt::blue, 1,Qt::SolidLine, Qt::RoundCap));
   paint.drawLine(0, center_y, width, center_y);

   paint.end();

   update();
}
