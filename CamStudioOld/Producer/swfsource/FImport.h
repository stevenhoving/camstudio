#ifndef FLASH_IMPORT_H_FILE
#define FLASH_IMPORT_H_FILE

#include "FBase.h"
#include "FDisplay.h"
#include "FControl.h"
#include "FAction.h"
#include "FBitmap.h"
#include "FSound.h"
#include "FButton.h"
#include "FSprite.h"
#include "FMorph.h"

#include <iostream>

class FlashImportHandler
{
public:
	FlashImportHandler() {}

	virtual void Import(FlashHeader &data) {}
	virtual void Import(FlashTagHeader &data) {}
	
	// To be used with FlashImporter::ImportTag
	virtual void Import(FlashTagRemoveObject &data) {}
	virtual void Import(FlashTagRemoveObject2 &data) {}
	virtual void Import(FlashTagPlaceObject &data) {}
	virtual void Import(FlashTagPlaceObject2 &data) {}
	virtual void Import(FlashTagShowFrame &data) {}

	virtual void Import(FlashTagBackgroundColor &data) {}
	virtual void Import(FlashTagLabelFrame &data) {}
	virtual void Import(FlashTagProtect &data) {}
	virtual void Import(FlashTagEnd &data) {}
	virtual void Import(FlashTagExportAssets &data) {}
	virtual void Import(FlashTagImportAssets &data) {}
	virtual void Import(FlashTagDefineBitsPtr &data) {}
	virtual void Import(FlashTagEnableDebugger &data) {}
	virtual void Import(FlashTagFreeCharacter &data) {}
	
	virtual void Import(FlashTagDefineBitsJPEG1 &data) {}
	virtual void Import(FlashTagJPEGTables &data) {}
	virtual void Import(FlashTagDefineBitsJPEG2 &data) {}
	virtual void Import(FlashTagDefineBitsJPEG3 &data) {}
	virtual void Import(FlashTagDefineBitsLossless &data) {}
	virtual void Import(FlashTagDefineBitsLossless2 &data) {}

	virtual void Import(FlashTagDefineShape1 &data) {}
	virtual void Import(FlashTagDefineShape2 &data) {}
	virtual void Import(FlashTagDefineShape3 &data) {}
	virtual void Import(FlashTagDefineMorphShape &data) {}
	
	virtual void Import(FlashTagDefineSound &data) {}
	virtual void Import(FlashTagDefineSoundMP3 &data) {}
	virtual void Import(FlashTagStartSound &data) {}
	virtual void Import(FlashTagSoundStreamHead &data) {}
	virtual void Import(FlashTagSoundStreamHead2 &data) {}
	virtual void Import(FlashTagSoundStreamBlock &data) {}

	virtual void Import(FlashTagDoAction &data) {}

	virtual void Import(FlashTagDefineButton &data) {}
	virtual void Import(FlashTagDefineButton2 &data) {}
	virtual void Import(FlashTagDefineButtonSound &data) {}

	virtual void Import(FlashTagDefineMovie &data) {}
	virtual void Import(FlashTagSprite &data) {}
	virtual void Import(FlashSpriteEnabled &data)
	{
		switch( data.GetTagID() )
		{
			case 4:  Import( *((FlashTagPlaceObject*)(&data)) ); break;
			case 26: Import( *((FlashTagPlaceObject2*)(&data)) ); break;
			case 5:  Import( *((FlashTagRemoveObject*)(&data)) ); break;
			case 28: Import( *((FlashTagRemoveObject2*)(&data)) ); break;

			case 0:  Import( *((FlashTagEnd*)(&data)) ); break;
			case 1:  Import( *((FlashTagShowFrame*)(&data)) ); break;
			case 12: Import( *((FlashTagDoAction*)(&data)) ); break;
			case 43: Import( *((FlashTagLabelFrame*)(&data)) ); break;

			case 15: Import( *((FlashTagStartSound*)(&data)) ); break;
			case 18: Import( *((FlashTagSoundStreamHead*)(&data)) ); break;
			case 45: Import( *((FlashTagSoundStreamHead2*)(&data)) ); break;
			case 19: Import( *((FlashTagSoundStreamBlock*)(&data)) ); break;
			default: break;
		}
	}

	// Alternate version - necessary at least for Sprite tags import
	// In this version, the handler is responsible for freeing "data"
	// To be used with FlashImporter::ImportTag_AT

