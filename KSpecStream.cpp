#include "KSpecStream.h"

KSpecStream::KSpecStream(
  int width_ ,
  int height_ ,
  int n_fft_ ,
  int n_buf_
) {

  m_width = width_;
  m_height = height_;
  n_fft = n_fft_;
  n_buf = n_buf_;
  n_hop = n_fft / 2;
  n_hfft = n_fft / 2 + 1;

  // printf("KSpecStream : w %d h %d n %d\n", width, height, n_fft);

   //setFixedSize(width, height);
   //setAutoFillBackground(true);

  buf_pix = new double[n_hfft];
  buf_stft = new double[n_fft + 2];
  stft = new STFT(1, n_fft, n_hop);
  buffer = new short[n_hop + n_buf];
  memset(buffer, 0, sizeof(short) * (n_buf + n_hop));

#ifdef green_theme
  img = QImage(m_width, m_height, QImage::Format_RGB16);
  pixmap_buf = QPixmap(m_width, m_height);
#else
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#endif

  refresh();
}

KSpecStream::~KSpecStream() {

  delete[] buf_pix;
  delete[] buf_stft;
  if (buffer)delete[] buffer;
  delete stft;
}


void KSpecStream::paintEvent(QPaintEvent* event) {

  // Ensure the backing pixmap fits the widget; if not, reallocate now.
  if (pixmap_buf.width() != width() ||
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

void KSpecStream::slot_stream_stft(double* stft) {
  StreamSTFT(stft);
}
void KSpecStream::inferno_color(double x, int* r, int* g, int* b) {


  if (x > color_max) x = color_max;
  if (x < color_min) x = color_min;
  float normalizedValue = (x - color_min) / (static_cast<float>(color_max - color_min + 1e-13));

  int idx_color = static_cast<int>(normalizedValue * 1023);

  *r = inferno_1024[idx_color][0];
  *g = inferno_1024[idx_color][1];
  *b = inferno_1024[idx_color][2];
}

void KSpecStream::jet_color(double x, int* r, int* g, int* b) {
  if (x > color_max) x = color_max;
  if (x < color_min) x = color_min;
  float normalizedValue = (x - color_min) / float(color_max - color_min + 1e-13);
  int idx_color = static_cast<int>(normalizedValue * 1023);

  *r = int(jet_1024[idx_color][0] * 255.0f);
  *g = int(jet_1024[idx_color][1] * 255.0f);
  *b = int(jet_1024[idx_color][2] * 255.0f);
}


void KSpecStream::push_buffer(short* buf_in) {
  const int cap = n_hop + n_buf;
  if (sz_buffer + n_buf > cap) {
    const int shift = (sz_buffer + n_buf) - cap;
    if (shift > 0 && shift <= sz_buffer) {
      ::memmove(buffer, buffer + shift, sizeof(short) * (sz_buffer - shift));
      sz_buffer -= shift;
    } else {
      sz_buffer = 0;
    }
  }

  ::memcpy(buffer + sz_buffer, buf_in, sizeof(short) * n_buf);
  sz_buffer += n_buf;
}

void KSpecStream::pop_buffer() {

  for (int i = 0; i < sz_buffer - n_hop; i++) {
    buffer[i] = buffer[i + n_hop];
  }
  sz_buffer -= n_hop;
}

void KSpecStream::stft2logspec(double* src, double* des) {
  const double eps = 1e-12;
  for (int i = 0; i < n_hfft * 2; i += 2) {
    double re = src[i + 0];
    double im = src[i + 1];
    double pwr = re * re + im * im;
    //printf("buf[%d] (%d %d) = %lf : %lf %lf\n", i / 2, re,im, pwr,src[re],src[im]);
    des[i >> 1] = 10.0 * std::log10(pwr + eps);
    //printf("INFO::log_spec::buf[%d] : %lf\n",i/2,buf[i/2]);
  }
}


void KSpecStream::StreamSTFT(double* spec) {
  //printf("KSpecStream::StreamSTFT\n");
  stft2logspec(spec, buf_pix);
  Stream(buf_pix);
}

void KSpecStream::Stream(short* buf_in) {
  push_buffer(buf_in);
  while (sz_buffer >= n_hop) {
    stft->stft(buffer, buf_stft);
    stft2logspec(buf_stft, buf_pix);
    Stream(buf_pix);
    pop_buffer();
  }
}

void KSpecStream::StreamAt(short* buf_in, int64_t base_idx, int samples_per_pixel) {
  // [1] push hop into internal audio buffer (same as Stream(short*))
  push_buffer(buf_in);

  // [2] cache SPP
  if (samples_per_pixel > 0) spp_ = samples_per_pixel;
  if (spp_ <= 0) spp_ = n_hop; // default

  // [3] process STFT frame(s) if available
  while (sz_buffer >= n_hop) {
    stft->stft(buffer, buf_stft);   // n_hop hop size by design
    stft2logspec(buf_stft, buf_pix);

    // Only draw a column when base_idx advanced by >= spp
    if ((base_idx - last_draw_idx_) >= spp_) {
      Stream(buf_pix);              // paint one column
      last_draw_idx_ += spp_;
    }

    pop_buffer();                   // slide by n_hop
  }
}

void KSpecStream::Stream(double* log_mag) {
  if (pixmap_buf.width() != width() ||
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
    val += log_mag[i];
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

void KSpecStream::ResetTimeline(int64_t base_idx) {
  last_draw_idx_ = base_idx;
  sz_buffer = 0;
  // Clear canvas
  if (pixmap_buf.width() != m_width || pixmap_buf.height() != m_height) {
    pixmap_buf = QPixmap(m_width, m_height);
  }
  refresh();
}

void KSpecStream::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  // Recreate buffers to exactly match the new widget size
  refresh();
  // Paint immediately for visual responsiveness
  update();
}

void KSpecStream::resizeStream(QSize size) {
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

  img = QImage(w, h, QImage::Format_RGB16);
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
void KSpecStream::slot_set_colormap(int val) {
  if (val > 0 && val < 2) {
    type_colormap = val;
  }
}
#else
void KSpecStream::slot_set_colormap(int val) {
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