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
	int cnt_update = 0;
	int interval_update = 2;

protected:
	void paintEvent(QPaintEvent* event) override;

public:
	KSpecStream(int width, int height, int n_hfft);
	~KSpecStream();


	int width;
	int height;

	int n_hfft;

	void StreamSTFT(double * stft);
	void Stream(double * buf);

};

#endif