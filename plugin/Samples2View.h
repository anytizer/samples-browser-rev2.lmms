#pragma once

#include <QObject>
#include <QShortcut>
#include <QString>
#include <QMediaPlayer>
#include <QStringList>
#include <QCloseEvent>
#include <QKeyEvent>

#include "ToolPluginView.h"

#include "Samples2.h"

namespace lmms
{
	class Samples2;
}

namespace lmms::gui
{
	class Samples2View : public ToolPluginView
	{
		private:
			Q_OBJECT
	
		public:
			Samples2View(Samples2* plugin);
		
			void closeEvent(QCloseEvent*) override;
			void keyPressEvent(QKeyEvent*) override;
	};
}