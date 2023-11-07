#ifndef _H_KSPECSTREAM_
#define _H_KSPECSTREAM_

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

	void stft2logspec(double*, double*);
	void jet_color(double x, int*r,int*g,int*b);

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

	//TODO
	void resizeStream(QSize);

	void StreamSTFT(double * stft);
	void Stream(double * buf);
	void Stream(short * buf);



};

#endif