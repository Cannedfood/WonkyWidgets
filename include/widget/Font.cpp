#include "Font.hpp"

#include "Error.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <xunicode>

#include <unordered_map>
#include <vector>

static FT_Library gLibrary;
static int        gNumFonts = 0;

static void incrementFonts();
static void decrementFonts();
static const char* getFtErrName(int err);

namespace widget {

struct Font::Data {
	FT_Face face;
	std::unordered_map<float, std::unordered_map<unsigned, std::weak_ptr<FontPage>>> pages;
};

#define mFace mData->face
#define mFontPages mData->pages

Font::Font() :
	mData(nullptr),
	mDpiX(72),
	mDpiY(72)
{
	incrementFonts();
}
Font::Font(std::string const& path, int index) :
	Font()
{
	load(path, index);
}
Font::~Font() {
	free();
	decrementFonts();
}

void Font::setDpi(float x, float y) {
	float newDpiY = y >= 0 ? y : x;
	if(mDpiX != x || mDpiY != newDpiY) {
		mDpiX = x;
		mDpiY = y;
		reloadAllPages();
	}
}

void Font::setTextureGenerator(TextureGenerator&& gen) {
	mTextureGenerator = gen;

	for(auto& pages_by_size : mFontPages) {
		for(auto& pages_by_pageindex : pages_by_size.second) {
			if(std::shared_ptr<FontPage> page = pages_by_pageindex.second.lock()) {
				if(mTextureGenerator) {
					page->texture = mTextureGenerator(page);
				}
				else {
					page->texture = nullptr;
				}
			}
		}
	}
}

void Font::load(std::string const& path, int index) {
	free();

	mData = new Data;
	try {
		int error = FT_New_Face(gLibrary, path.c_str(), index, &mFace);
		if(error) {
			throw exceptions::FailedLoadingFile(path, getFtErrName(error));
		}
	}
	catch(...) {
		delete mData;
		mData = nullptr;
		throw;
	}
}
void Font::load(void const* data, size_t length, int index) {
	free();

	mData = new Data;
	try {
		int error = FT_New_Memory_Face(gLibrary, (FT_Byte const*) data, length, index, &mFace);
		if(error) {
			throw std::runtime_error(std::string("Failed loading font from memory: ") + getFtErrName(error));
		}
	}
	catch(...) {
		delete mData;
		mData = nullptr;
		throw;
	}
}

void Font::free() {
	if(mData) {
		FT_Done_Face(mFace);
		delete mData;
		mData = nullptr;
	}
}

void Font::reloadAllPages() {
	for(auto& pages_by_size : mFontPages) {
		for(auto& pages_by_pageindex : pages_by_size.second) {
			if(std::shared_ptr<FontPage> page = pages_by_pageindex.second.lock()) {
				generateFontPage(page.get(), pages_by_size.first, pages_by_pageindex.first);
				if(mTextureGenerator) {
					page->texture = mTextureGenerator(page);
				}
				else {
					page->texture = nullptr;
				}
			}
		}
	}
}

void Font::generateFontPage(FontPage* page, float size, uint32_t index) {
	bool testPrintability = index <= (0x3F);

	std::vector<FT_GlyphSlot> glyphs(256);

	// TODO: generate
}

std::shared_ptr<FontPage> Font::getFontPage(float size, uint32_t with_character) {
	uint32_t page = with_character >> 8;

	std::weak_ptr<FontPage>& pageRef = mFontPages[size][page];
	std::shared_ptr<FontPage> result = pageRef.lock();
	if(result) {
		return result;
	}

	pageRef = result = std::make_shared<FontPage>();
	generateFontPage(result.get(), size, page);

	return result;
}

void Font::render(std::string const& s, uint8_t* data, unsigned w, unsigned h) {
	
}

} // namespace widget

static
void incrementFonts() {
	if(gNumFonts == 0) {
		int error = FT_Init_FreeType(&gLibrary);
		if(error) {
			throw std::runtime_error(std::string("Failed initializing freetype: ") + getFtErrName(error));
		}
	}
	++gNumFonts;
}

