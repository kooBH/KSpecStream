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
	int gap = 4;

	int bool_pos = true;
	int center_y = 0;

	int cnt_vertical = 0;
	int interval_vertical = 150;

#ifndef green_theme
	QColor color_bg_{ "#111821" };
  	QColor color_grid_{ "#2A3140" };
	QColor color_wave_{ "#7DB3FF" };
	QColor color_center_line_{ "#3EA6FF" };
#endif

  void refresh();

protected:
	void paintEvent(QPaintEvent* event) override;

public:
	KWavStream(int width, int height, int n_hop, int n_disp);
	~KWavStream();

#ifndef green_theme
	void SetBackgroundColor(const QColor& c);
  	void SetGridColor(const QColor& c);
	void SetPenColor(const QColor& c);
#endif

	int m_width;
	int m_height;

	int n_hop;
	int n_disp;

	void Stream(short * buf);
	void resizeStream(QSize size);

};

#endif