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
	// spec ���� �ÿ���  ���
	QImage img;
	// ��¿� ����
	QPixmap pixmap_buf; // ���� ����
	QPixmap buf_alt; // ��¿� ����
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

#ifndef green_theme
	QColor color_bg_{ "#111821" };
#endif

protected:
	void paintEvent(QPaintEvent* event) override;

public:
	KSpecStream(int width, int height, int n_fft);
	~KSpecStream();

	int m_width;
	int m_height;

	int color_max = 20;
	int color_min = -40;

    void SetUpdateInterval(int val) {interval_update = val;}

	//TODO
	void resizeStream(QSize);

	void StreamSTFT(double * stft);
	void Stream(double * buf);
	void Stream(short * buf);

#ifndef green_theme
	void SetBackgroundColor(const QColor& c);
#endif

public	 slots:
	void slot_set_colormap(int);

};

#endif
