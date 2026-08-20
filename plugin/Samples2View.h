#pragma once

#include <QObject>
#include <QShortcut>
#include <QString>
#include <QMediaPlayer>
#include <QStringList>
#include <QCloseEvent>
#include <QKeyEvent>

#include "ToolPluginView.h"

//#include "WaveformWidget.h"

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
			
			Samples2* m_plugin;
			// QString m_samplesDirectory;
			// WaveformWidget* m_widget;
			// QShortcut* f5Shortcut;
			// QMediaPlayer* mediaPlayer;
			// QString lastSelectedFolderPath;
			// QString currentSelectedFilePath;
			// QStringList baseHeaders;
			// int currentSortColumn = -1;
			// Qt::SortOrder currentSortOrder;
	
		public:
			Samples2View(Samples2* plugin);
		
			void closeEvent(QCloseEvent*) override;
			void keyPressEvent(QKeyEvent*) override;
	};
}