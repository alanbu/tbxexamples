// Name        : Loaders.h
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Helper classes to load data to the buttons and sprite
//

#ifndef _LOADERS_
#define _LOADERS_

#include "tbx/memoryloader.h"
#include "tbx/button.h"
#include "tbx/sprite.h"

class ButtonLoader : public tbx::MemoryLoader
{
public:
    ButtonLoader(tbx::Button button)
	  : tbx::MemoryLoader(true, "Failed to load text"),
	  _button(button)
	  {}

    virtual void data_loaded(const tbx::LoadEvent &, char *data, int data_size, bool &)
	{
		_button.value(std::string(data, data_size));
	}
private:
    tbx::Button _button;
};

class SpriteLoader : public tbx::Loader
{
	public:
	   SpriteLoader(tbx::Button button, tbx::SpriteArea &sprite_area);
	   virtual ~SpriteLoader();

	   virtual bool load_file(tbx::LoadEvent &event);
	   virtual void *data_buffer(const tbx::LoadEvent &event, int &buffer_size);
	   virtual bool data_received(tbx::DataReceivedEvent &event);
	   virtual void data_error(const tbx::LoadEvent &event);
	private:
	   void refresh_button();

	private:
	   tbx::Button _button;
	   tbx::SpriteArea &_sprite_area;
	   int *_buffer;
	   int _size;
	   int _offset;
};

#endif
