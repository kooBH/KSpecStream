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
	// spec 생성 시에만  사용
	QImage img;
	// 출력용 버퍼
	QPixmap buf; // 원본 버퍼
	QPixmap buf_alt; // 출력용 버퍼
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