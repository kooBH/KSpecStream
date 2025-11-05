#ifndef _H_KSPECSTREAM_
#define _H_KSPECSTREAM_

#include "colortable_inferno.h"
#include "colortable_jet.h"

#include <QOpenGLWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QPainter>
#include <QObject>
#include <QBrush>
#include <QImage>              
#include <QPixmap>            
#include <QColor>

#include "STFT.h"

class KSpecStream : public QWidget  {
	Q_OBJECT

public:
  KSpecStream(int width_ = 640, int height_ = 256, int n_fft_ = 256, int n_buf_ = 128);
  ~KSpecStream();

//============//
//= Drawring =//
//============//
private:
	QImage img; // itermediate for QImage drawing
	QPixmap pixmap_buf; // Actual drawing
	QVBoxLayout layout;

	QColor color_bg_{ "#111821" };

	int cnt_update = 0;
	int interval_update = 1;

	void refresh();

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;


public :
	int m_width;
	int m_height;

  	void SetUpdateInterval(int val) {interval_update = std::max(1,val);}
	void SetBackgroundColor(const QColor& c);
	void resizeStream(QSize);
	void ResetTimeline(int64_t base_idx = 0);

//============//
//= Colormap =//
//============//
private :
	/*
	0 : jet
	1 : inferno
	*/
	int type_colormap = 0;

	void jet_color(double x, int*r,int*g,int*b);
	void inferno_color(double x, int* r, int* g, int* b);

public : 
	int color_max = 20;
	int color_min = -40;

	public slots :
	void slot_set_colormap(int);


//==============//
//= Processing =//
//==============//
private :
	void stft2logspec(double*, double*);

	double* buf_pix;
	double* buf_stft;

	STFT * stft;
	int n_fft; 
	int n_hop; // will be n_fft/2
	int n_hfft;
  	int n_buf; // unit of input buffer
  	// NOTE : n_buf might be different from n_hop
	short* buffer = nullptr;
	int sz_buffer = 0;

	int64_t last_draw_idx_ = 0;  // last base sample index that produced the last column
	int     spp_           = 0;  // samples per pixel

	void push_buffer(short* buf_in);
	void pop_buffer();
public : 
	void StreamAt(short* buf, int64_t base_idx, int samples_per_pixel);

	void StreamSTFT(double * spec);
	void Stream(double * log_mag);
	void Stream(short * buf);

public slots :
	void slot_stream_stft(double*);

};

#endif
