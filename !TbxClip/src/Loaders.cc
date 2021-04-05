// Name        : Loaders.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Helper classes to load data to the buttons and sprite
//

#include "Loaders.h"
#include "tbx/messagewindow.h"
#include <cstring>

SpriteLoader::SpriteLoader(tbx::Button button, tbx::SpriteArea &sprite_area) :
   _button(button),
   _sprite_area(sprite_area),
   _buffer(0),
   _size(0),
   _offset(0)
{
}

SpriteLoader::~SpriteLoader()
{
	delete [] _buffer;
}

bool SpriteLoader::load_file(tbx::LoadEvent &event)
{
	_sprite_area.clear();
	bool ok = _sprite_area.load(event.file_name());
	refresh_button();
	return ok;
}

void *SpriteLoader::data_buffer(const tbx::LoadEvent &event, int &buffer_size)
{
	delete [] _buffer;
	_size = buffer_size / 4;
	if (buffer_size & 3) ++_size;
	++_size; // Add one for area size
	++_size; // Add one so it comes in one messages if buffer_size is correct
	_offset = 4; // Start after size
	_buffer = new int[_size];
	buffer_size = _size * 4;
	return _buffer;
}

bool SpriteLoader::data_received(tbx::DataReceivedEvent &event)
{
	_offset += event.received();
	event.buffer_size(_size * 4 - _offset);
	if (event.more())
	{
		if (event.buffer_size() <= 0)
		{
			int new_size = _size + 64;
			try
			{
			int *new_buf = new int[new_size];
			std::memcpy(new_buf, _buffer, _offset);
			delete [] _buffer;
			_buffer = new_buf;
			_size = new_size;
			event.buffer_size(_size * 4 - _offset);
			} catch(...)
			{
				// Most likely a memory exception so give up
				// Delete buffer as it may be large by now.
				delete [] _buffer;
				_buffer = 0;
				_size = _offset = 0;
				data_error(event.load_event());
				return false;
			}
		}
		event.buffer(_buffer + _offset);
	} else
	{
		_buffer[0] = _size * 4;
		_sprite_area.set(_buffer, true);
		_buffer = 0;
		_size = 0;
		_offset = 0;
        refresh_button();
	}

	return true;
}

void SpriteLoader::data_error(const tbx::LoadEvent &event)
{
	tbx::show_message("Failed to load the sprite");
}

void SpriteLoader::refresh_button()
{
	tbx::BBox bounds = _button.bounds();
	_button.window().force_redraw(bounds);
}