static
void decrementFonts() {
	--gNumFonts;
	if(gNumFonts == 0) {
		FT_Done_FreeType(gLibrary);
	}
}

static
const char* getFtErrName(int err) {
	switch (err) {
		case FT_Err_Cannot_Open_Resource:   return "Cannot_Open_Resource";
		case FT_Err_Unknown_File_Format:    return "Unknown_File_Format";
		case FT_Err_Invalid_File_Format:    return "Invalid_File_Format";
		case FT_Err_Invalid_Version:        return "Invalid_Version";
		case FT_Err_Lower_Module_Version:   return "Lower_Module_Version";
		case FT_Err_Invalid_Argument:       return "Invalid_Argument";
		case FT_Err_Unimplemented_Feature:  return "Unimplemented_Feature";
		case FT_Err_Invalid_Table:          return "Invalid_Table";
		case FT_Err_Invalid_Offset:         return "Invalid_Offset";
		case FT_Err_Array_Too_Large:        return "Array_Too_Large";
		case FT_Err_Missing_Module:         return "Missing_Module";
		case FT_Err_Missing_Property:       return "Missing_Property";
		case FT_Err_Invalid_Glyph_Index:    return "Invalid_Glyph_Index";
		case FT_Err_Invalid_Character_Code: return "Invalid_Character_Code";
		case FT_Err_Invalid_Glyph_Format:   return "Invalid_Glyph_Format";
		case FT_Err_Cannot_Render_Glyph:    return "Cannot_Render_Glyph";
		case FT_Err_Invalid_Outline:        return "Invalid_Outline";
		case FT_Err_Invalid_Composite:      return "Invalid_Composite";
		case FT_Err_Too_Many_Hints:         return "Too_Many_Hints";
		case FT_Err_Invalid_Pixel_Size:     return "Invalid_Pixel_Size";
		case FT_Err_Invalid_Handle:         return "Invalid_Handle";
		case FT_Err_Invalid_Library_Handle: return "Invalid_Library_Handle";
		case FT_Err_Invalid_Driver_Handle:  return "Invalid_Driver_Handle";
		case FT_Err_Invalid_Face_Handle:    return "Invalid_Face_Handle";
		case FT_Err_Invalid_Size_Handle:    return "Invalid_Size_Handle";
		case FT_Err_Invalid_Slot_Handle:    return "Invalid_Slot_Handle";
		case FT_Err_Invalid_CharMap_Handle: return "Invalid_CharMap_Handle";
		case FT_Err_Invalid_Cache_Handle:   return "Invalid_Cache_Handle";
		case FT_Err_Invalid_Stream_Handle:  return "Invalid_Stream_Handle";
		case FT_Err_Too_Many_Drivers:       return "Too_Many_Drivers";
		case FT_Err_Too_Many_Extensions:    return "Too_Many_Extensions";
		case FT_Err_Out_Of_Memory:          return "Out_Of_Memory";
		case FT_Err_Unlisted_Object:        return "Unlisted_Object";
		case FT_Err_Cannot_Open_Stream:     return "Cannot_Open_Stream";
		case FT_Err_Invalid_Stream_Seek:    return "Invalid_Stream_Seek";
		case FT_Err_Invalid_Stream_Skip:    return "Invalid_Stream_Skip";
		case FT_Err_Invalid_Stream_Read:    return "Invalid_Stream_Read";
		case FT_Err_Invalid_Stream_Operation: return "Invalid_Stream_Operation";
		case FT_Err_Invalid_Frame_Operation: return "Invalid_Frame_Operation";
		case FT_Err_Nested_Frame_Access:     return "Nested_Frame_Access";
		case FT_Err_Invalid_Frame_Read:      return "Invalid_Frame_Read";
		case FT_Err_Raster_Uninitialized:    return "Raster_Uninitialized";
		case FT_Err_Raster_Corrupted:        return "Raster_Corrupted";
		case FT_Err_Raster_Overflow:         return "Raster_Overflow";
		case FT_Err_Raster_Negative_Height:  return "Raster_Negative_Height";
		case FT_Err_Too_Many_Caches:         return "Too_Many_Caches";
		case FT_Err_Invalid_Opcode:          return "Invalid_Opcode";
		case FT_Err_Too_Few_Arguments:       return "Too_Few_Arguments";
		case FT_Err_Stack_Overflow:          return "Stack_Overflow";
		case FT_Err_Code_Overflow:           return "Code_Overflow";
		case FT_Err_Bad_Argument:            return "Bad_Argument";
		case FT_Err_Divide_By_Zero:          return "Divide_By_Zero";
		case FT_Err_Invalid_Reference:       return "Invalid_Reference";
		case FT_Err_Debug_OpCode:            return "Debug_OpCode";
		case FT_Err_ENDF_In_Exec_Stream:     return "ENDF_In_Exec_Stream";
		case FT_Err_Nested_DEFS:             return "Nested_DEFS";
		case FT_Err_Invalid_CodeRange:       return "Invalid_CodeRange";
		case FT_Err_Execution_Too_Long:      return "Execution_Too_Long";
		case FT_Err_Too_Many_Function_Defs:  return "Too_Many_Function_Defs";
		case FT_Err_Too_Many_Instruction_Defs: return "Too_Many_Instruction_Defs";
		case FT_Err_Table_Missing:           return "Table_Missing";
		case FT_Err_Horiz_Header_Missing:    return "Horiz_Header_Missing";
		case FT_Err_Locations_Missing:       return "Locations_Missing";
		case FT_Err_Name_Table_Missing:      return "Name_Table_Missing";
		case FT_Err_CMap_Table_Missing:      return "CMap_Table_Missing";
		case FT_Err_Hmtx_Table_Missing:      return "Hmtx_Table_Missing";
		case FT_Err_Post_Table_Missing:      return "Post_Table_Missing";
		case FT_Err_Invalid_Horiz_Metrics:   return "Invalid_Horiz_Metrics";
		case FT_Err_Invalid_CharMap_Format:  return "Invalid_CharMap_Format";
		case FT_Err_Invalid_PPem:            return "Invalid_PPem";
		case FT_Err_Invalid_Vert_Metrics:    return "Invalid_Vert_Metrics";
		case FT_Err_Could_Not_Find_Context:  return "Could_Not_Find_Context";
		case FT_Err_Invalid_Post_Table_Format: return "Invalid_Post_Table_Format";
		case FT_Err_Invalid_Post_Table:      return "Invalid_Post_Table";
		case FT_Err_Syntax_Error:            return "Syntax_Error";
		case FT_Err_Stack_Underflow:         return "Stack_Underflow";
		case FT_Err_Ignore:                  return "Ignore";
		case FT_Err_No_Unicode_Glyph_Name:   return "No_Unicode_Glyph_Name";
		case FT_Err_Glyph_Too_Big:           return "Glyph_Too_Big";
		case FT_Err_Missing_Startfont_Field: return "Missing_Startfont_Field";
		case FT_Err_Missing_Font_Field:      return "Missing_Font_Field";
		case FT_Err_Missing_Size_Field:      return "Missing_Size_Field";
		case FT_Err_Missing_Fontboundingbox_Field: return "Missing_Fontboundingbox_Field";
		case FT_Err_Missing_Chars_Field:     return "Missing_Chars_Field";
		case FT_Err_Missing_Startchar_Field: return "Missing_Startchar_Field";
		case FT_Err_Missing_Encoding_Field:  return "Missing_Encoding_Field";
		case FT_Err_Missing_Bbx_Field:       return "Missing_Bbx_Field";
		case FT_Err_Bbx_Too_Big:             return "Bbx_Too_Big";
		case FT_Err_Corrupted_Font_Header:   return "Corrupted_Font_Header";
		case FT_Err_Corrupted_Font_Glyphs:   return "Corrupted_Font_Glyphs";
		default: return "Unknown error code";
	}
}
