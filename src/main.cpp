#include "common.h"
#include <qevent.h>
#include <QPushButton>

static QPainterPath painterPathForTriangle()
{
	static const QPointF bottomLeft(-1.0, -1.0);
//	static const QPointF top(0.0, 1.0);
	static const QPointF top(0.0, 1.0);
	static const QPointF bottomRight(1.0, -1.0);

	QPainterPath path(bottomLeft);
	path.lineTo(top);
	path.lineTo(bottomRight);
	path.closeSubpath();
	return path;
}

class OpenGLWindow : public QOpenGLWindow
{
	Q_OBJECT

public:
	OpenGLWindow();

protected:
	void paintGL() override;
	void resizeGL(int w, int h) override;
	void keyPressEvent(QKeyEvent *e) override;

private:
	//void setAnimating(bool enabled);

	QMatrix4x4 m_window_normalised_matrix;
	QMatrix4x4 m_projection;
	QMatrix4x4 m_view;
	QMatrix4x4 m_space;

	QBrush     m_brush;

	bool m_animate;
};

static QVector3D s_eye( 1, 1, 1 );
static QVector3D s_center( 0, 0, 0 );
static QVector3D s_up( 0, 1, 0 );

#define PARAM_INCREMENT   0.2

OpenGLWindow::OpenGLWindow()
	: QOpenGLWindow(QOpenGLWindow::NoPartialUpdate)
	, m_animate(true)
{
	//m_view.lookAt(QVector3D(3, 1, 1),
		//QVector3D(0, 0, 0),
		//QVector3D(0, 1, 0));
	m_view.lookAt(s_eye, s_center, s_up);

	QLinearGradient gradient(QPointF(-1, -1), QPointF(1, 1));
	gradient.setColorAt(0, Qt::red);
	gradient.setColorAt(1, Qt::green);

	m_brush = QBrush(gradient);

	//setAnimating(m_animate);
//	typedef void (QPaintDeviceWindow::*QPaintDeviceWindowVoidSlot)();
	//connect(this, &QOpenGLWindow::frameSwapped,
		//this, static_cast<QPaintDeviceWindowVoidSlot>(&QPaintDeviceWindow::update));
}

void OpenGLWindow::paintGL()
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	QRect bgrect = p.window();
	p.setBrush(QBrush(Qt::black));
	p.drawRect( bgrect );
	p.restore();
	p.setWorldTransform(m_window_normalised_matrix.toTransform());

	QMatrix4x4 mvp = m_projection * m_view * m_space;
	p.setTransform( mvp.toTransform(), true );

	p.fillPath(painterPathForTriangle(), m_brush);

	//m_space.rotate(-1, 0, 1, 0);
}

void OpenGLWindow::resizeGL(int w, int h)
{
	m_window_normalised_matrix.setToIdentity();
	m_window_normalised_matrix.translate(w / 2.0, h / 2.0);
	//m_window_normalised_matrix.translate( w, h );
	//	m_window_normalised_matrix.scale(w / 2.0, -h / 2.0);
	m_window_normalised_matrix.scale( w / 3.0, -h / 3.0 );

	m_projection.setToIdentity();
	//m_projection.perspective(45.f, qreal(w) / qreal(h), 0.1f, 100.f);
}

void OpenGLWindow::keyPressEvent(QKeyEvent *e)
{
	if ( e->key() == Qt::Key_Right || e->key() == Qt::Key_Left || e->key() == Qt::Key_Up || e->key() == Qt::Key_Down) // shift parameter
	{
		switch (e->key())
		{
		case Qt::Key_Right:
			s_eye.setX(s_eye.x() + PARAM_INCREMENT);
			break;
		case Qt::Key_Left:
			s_eye.setX(s_eye.x() - PARAM_INCREMENT);
			break;
		case Qt::Key_Up:
			s_eye.setY(s_eye.y() + PARAM_INCREMENT);
			break;
		case Qt::Key_Down:
			s_eye.setY(s_eye.y() - PARAM_INCREMENT);
			break;
		}
		m_view.setToIdentity();
		m_view.lookAt(s_eye, s_center, s_up);
		update();
	}
}

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

		OpenGLWindow window;
		//QWidget wgt;
		//QPushButton btn("TTTTTTTTTTT", &wgt );
		QSurfaceFormat fmt;
		fmt.setDepthBufferSize(24);
		fmt.setStencilBufferSize(8);
		window.setFormat(fmt);
		QSize sz = a.primaryScreen()->size();
		window.resize(sz.width() / 2, sz.height() / 2);
		window.showNormal();
		//wgt.show();

		return a.exec();
}

#include "main.moc"
