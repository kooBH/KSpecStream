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
	// spec ���� �ÿ���  ���
	QImage img;
	// ��¿� ����
	QPixmap buf; // ���� ����
	QPixmap buf_alt; // ��¿� ����
	QVBoxLayout layout;

	void stft2logspec(double*, double*);
	void jet_color(double x, int*r,int*g,int*b);

	double* buf_pix;

protected:
	void paintEvent(QPaintEvent* event) override;

public:
	KSpecStream(int width, int height);
	~KSpecStream();


	int width;
	int height;

	int n_fft;

	void StreamSTFT(double * stft);
	void Stream(double * buf);

};

#endif