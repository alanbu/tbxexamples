// Name        : TbxClipWnd.cc
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : Class to handle events from the main window

#include "TbxClipWnd.h"
#include "BuildSpriteData.h"
#include "tbx/window.h"
#include "tbx/deleteonhidden.h"
#include "tbx/actionbutton.h"
#include "tbx/clipboard.h"
#include "tbx/caret.h"
#include "tbx/modeinfo.h"
#include "tbx/offsetgraphics.h"
#include "tbx/messagewindow.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * Construct the object and create the toolbox window
 */
TbxClipWnd::TbxClipWnd()
   : _window("Window"),
   _copy(this, &TbxClipWnd::copy),
   _paste(this, &TbxClipWnd::paste),
   _check(this, &TbxClipWnd::check_formats),
   _copy_button(_window.gadget(1)),
   _paste_button(_window.gadget(2)),
   _text(_window.gadget(0)),
   _sprite(_window.gadget(3)),
   _text_and_sprite(_window.gadget(4)),
   _text_loader(_text),
   _sprite_loader(_sprite, _sprite_area),
   _text_and_sprite_loader(_text_and_sprite)
{
	_copy_button.add_selected_command(&_copy);
	_paste_button.add_selected_command(&_paste);
	tbx::ActionButton check_button = _window.gadget(5);
	check_button.add_selected_command(&_check);

	_selected = _text;

	_text.add_loader(&_text_loader, 0xFFF);
	_sprite.add_loader(&_sprite_loader, 0xFF9);
    _text_and_sprite.add_loader(&_text_and_sprite_loader, 0xFFF);

	_window.add_all_mouse_click_listener(this);
	_window.add_redraw_listener(this);

    // Handler to delete this object when the toolbox window has been hidden
    _window.add_has_been_hidden_listener(
              new tbx::DeleteClassOnHidden<TbxClipWnd>(this)
            );

	_sprite_area.load("<TbxClip$Dir>.sample");
}

/**
 * Destructor - deletes toolbox object handled by this class
 */
TbxClipWnd::~TbxClipWnd()
{
   _window.delete_object();
}

void TbxClipWnd::copy()
{
	if (_selected == _text)
	{
		tbx::clipboard()->copy(_text.value());
	} else if (_selected == _sprite)
	{
		// Put sprite area on clipboard
		tbx::clipboard()->copy(0xFF9, reinterpret_cast<char *>(_sprite_area.pointer()), _sprite_area.size());
	} else if (_selected == _text_and_sprite)
	{
		// Copy text to clipboard
		tbx::clipboard()->copy(_text_and_sprite.value());
		// Copy special item that creates a sprite on demand.
		tbx::clipboard()->copy(0xFF9, new BuildSpriteData(tbx::clipboard()->data(0xFFF)), true);
	}
}

void TbxClipWnd::paste()
{
	if (_selected == _text)
	{
		tbx::clipboard()->paste(0xFFF, _text);
	} else if (_selected == _sprite)
	{
		tbx::clipboard()->paste(0xFF9, _sprite);
	} else if (_selected == _text_and_sprite)
	{
		if (tbx::clipboard()->owns_clipboard())
		{
			// Local so we can just copy without going via paste
			tbx::ClipboardData *clip_data = tbx::clipboard()->data(0xFFF);
			if (clip_data)
			{
				_text_and_sprite.value(std::string(clip_data->data(), clip_data->size()));
			}
		} else
		{
			// Paste from another application
			tbx::clipboard()->paste(0xFFF, _text_and_sprite);
		}
	}
}

/**
 * Check clipboard formats available
 */
void TbxClipWnd::check_formats()
{
	int types[2];
	types[0] = 0xFFF; // Text
	types[1] = 0xFF9; // Sprite
	_sprite_checked = false;
	_text_found = false;
	// Adding/removing format listener during check so
	// we don't popup the format message box on all windows
	tbx::clipboard()->add_format_listener(this);
	tbx::clipboard()->check(types, 2,_window);
}

void TbxClipWnd::clipboard_format_available(int file_type)
{
	if (file_type == 0xFFF && !_sprite_checked)
	{
		_text_found = true;
		_sprite_checked = true;
		tbx::clipboard()->check(0xFF9,_window);
	} else
	{
		if (file_type == 0xFFF) _text_found = true;
		std::string msg;
		msg = "Clipboard formats found:\n";
		if (_text_found) msg += "Text (0xFFF)\n";
		if (file_type == 0xFF9) msg += "Sprite (0xFF9)\n";
		if (!_text_found && file_type != 0xFF9)
		{
			std::ostringstream ss;
			ss << "Neither text nor sprite, found 0x" << std::hex << file_type << "\n";
			msg += ss.str();
		}
		tbx::show_message(msg, "Clipboard check", "information");
		tbx::clipboard()->remove_format_listener(this);
	}
}

void TbxClipWnd::clipboard_empty()
{
	tbx::show_message("Clipboard empty", "Clipboard check", "information");
	tbx::clipboard()->remove_format_listener(this);
}

/**
 * Process click on window or one of it's children.
 * On a click we need to set the focus, claim the caret
 * and possibly change the selection.
 */
void TbxClipWnd::mouse_click(tbx::MouseClickEvent &event)
{
	_window.focus();
	tbx::clipboard()->claim_caret();
	tbx::Component clicked_comp = event.id_block().self_component();

	// Change selection only when selectable item is clicked
	if (clicked_comp == _text
		|| clicked_comp == _sprite
		|| clicked_comp == _text_and_sprite)
	{
		tbx::Button clicked_button = clicked_comp;
		if (_selected != clicked_button)
		{
			tbx::BBox redraw_box;
			if (_selected)
			{
				redraw_box = _selected.bounds();
				if (clicked_button)
				{
					redraw_box.cover(clicked_button.bounds());
				}
			} else
			{
				redraw_box = clicked_button.bounds();
			}
			tbx::Size pixel_size = tbx::ModeInfo().screen_unit();
			redraw_box.inflate(pixel_size.width * 2, pixel_size.height * 2);
			_selected = clicked_button;
			_window.force_redraw(redraw_box);

			_copy_button.fade(!_selected);
			_paste_button.fade(!_selected);
		}
	}
}

void TbxClipWnd::redraw(const tbx::RedrawEvent &e)
{
	tbx::OffsetGraphics g(e.visible_area());
	if (_sprite_area.sprite_count())
	{
		tbx::UserSprite sprite = *_sprite_area.begin();
	    if (sprite.is_valid())
		{
			// Clip image to sprite bounds
			tbx::BBox clip = g.clip();
			tbx::BBox old_clip(clip);
			tbx::BBox sprite_bounds(_sprite.bounds());
			// Clip is inclusive so adjust bounding box
			--sprite_bounds.max.x; --sprite_bounds.max.y;
			if (clip.intersect(sprite_bounds))
			{
				g.clip(clip);
				g.image(_sprite.bounds().min, sprite);
				g.clip(old_clip);
			}
		}
	}
	if (_selected)
	{
		g.wimp_foreground(tbx::WimpColour::red);
		tbx::Size pixel_size = tbx::ModeInfo().screen_unit();
		tbx::BBox outline = _selected.bounds();
		outline.inflate(pixel_size.width, pixel_size.height);
		g.rectangle(outline.min.x, outline.min.y, outline.max.x - pixel_size.width, outline.max.y - pixel_size.height);
		outline.inflate(pixel_size.width, pixel_size.height);
		g.rectangle(outline.min.x, outline.min.y, outline.max.x - pixel_size.width, outline.max.y - pixel_size.height);
	}
}
