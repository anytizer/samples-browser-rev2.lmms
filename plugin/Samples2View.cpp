#include <QLayout>
#include <QDomElement>
#include <QKeyEvent>
#include <QDebug>

#include "Samples2.h"
#include "Samples2View.h"
//#include "WaveformWidget.h"


#include "Song.h"
#include "Track.h"
#include "SampleTrack.h"
#include "PatternStore.h"
#include "InstrumentTrack.h"
#include "Instrument.h"

#include "SamplePlayHandle.h"


namespace lmms::gui
{
	Samples2View::Samples2View(Samples2* plugin)
	: ToolPluginView(plugin)
	, m_plugin(plugin)
	{
		this->setWindowTitle("Samples Browser - Rev2");
		this->setMinimumSize(800, 600);
		this->setFixedSize(800, 600);
    	this->resize(800, 600);
		this->setAcceptDrops(true);
		

		QLayout* layout = this->layout();
		if (layout) {
			QLayoutItem* item;
			while ((item = layout->takeAt(0)) != nullptr) {
				if (QWidget* widget = item->widget()) {
					widget->deleteLater();
				}
				delete item;
			}
		}
		delete this->layout();

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		QVBoxLayout* tl = new QVBoxLayout();

		// WaveformWidget* m_widget = new WaveformWidget(this);
		// m_widget->setMinimumHeight(140);

		// tl->addWidget(m_widget, 1);
		mainLayout->addLayout(tl);








		QWidget* pw = parentWidget();
		if (pw!=nullptr)
		{
			pw->hide(); // default hidden

			Qt::WindowFlags flags = pw->windowFlags();
			//flags |= Qt::MSWindowsFixedSizeDialogHint;
			//flags |= Qt::WindowMaximizeButtonHint;
			flags |= Qt::WindowStaysOnTopHint;
			pw->setWindowFlags(flags);

			pw->adjustSize();
		}
	}

	void Samples2View::closeEvent(QCloseEvent*) {}
	void Samples2View::keyPressEvent(QKeyEvent* event) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_F5) {
				// ask();
				qDebug() << "F5 key caught by Samples2View.cpp!";
				return;// true; // Handled/Intercepted
			}

			if (keyEvent->key() == Qt::Key_Escape)
			{
				// @todo It is NOT closing properly.
				// this->hide();
				this->close();
				return;
			}
		}
	}
} // namespace
