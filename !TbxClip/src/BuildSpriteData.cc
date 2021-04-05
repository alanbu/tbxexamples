// Name        : BuildSpriteData.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : class to create a sprite based on text on the local clipboard
//

#include "BuildSpriteData.h"
#include "tbx/osgraphics.h"

BuildSpriteData::BuildSpriteData(ClipboardData *text_data)
  : _text_data(text_data)
{
}

BuildSpriteData::~BuildSpriteData()
{
}

int BuildSpriteData::size()
{
   if (!_sprite_area.is_valid()) copy_started();
   return _sprite_area.size();
}

const char *BuildSpriteData::data()
{
   if (!_sprite_area.is_valid()) copy_started();
   return reinterpret_cast<const char *>(_sprite_area.pointer());
}

void BuildSpriteData::copy_started()
{
    // Build sprite on demand
	tbx::UserSprite sprite = _sprite_area.create_sprite("textsprite", _text_data->size() * 16, 32, tbx::SF_Colour16dpi90, false);
	tbx::SpriteCapture capture(&sprite, true);
	tbx::OSGraphics g;
	g.foreground(tbx::Colour::black);
	g.text(0,8, std::string(_text_data->data(), _text_data->size()));
}

void BuildSpriteData::copy_finished()
{
	_sprite_area.clear(); // Dump memory for sprite
}
