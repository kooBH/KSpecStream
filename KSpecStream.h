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

class KSpecStream : public QOpenGLWidget {
	Q_OBJECT

	public slots :
	void slot_stream_stft(double*);

private:
	// spec 생성 시에만  사용
	QImage img;
	// 출력용 버퍼
	QPixmap pixmap_buf; // 원본 버퍼
	QPixmap buf_alt; // 출력용 버퍼
	QVBoxLayout layout;


	/*
	0 : jet
	1 : inferno
	*/
	int type_colormap = 0;

	void stft2logspec(double*, double*);
	void jet_color(double x, int*r,int*g,int*b);
	void inferno_color(double x, int* r, int* g, int* b);
	void refresh();

	double* buf_pix;
	double* buf_stft;
	int cnt_update = 0;
	int interval_update = 2;

	STFT * stft;
	int n_hfft;
	int n_fft;

protected:
	void paintEvent(QPaintEvent* event) override;

public:
	KSpecStream(int width, int height, int n_fft);
	~KSpecStream();

	int width;
	int height;

	int color_max = 20;
	int color_min = -40;

	//TODO
	void resizeStream(QSize);

	void StreamSTFT(double * stft);
	void Stream(double * buf);
	void Stream(short * buf);

public	 slots:
	void slot_set_colormap(int);

};

#endif