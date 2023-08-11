#ifndef SHADOW_NATIVE_UI_FONT_HPP
#define SHADOW_NATIVE_UI_FONT_HPP

namespace Shadow {

class FontManager {
public:
	FontManager();
	~FontManager();

	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;

	void createFontFromTTF();
};

}

#endif /* SHADOW_NATIVE_UI_FONT_HPP */