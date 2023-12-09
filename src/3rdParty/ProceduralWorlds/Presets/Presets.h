#pragma once

namespace that
{
	class Generator;

	namespace preset
	{
		class Presets
		{
		public:
			static void CreateDefaultTerrain(Generator& generator, unsigned int seed, float mapZoom);
		};
	}
}

