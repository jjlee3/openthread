#pragma once

namespace OpenThreadTalk
{
	enum class Protocol
	{
		TCP,
		UDP,
	};

	struct Configurations
	{
		Protocol protocol = Protocol::TCP;
	};
}

extern OpenThreadTalk::Configurations g_configurations;