	virtual void Import(FlashTagRemoveObject *data)          { delete data; }
	virtual void Import(FlashTagRemoveObject2 *data)			{ delete data; }
	virtual void Import(FlashTagPlaceObject *data)				{ delete data; }
	virtual void Import(FlashTagPlaceObject2 *data)				{ delete data; }
	virtual void Import(FlashTagShowFrame *data)					{ delete data; }

	virtual void Import(FlashTagBackgroundColor *data)			{ delete data; }
	virtual void Import(FlashTagLabelFrame *data)				{ delete data; }
	virtual void Import(FlashTagProtect *data)					{ delete data; }
	virtual void Import(FlashTagEnd *data)							{ delete data; }
	virtual void Import(FlashTagExportAssets *data)				{ delete data; }
	virtual void Import(FlashTagImportAssets *data)				{ delete data; }
	virtual void Import(FlashTagDefineBitsPtr *data)			{ delete data; }
	virtual void Import(FlashTagEnableDebugger *data)			{ delete data; }
	virtual void Import(FlashTagFreeCharacter *data)			{ delete data; }
	
	virtual void Import(FlashTagDefineBitsJPEG1 *data)			{ delete data; }
	virtual void Import(FlashTagJPEGTables *data)				{ delete data; }
	virtual void Import(FlashTagDefineBitsJPEG2 *data)			{ delete data; }
	virtual void Import(FlashTagDefineBitsJPEG3 *data)			{ delete data; }
	virtual void Import(FlashTagDefineBitsLossless *data)		{ delete data; }
	virtual void Import(FlashTagDefineBitsLossless2 *data)	{ delete data; }

	virtual void Import(FlashTagDefineShape1 *data)				{ delete data; }
	virtual void Import(FlashTagDefineShape2 *data)				{ delete data; }
	virtual void Import(FlashTagDefineShape3 *data)				{ delete data; }
	virtual void Import(FlashTagDefineMorphShape *data)		{ delete data; }
	
	virtual void Import(FlashTagDefineSound *data)				{ delete data; }
	virtual void Import(FlashTagDefineSoundMP3 *data)			{ delete data; }
	virtual void Import(FlashTagStartSound *data)				{ delete data; }
	virtual void Import(FlashTagSoundStreamHead *data)			{ delete data; }
	virtual void Import(FlashTagSoundStreamHead2 *data)		{ delete data; }
	virtual void Import(FlashTagSoundStreamBlock *data)		{ delete data; }

	virtual void Import(FlashTagDoAction *data)					{ delete data; }

	virtual void Import(FlashTagDefineButton *data)				{ delete data; }
	virtual void Import(FlashTagDefineButton2 *data)			{ delete data; }
	virtual void Import(FlashTagDefineButtonSound *data)		{ delete data; }

	virtual void Import(FlashTagDefineMovie *data)				{ delete data; }
	virtual void Import(FlashTagSprite *data)						{ delete data; }
	virtual void Import(FlashSpriteEnabled *data)
	{
		switch( data->GetTagID() )
		{
			case 4:  Import( (FlashTagPlaceObject*)data ); break;
			case 26: Import( (FlashTagPlaceObject2*)data ); break;
			case 5:  Import( (FlashTagRemoveObject*)data ); break;
			case 28: Import( (FlashTagRemoveObject2*)data ); break;

			case 0:  Import( (FlashTagEnd*)data ); break;
			case 1:  Import( (FlashTagShowFrame*)data ); break;
			case 12: Import( (FlashTagDoAction*)data ); break;
			case 43: Import( (FlashTagLabelFrame*)data ); break;

			case 15: Import( (FlashTagStartSound*)data ); break;
			case 18: Import( (FlashTagSoundStreamHead*)data ); break;
			case 45: Import( (FlashTagSoundStreamHead2*)data ); break;
			case 19: Import( (FlashTagSoundStreamBlock*)data ); break;
			default: break;
		}
	}

};


#define SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )             \
   {                                                        \
      if( data->isSpriteEnabled() )                         \
         AddToSpriteTagList( (FlashSpriteEnabled*)data );   \
      else                                                  \
         delete data;                                       \
   }

// This handler is responsible for importing Sprite tags and storing them in a list
class FlashSpriteImport : public FlashImportHandler
{
public:
	FlashSpriteImport() {}

