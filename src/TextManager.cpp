#include "StdAfx.h"
#include "TextManager.h"
#include "FontImplementation.h"

//
// Text Manager
// Holds text instances and modify, update and render text instances.
//

// TextLine Dynamic Pool
DynamicPool<TextManager::STextLine> TextManager::STextLine::_pool;

TextManager::TextManager() : _fsContext(nullptr), VPWidth(0.0f), VPHeight(0.0f)
{
}

TextManager::~TextManager()
{
}

bool TextManager::Initialize(const int w, const int h)
{
	// Create GL stash for WxH texture, our coordinate system has zero at top-left.
	_fsContext = glfonsCreate(w, h, FONS_ZERO_TOPLEFT);

	if (_fsContext == nullptr)
		return false;

	VPWidth = static_cast<float>(w);
	VPHeight = static_cast<float>(h);

	LOG_INFO("GL stash has been created. Dimensions: {}x{}", VPWidth, VPHeight);

	return true;
}

bool TextManager::AddFont(const char* font)
{
	int fontRes = fonsAddFont(_fsContext, "sans", font);

	// Could not add the font.
	if (fontRes == FONS_INVALID)
	{
		LOG_ERROR("can not add font {} to the font atlas", font);
		return false;
	}

	LOG_INFO("font {} has been added to the font atlas, current atlas font size: {}", font, _fsContext->nfonts);

	return true;
}

TextManager::STextLine* TextManager::CreateTextLine(const char* text, float x, float y, float size, unsigned int color, float blur, float shadow)
{
	static int id = 0;

	STextLine* pTextLine = STextLine::New();
	pTextLine->SetID(id++);
	pTextLine->SetFontIndex(0);
	pTextLine->SetText(text);
	pTextLine->SetPosition(x, y);
	pTextLine->SetSize(size);
	pTextLine->SetColor(color);
	pTextLine->SetShadow(shadow);
	pTextLine->SetBlur(blur);

	_textLineVector.push_back(pTextLine);

	return pTextLine;
}


void TextManager::DeleteTextLine(STextLine* pTextLine)
{
	auto it = _textLineVector.begin();

	for (; it != _textLineVector.end(); ) {
		if (*it == pTextLine)
			it = _textLineVector.erase(it);
		else
			++it;
	}
}

void TextManager::Update()
{
	//for (const auto& p : _textLineVec)
	//{
	//}
}

void TextManager::Render()
{
	for (const auto& p : _textLineVector)
	{
		fonsSetFont(_fsContext, p->GetFontIndex());
		fonsSetSize(_fsContext, p->GetSize());
		fonsSetColor(_fsContext, p->GetColor());
		fonsSetSpacing(_fsContext, p->GetSpacing());

		// Drop Shadow
		if (p->GetShadow())
		{
			fonsSetBlur(_fsContext, p->GetShadow());
			fonsDrawText(_fsContext, p->GetX(), p->GetY(), p->GetText(), NULL);
		}

		fonsSetBlur(_fsContext, p->GetBlur());
		fonsDrawText(_fsContext, p->GetX(), p->GetY(), p->GetText(), NULL);

		fonsClearState(_fsContext);
	}
}

