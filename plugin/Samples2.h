/*
 * Samples2.h - header for Samples2
 */
#pragma once

#include "ToolPlugin.h"

#include "Samples2View.h"

namespace lmms
{
    using lmms::ToolPlugin;
    using lmms::gui::PluginView;

    class Samples2 : public ToolPlugin
    {
        private:
            Q_OBJECT

        public:
            Samples2();
            QString nodeName() const override;
            void saveSettings(QDomDocument&, QDomElement&) override {}
	        void loadSettings(const QDomElement&) override {}

            gui::PluginView* instantiateView(QWidget*) override
            {
                gui::Samples2View* plugin = new gui::Samples2View(this);
                return plugin;
            }
    };
} // namespace lmms