	virtual void Import(FlashTagRemoveObject *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagRemoveObject2 *data)         SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagPlaceObject *data)           SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagPlaceObject2 *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagShowFrame *data)             SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )

	virtual void Import(FlashTagBackgroundColor *data)       SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagLabelFrame *data)            SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagProtect *data)               SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagEnd *data)                   SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagExportAssets *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagImportAssets *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineBitsPtr *data)         SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagEnableDebugger *data)        SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagFreeCharacter *data)         SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	
	virtual void Import(FlashTagDefineBitsJPEG1 *data)       SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagJPEGTables *data)            SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineBitsJPEG2 *data)       SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineBitsJPEG3 *data)       SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineBitsLossless *data)    SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineBitsLossless2 *data)   SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )

	virtual void Import(FlashTagDefineShape1 *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineShape2 *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineShape3 *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineMorphShape *data)      SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	
	virtual void Import(FlashTagDefineSound *data)           SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineSoundMP3 *data)        SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagStartSound *data)            SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagSoundStreamHead *data)       SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagSoundStreamHead2 *data)      SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagSoundStreamBlock *data)      SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )

	virtual void Import(FlashTagDoAction *data)              SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )

	virtual void Import(FlashTagDefineButton *data)          SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineButton2 *data)         SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagDefineButtonSound *data)     SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )

	virtual void Import(FlashTagDefineMovie *data)           SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )
	virtual void Import(FlashTagSprite *data)                SWFSOURCE_ADD_TO_SPRITE_TAGLIST( data )

public:
	// Specific methods for this implementation:
	inline void AddToSpriteTagList(FlashSpriteEnabled *tag)           { sprite_tags.push_back(tag); }
	inline N_STD::vector<FlashSpriteEnabled *> &GetSpriteTagsList()   { return sprite_tags; }

private:
	// See FImport.cpp - this Import Handler will be responsible for freeing these tags
	N_STD::vector<FlashSpriteEnabled *>                      sprite_tags;

};


class FlashMyImporter : public FlashImportHandler
{
public:
	FlashMyImporter() {}

	void PrintMatrix(const FlashMatrix &m);
	void PrintCFX(const FlashColorTransform &cfx);
	void PrintRect(const FlashRect &r);

	virtual void Import(FlashHeader &data);
	virtual void Import(FlashTagHeader &data);
	
	virtual void Import(FlashTagRemoveObject &data);
	virtual void Import(FlashTagRemoveObject2 &data);
	virtual void Import(FlashTagPlaceObject &data);
	virtual void Import(FlashTagPlaceObject2 &data);
	virtual void Import(FlashTagShowFrame &data);

	virtual void Import(FlashTagBackgroundColor &data);
	virtual void Import(FlashTagLabelFrame &data);
	virtual void Import(FlashTagProtect &data);
	virtual void Import(FlashTagEnd &data);
	virtual void Import(FlashTagExportAssets &data);
	virtual void Import(FlashTagImportAssets &data);
	virtual void Import(FlashTagDefineBitsPtr &data);
	virtual void Import(FlashTagEnableDebugger &data);
	virtual void Import(FlashTagFreeCharacter &data);
	
	virtual void Import(FlashTagDefineBitsJPEG1 &data);
	virtual void Import(FlashTagJPEGTables &data);
	virtual void Import(FlashTagDefineBitsJPEG2 &data);
	virtual void Import(FlashTagDefineBitsJPEG3 &data);
	virtual void Import(FlashTagDefineBitsLossless &data);
	virtual void Import(FlashTagDefineBitsLossless2 &data);

	virtual void Import(FlashTagDefineShape1 &data);
	virtual void Import(FlashTagDefineShape2 &data);
	virtual void Import(FlashTagDefineShape3 &data);
	virtual void Import(FlashTagDefineMorphShape &data);
	
	virtual void Import(FlashTagDefineSound &data);
	virtual void Import(FlashTagDefineSoundMP3 &data);
	virtual void Import(FlashTagStartSound &data);
	virtual void Import(FlashTagSoundStreamHead &data);
	virtual void Import(FlashTagSoundStreamHead2 &data);
	virtual void Import(FlashTagSoundStreamBlock &data);

	virtual void Import(FlashTagDoAction &data);

	virtual void Import(FlashTagDefineButton &data);
	virtual void Import(FlashTagDefineButton2 &data);
	virtual void Import(FlashTagDefineButtonSound &data);

	virtual void Import(FlashTagDefineMovie &data);
	virtual void Import(FlashTagSprite &data);
	virtual void Import(FlashSpriteEnabled &data)   { FlashImportHandler::Import( data ); }
};

class FlashImporter
{
public:
	FlashImporter() {}
	
	FlashHeader ImportHeader(N_STD::istream &in, FlashImportHandler &i);
	bool ImportTag(N_STD::istream &in, FlashImportHandler &i);
	bool ImportTag_AT(N_STD::istream &in, FlashImportHandler &i);  // alternate version
private:
	
};


#endif