// Inspired by @hypernewbie's VEFontCache demo.
// See: https://github.com/hypernewbie/VEFontCache
void TextManager::RenderRainCode(float dT)
{
	float section_start = 0;
	float current_scroll = 75.0f;

	fonsSetFont(_fsContext, 2);
	fonsSetSize(_fsContext, 50.0f);

	const int GRID_W = 80, GRID_H = 40, NUM_RAINDROPS = GRID_W / 3;

	static bool init_grid = false;
	static int grid[GRID_W * GRID_H];
	static float grid_age[GRID_W * GRID_H];
	static int raindropsX[NUM_RAINDROPS];
	static int raindropsY[NUM_RAINDROPS];
	static float code_colour[4];

	static std::array< std::string, 72> codes = {
		u8" ", u8"0", u8"1", u8"2", u8"3", u8"4", u8"5", u8"6", u8"7", u8"8", u8"9", u8"Z", u8"T", u8"H", u8"E", u8"｜", u8"¦", u8"日",
		u8"ﾊ", u8"ﾐ", u8"ﾋ", u8"ｰ", u8"ｳ", u8"ｼ", u8"ﾅ", u8"ﾓ", u8"ﾆ", u8"ｻ", u8"ﾜ", u8"ﾂ", u8"ｵ", u8"ﾘ", u8"ｱ", u8"ﾎ", u8"ﾃ", u8"ﾏ",
		u8"ｹ", u8"ﾒ", u8"ｴ", u8"ｶ", u8"ｷ", u8"ﾑ", u8"ﾕ", u8"ﾗ", u8"ｾ", u8"ﾈ", u8"ｽ", u8"ﾂ", u8"ﾀ", u8"ﾇ", u8"ﾍ", u8":", u8"・", u8".",
		u8"\"", u8"=", u8"*", u8"+", u8"-", u8"<", u8">", u8"ç", u8"ﾘ", u8"ｸ", u8"ｺ", u8"ﾁ", u8"ﾔ", u8"ﾙ", u8"ﾝ", u8"C", u8"O", u8"D"
	};

	if (!init_grid)
	{
		for (int i = 0; i < NUM_RAINDROPS; i++)
			raindropsY[i] = GRID_H;

		init_grid = true;
	}

	static float fixed_timestep_passed = 0.0f;
	fixed_timestep_passed += dT;

	while (fixed_timestep_passed > (1.0f / 20.0f))
	{
		// Step grid.
		for (int i = 0; i < GRID_W * GRID_H; i++)
			grid_age[i] += dT;

		// Step raindrops.
		for (int i = 0; i < NUM_RAINDROPS; i++)
		{
			raindropsY[i]++;

			if (raindropsY[i] < 0)
				continue;

			if (raindropsY[i] >= GRID_H)
			{
				raindropsY[i] = -5 - (rand() % 40);
				raindropsX[i] = rand() % GRID_W;
				continue;
			}

			grid[raindropsY[i] * GRID_W + raindropsX[i]] = rand() % codes.size();
			grid_age[raindropsY[i] * GRID_W + raindropsX[i]] = 0.0f;
		}

		fixed_timestep_passed -= (1.0f / 20.0f);
	}

	// Draw grid.
	for (int y = 0; y < GRID_H; y++)
	{
		for (int x = 0; x < GRID_W; x++)
		{
			float posx = 1.0f + x * 8.6f, posy = current_scroll - (section_start + 8.6f + y * -25.0f);
			float age = grid_age[y * GRID_W + x];
			code_colour[0] = 1.0f; code_colour[1] = 1.0f; code_colour[2] = 1.0f; code_colour[3] = 1.0f;

			if (age > 0.0f)
			{
				code_colour[0] = 0.2f; code_colour[1] = 0.3f; code_colour[2] = 0.4f;
				code_colour[3] = 1.0f - age;

				if (code_colour[3] < 0.0f)
					continue;
			}

			fonsSetColor(_fsContext,
				encodeRGBA(
					int(code_colour[0] * 255),
					int(code_colour[1] * 255),
					int(code_colour[2] * 255),
					int(code_colour[3] * 255)));

			fonsDrawText(_fsContext, posx, posy, codes[grid[y * GRID_W + x]].data(), NULL);
		}
	}

	code_colour[0] = code_colour[1] = code_colour[2] = code_colour[3] = 1.0f;
	fonsSetColor(_fsContext, 0xFFFFFFFF);
}

