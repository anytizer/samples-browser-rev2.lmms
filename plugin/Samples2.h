/**
 * Samples2.h - header for Samples2
 */
#pragma once

#include "ToolPlugin.h"

#include "Samples2View.h"

namespace lmms
{
    class ToolPlugin;

    namespace gui
    {
        class PluginView;
        class Samples2View;
    }
}

namespace lmms
{
    class Samples2 : public ToolPlugin
    {
        Q_OBJECT

        public:
            Samples2();
            QString nodeName() const override;
            void saveSettings(QDomDocument&, QDomElement&) override {}
            void loadSettings(const QDomElement&) override {}

            gui::PluginView* instantiateView(QWidget* parent) override;
    };
} // namespace lmms
