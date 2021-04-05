// Name        : TbxClipWnd.h
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Class to handle events from the main window

#ifndef _TbxClipWND_H_
#define _TbxClipWND_H_

#include "tbx/window.h"
#include "tbx/button.h"
#include "tbx/actionbutton.h"
#include "tbx/command.h"
#include "tbx/loader.h"
#include "tbx/mouseclicklistener.h"
#include "tbx/redrawlistener.h"
#include "tbx/sprite.h"
#include "tbx/memoryloader.h"
#include "tbx/clipboard.h"
#include "Loaders.h"



/**
 * Class to handle the main window.
 */
class TbxClipWnd
: public tbx::MouseClickListener,
  public tbx::RedrawListener,
  public tbx::ClipboardFormatListener
{
     // The toolbox window this is handling
     tbx::Window _window;
public:
     TbxClipWnd();
     ~TbxClipWnd();

	 void copy();
	 void paste();
	 void check_formats();

	virtual void mouse_click(tbx::MouseClickEvent &event);
	virtual void redraw(const tbx::RedrawEvent &e);

	virtual void clipboard_format_available(int file_type);
	virtual void clipboard_empty();

private:
    tbx::CommandMethod<TbxClipWnd> _copy;
    tbx::CommandMethod<TbxClipWnd> _paste;
	tbx::CommandMethod<TbxClipWnd> _check;
	tbx::ActionButton _copy_button;
	tbx::ActionButton _paste_button;
    tbx::Button _text;
	tbx::Button _sprite;
	tbx::Button _text_and_sprite;
	tbx::SpriteArea _sprite_area;
	ButtonLoader _text_loader;
	SpriteLoader _sprite_loader;
	ButtonLoader _text_and_sprite_loader;

	tbx::Button _selected;
	bool _sprite_checked;
	bool _text_found;
};

#endif
