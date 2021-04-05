// Name        : BuildSpriteData.h
// Author      : Alan Buckley
// Copyright   : This is released under the same COPYRIGHT as TBX
// Description : class to create a sprite based on text on the local clipboard
//

#ifndef BUILDSPRITEDATA_H
#define BUILDSPRITEDATA_H

#include "tbx/clipboard.h"
#include "tbx/sprite.h"

class BuildSpriteData : public tbx::ClipboardData
{
public:
    BuildSpriteData(ClipboardData *text_data);
	virtual ~BuildSpriteData();

	virtual int size();
    virtual const char *data();
    virtual void copy_started();
	virtual void copy_finished();

private:
	ClipboardData *_text_data;
	tbx::SpriteArea _sprite_area;
};



#endif