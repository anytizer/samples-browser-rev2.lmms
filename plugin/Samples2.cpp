/*
 * Samples2.cpp - Samples2.
 *
 */
#include "Samples2.h"
#include "Samples2View.h"

#include "Song.h"
#include "embed.h"
#include "plugin_export.h"

namespace lmms
{
	extern "C"
	{
		Plugin::Descriptor PLUGIN_EXPORT samples2_plugin_descriptor =
		{
			LMMS_STRINGIFY(PLUGIN_NAME),
			"Samples Browser - Rev2",
			QT_TRANSLATE_NOOP("PluginBrowser", "Fetch, Preview and Import .wav samples"),
			"@anytizer <anytizer@users.noreply.github.com>",
			0x0100,
			Plugin::Type::Tool,
			new PluginPixmapLoader("logo"),
			"wav", // @todo copy from Sample Decoder's capabilities
			nullptr,
		};

		PLUGIN_EXPORT Plugin* lmms_plugin_main(Model*, void*)
		{
			return new Samples2();
		}
	}

	QString Samples2::nodeName() const
	{
		return samples2_plugin_descriptor.name;
	}

	Samples2::Samples2()
		: ToolPlugin(&samples2_plugin_descriptor, nullptr)
	{
	}

} // namespace lmms
