#ifndef _H_KWAV_STREAM_
#define _H_KWAV_STREAM_

#include <QOpenGLWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QPainter>
#include <QObject>
#include <QBrush>

#include <QImage>              
#include <QPixmap>            
#include <QColor>

class KWavStream : public QOpenGLWidget {
	Q_OBJECT

	public slots :
	void slot_stream_wav(short*);

private:
	QImage img;
	QPixmap pixmap_buf; 	
	QVBoxLayout layout;

	short* buf_wav;
	int idx_buf;
	int sz_buf;
	int prev_y = 0;
	double gap_accum_ = 0.0;   // fractional scroll accumulator
  	double gap_f_ = 2.0;		// 3.1 for mpADB
  	int gap_i_ = 3;            // integer fallback

	int bool_pos = true;
	int center_y = 0;

	int cnt_vertical = 0;
	int interval_vertical = 150;

	int64_t last_draw_idx_ = 0;        // last base sample index that produced the last column
	int     spp_           = 0;        // samples per pixel (cached)

	QColor color_bg_{ "#111821" };
  	QColor color_grid_{ "#2A3140" };
	QColor color_wave_{ "#7DB3FF" };
	QColor color_center_line_{ "#3EA6FF" };

	double amp_scale_ = 1.0;

  void refresh();

protected:
	void paintEvent(QPaintEvent* event) override;

public:
	KWavStream(int width, int height, int n_hop, int n_disp);
	~KWavStream();

	void SetBackgroundColor(const QColor& c);
  	void SetGridColor(const QColor& c);
	void SetPenColor(const QColor& c);

	inline void SetAmplitudeScale(double s) {
		amp_scale_ = (s > 0.0 ? s : 1.0);
	}

	void SetScrollSpeed(float speed) {gap_f_ = speed; gap_accum_ = 0.0f;}
	float GetScrollSpeed() {return gap_f_;}

	void ResetTimeline(int64_t base_idx = 0) {
		last_draw_idx_ = base_idx;
		idx_buf = 0;
		prev_y = center_y;
		gap_accum_ = 0.0;
		// Clear canvas
		img = QImage(m_width, m_height, QImage::Format_RGB16);
		pixmap_buf = QPixmap(m_width, m_height);
		refresh();
	}

	int m_width;
	int m_height;

	int n_hop;
	int n_disp;

	void StreamAt(short* buf, int64_t base_idx, int samples_per_pixel);
	void Stream(short * buf);
	void resizeStream(QSize size);
};

#endif