void TextManager::ImGuiProcess()
{
	if (ImGui::Button("Create a new TextLine"))
		ImGui::OpenPopup("Create a new TextLine");

	if (ImGui::BeginPopupModal("Create a new TextLine", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("TextLine Details:");
		ImGui::Separator();

		static char n_text[128] = "Text...";
		ImGui::InputText("Text###Create1", &n_text[0], sizeof(n_text));

		static float n_x = 150.0f;
		static float n_y = 150.0f;
		ImGui::DragFloat("X###Create2", &n_x, 1.0f, 0.0f, VPWidth, "%.3f");
		ImGui::SameLine();
		ImGui::DragFloat("Y###Create3", &n_y, 1.0f, 0.0f, VPHeight, "%.3f");

		static float n_size = 65.0f;
		ImGui::InputFloat("Size###Create4", &n_size, 3.0f, 3.0f, 2);

		static float n_spacing = 0.0f;
		ImGui::DragFloat("Spacing###Create5", &n_spacing, 1.0f, -100.0f, 100.0f);

		static float n_blur = 0.0f;
		ImGui::DragFloat("Set Blur###Create6", &n_blur, 0.005f, 0.0f, 5.0f, "%.3f");

		static float n_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		ImGui::ColorEdit4("Color###Create7", (float*)&n_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

		static float n_shadow = 0.0f;
		ImGui::DragFloat("Shadow Color Amount###Create8", &n_shadow, 0.005f, 0.0f, 5.0f);

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			unsigned int n_color_ex = encodeRGBA(int(n_color[0] * 255), int(n_color[1] * 255), int(n_color[2] * 255), int(n_color[3] * 255));
			CreateTextLine(n_text, n_x, n_y, n_size, n_color_ex, n_blur, n_shadow);

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

		ImGui::EndPopup();
	}

	// Push current text lines for edit
	if (ImGui::TreeNodeEx("Current TextLines:", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (const auto& p : _textLineVector)
		{
			ImGui::Button(fmt::format("{}###I{}", p->_text, p->_id).data());

			if (ImGui::BeginPopupContextItem(fmt::format("{}", p->_id).data()))
			{
				float* color = decodeRGBA(p->_color);

				ImGui::Text("Edit Text:");
				ImGui::InputText(fmt::format("Text###TextID{}", p->_id).data(), &p->_text[0], sizeof(p->_text));

				// Have an estimate width boundary.
				// TODO: Get each glpyh width and multiply the width and get the actual size of the text!
				ImGui::DragFloat("X###Edit1", &p->_x, 1.0f, 0.0f, VPWidth, "%.3f");
				ImGui::SameLine();
				ImGui::DragFloat("Y###Edit2", &p->_y, 1.0f, 0.0f, VPHeight, "%.3f");

				if (ImGui::InputFloat("Size###Edit3", &p->_size, 3.0f, 3.0f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					// Because of updating size, while renderupdate calls, renderdraw reads old size and this
					// is causing text to not render properly, this is a quick dirty to fix that.
					CreateTextLine(p->_text, p->_x, p->_y, p->_size, encodeRGBA(int(color[0] * 255), int(color[1] * 255), int(color[2] * 255), int(color[3] * 255)), p->_blur, p->_shadow);
					DeleteTextLine(p);
				}

				ImGui::DragFloat("Set Blur###Edit4", &p->_blur, 0.005f, 0.0f, 5.0f, "%.3f");
				ImGui::DragFloat("Spacing###Edit5", &p->_spacing, 1.0f, -100.0f, 100.0f);

				if (ImGui::ColorEdit4(fmt::format("Color Pick###ColorPick{}", p->_id).data(), color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
					p->SetColor(encodeRGBA(int(color[0] * 255), int(color[1] * 255), int(color[2] * 255), int(color[3] * 255)));

				ImGui::DragFloat("Shadow Color Amount###Edit6", &p->_shadow, 0.005f, 0.0f, 5.0f);

				if (ImGui::Button("Delete###Edit7"))
				{
					DeleteTextLine(p);
					ImGui::CloseCurrentPopup();
				}

				ImGui::Separator();

				if (ImGui::Button("Close Popup###Edit8"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		}

		ImGui::TreePop();
	}
}

void TextManager::Destroy()
{
	STextLine::DestroySystem();
}

///////////////////////
//

TextManager::STextLine* TextManager::STextLine::New()
{
	return _pool.Allocate();
}

void TextManager::STextLine::Delete(TextManager::STextLine* pTextLine)
{
	_pool.Free(pTextLine);
}

void TextManager::STextLine::DestroySystem()
{
	_pool.Destroy();
}

TextManager::STextLine::STextLine() :
	_id(0),
	_fontIndex(0),
	_text(""),
	_x(0.0f),
	_y(0.0f),
	_size(0.0f),
	_color(0xFFFFFFFF), // White default
	_blur(0.0f),
	_spacing(0.0f),
	_shadow(0.0f)
{
}

TextManager::STextLine::~STextLine()
{
}

TextManager tm;
