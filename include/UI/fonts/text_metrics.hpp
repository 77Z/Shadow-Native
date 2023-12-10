#ifndef SHADOW_NATIVE_UI_FONTS_TEXT_METRICS_HPP
#define SHADOW_NATIVE_UI_FONTS_TEXT_METRICS_HPP

#include "font_manager.hpp"

class TextMetrics
{
public:
	TextMetrics(FontManager* _fontManager);

	/// Append an ASCII/utf-8 string to the metrics helper.
	void appendText(FontHandle _fontHandle, const char* _string);

	/// Append a wide char string to the metrics helper.
	void appendText(FontHandle _fontHandle, const wchar_t* _string);

	/// Return the width of the measured text.
	float getWidth() const { return m_width; }

	/// Return the height of the measured text.
	float getHeight() const { return m_height; }

	/// Clear the width and height of the measured text.
	void clearText();

private:
	FontManager* m_fontManager;
	float m_width;
	float m_height;
	float m_x;
	float m_lineHeight;
	float m_lineGap;
};

/// Compute text crop area for text using a single font.
class TextLineMetrics
{
public:
	TextLineMetrics(const FontInfo& _fontInfo);

	/// Return the height of a line of text using the given font.
	float getLineHeight() const { return m_lineHeight; }

	/// Return the number of text line in the given text.
	uint32_t getLineCount(const bx::StringView& _string) const;

	/// Return the first and last character visible in the [_firstLine, _lastLine] range.
	void getSubText(const bx::StringView& _str, uint32_t _firstLine, uint32_t _lastLine, const char*& _begin, const char*& _end);

private:
	float m_lineHeight;
};

#endif // SHADOW_NATIVE_UI_FONTS_TEXT_METRICS_HPP
