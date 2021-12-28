namespace ion
{
	namespace gamekit
	{
		static const int debugFontCharacterWidth = 8;
		static const int debugFontCharacterHeight = 8;
		static const int debugFontNumCharacters = 1;

		static const u8 debugFont[debugFontNumCharacters][debugFontCharacterWidth*debugFontCharacterHeight] =
		{
			{
				0,0,0,1,1,0,0,0,
				0,0,1,0,0,1,0,0,
				0,1,0,0,0,0,1,0,
				0,1,0,0,0,0,1,0,
				0,1,1,1,1,1,1,0,
				0,1,0,0,0,0,1,0,
				0,1,0,0,0,0,1,0,
				0,1,0,0,0,0,1,0,
			}
		};
	}
}