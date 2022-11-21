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

#include <random>
#include <windows.h>

#include <thread>

class KSpecStream : public QOpenGLWidget {
	Q_OBJECT

private:
	// spec ���� �ÿ���  ���
	QImage img;
	// ��¿� ����
	QPixmap buf; // ���� ����
	QPixmap buf_alt; // ��¿� ����
	QVBoxLayout layout;

	std::random_device rd;

	std::thread* thread_process = nullptr;

protected:
	void paintEvent(QPaintEvent* event) override;


public:
	KSpecStream();
	~KSpecStream();


	int width=640;
	int height=480;

	void Stream();

};

#endif