#pragma once

//
// Text Manager
// Holds text instances and helps update and render text instances.
//

struct FONScontext;

class TextManager : public Singleton<TextManager>
{
public:
	struct STextLine
	{
		STextLine();
		virtual ~STextLine();

		void SetText(const char* text) { strncpy_s(_text, text, sizeof(_text)); }
		const char* GetText() const { return _text; }

		void SetID(unsigned int id) { _id = id; }
		unsigned int GetID() const { return _id; }

		void SetFontIndex(unsigned int fontIndex) { _fontIndex = fontIndex; }
		unsigned int GetFontIndex() const { return _fontIndex; }

		void SetPosition(float x, float y) { _x = x; _y = y; }

		float GetX() const { return _x; }
		float GetY() const { return _y; }

		void SetSize(float size) { _size = size; }
		float GetSize() const { return _size; }

		void SetColor(unsigned int color) { _color = color; }
		unsigned int GetColor() const { return _color; }

		void SetBlur(float blur) { _blur = blur; }
		float GetBlur() const { return _blur; }

		void SetSpacing(float spacing) { _spacing = spacing; }
		float GetSpacing() const { return _spacing; }

		void SetShadow(float shadow) { _shadow = shadow; }
		float GetShadow() const { return _shadow; }

		int _id;
		int _fontIndex;
		char _text[128];
		float _x, _y;
		float _size;
		unsigned int _color;
		float _blur;
		float _spacing;
		float _shadow;

		bool operator == (const STextLine* rhs) const
		{
			return (_id == rhs->_id);
		}

		// Dynamic Pool Functions
		static void DestroySystem();
		static STextLine* New();
		static void Delete(STextLine* pTextLine);
		static DynamicPool<STextLine> _pool;
	};

public:
	TextManager();
	virtual ~TextManager();

	// Creates fontstash texture map with specified WxH
	bool Initialize(const int w, const int h);

	// Adds a font to the fontstash
	bool AddFont(const char* font);

	// Creates a new text line
	STextLine* CreateTextLine(const char * text, float x, float y, float size, unsigned int color, float blur, float shadow);

	// Deletes a text line
	void DeleteTextLine(STextLine* pTextLine);

	// Updates (position etc.) all text line instances
	void Update();
	
	// Renders all text line instances
	void Render();

	// Render RainCode demo
	void RenderRainCode(float dT);

	// Process Callbacks
	void ImGuiProcess();

	void Destroy();

private:
	std::vector<STextLine*> _textLineVector;
	FONScontext* _fsContext;

	float VPWidth, VPHeight; // VP Constraints for text positions (Will be controlled by ImGui)
};
