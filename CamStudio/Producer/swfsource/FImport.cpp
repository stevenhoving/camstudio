#include "FBase.h"
#include "FImport.h"
#include "FBitmap.h"
#include "FSound.h"
#include "FControl.h"
#include "FButton.h"

#include <iostream>

// NOT COMPLETE

void FlashMyImporter::PrintMatrix(const FlashMatrix &m)
{
	if(m.HasRotate()) std::cout << "  Rotate:    " << (double)m.GetRotateX().lowerval/0xffff + m.GetRotateX().upperval << ", " << (double)m.GetRotateY().lowerval/0xffff + m.GetRotateY().upperval << "\n";
	if(m.HasScale())  std::cout << "  Scale:     " << (double)m.GetScaleX().lowerval/0xffff + m.GetScaleX().upperval << ", " << (double)m.GetScaleY().lowerval/0xffff + m.GetScaleY().upperval << "\n";
	std::cout					<< "  Translate: " << m.GetTranslateX()  << ", " << m.GetTranslateY() << "\n";
}

void FlashMyImporter::PrintCFX(const FlashColorTransform &cfx)
{
	
	if(cfx.HasAdd())
	{
		if(cfx.GetAddRGB().GetAlphaWriteMode())
		{
			std::cout   << "  AddRGBA:  " << cfx.GetAddRGB().GetR() << ", " << cfx.GetAddRGB().GetG() << ", " << cfx.GetAddRGB().GetB() << ", " << cfx.GetAddRGB().GetA() << "\n";
		}
		else std::cout  << "  AddRGB:   " << cfx.GetAddRGB().GetR() << ", " << cfx.GetAddRGB().GetG() << ", " << cfx.GetAddRGB().GetB() << "\n";
	}
	if(cfx.HasMult())
	{
		if(cfx.GetMultRGB().GetAlphaWriteMode())
		{
			std::cout   << "  MultRGBA:  " << cfx.GetMultRGB().GetR() << ", " << cfx.GetMultRGB().GetG() << ", " << cfx.GetMultRGB().GetB() << ", " << cfx.GetMultRGB().GetA() << "\n";
		}
		else std::cout   <<"  MultRGB:   " << cfx.GetMultRGB().GetR() << ", " << cfx.GetMultRGB().GetG() << ", " << cfx.GetMultRGB().GetB() << "\n";
	}
}


void FlashMyImporter::PrintRect(const FlashRect &r)
{
	std::cout << " [RECT] (" << r.GetX1() << ", " << r.GetY1() << ", " << r.GetX2() << ", " << r.GetY2() << ")\n";
}

void FlashMyImporter::Import(FlashHeader &data)
{
	std::cout << "\n<FLASH FILE HEADER>\n";
	std::cout << "File Version: "<< (int)data.GetVersion() << "\n";
	std::cout << "Frame Size (x): "  << (data.GetScreenSize().GetX2()) << "\n";
	std::cout << "Frame Size (y): "  << (data.GetScreenSize().GetY2()) << "\n";
	std::cout << "Frame Rate: "  << data.GetFrameRate().ConvertToDouble() << "\n";
	std::cout << "Frame Count: " << data.GetFrameCount() << "\n";
	std::cout << "File Length: " << data.GetFilesize() << "\n";
	if( (int)data.GetVersion() > 5 )
		std::cout << "Compressed Movie: " << data.IsMovieCompressed() << "\n";
}

void FlashMyImporter::Import(FlashTagHeader &data)
{
	std::cout << "\n<UNKNOWN TAG>\n";
	std::cout << "ID: "		<< data.GetTagID() << "\n";
	std::cout << "Length: " << data.GetTagLength() << "\n";
}
void FlashMyImporter::Import(FlashTagRemoveObject &data)
{
	std::cout << "\n<RemoveObject>\n";
	std::cout << "Character ID: " << data.GetCharID() << "\n";
	std::cout << "Depth: "        << data.GetDepth() << "\n";
}
void FlashMyImporter::Import(FlashTagRemoveObject2 &data)
{
	std::cout << "\n<RemoveObject2>\n";
	std::cout << "Depth: "        << data.GetDepth() << "\n";
}
void FlashMyImporter::Import(FlashTagPlaceObject &data)
{
	std::cout << "\n<PlaceObject>\n";
	std::cout << "Character ID: " << data.GetCharID() << "\n";
	std::cout << "Depth: "		  << data.GetDepth() << "\n";
	std::cout << "Matrix: "		  << "\n";
	PrintMatrix(data.GetMatrix());
	if(data.HasColorTransform())  std::cout << "HasColorTransform: true"  << "\n"; 
	else						std::cout << "HasColorTransform: false" << "\n"; 
}
void FlashMyImporter::Import(FlashTagPlaceObject2 &data)
{
	std::cout << "\n<PlaceObject2>\n";
	std::cout << "Depth: "		  << data.GetDepth() << "\n";
	
	if(data.HasCharID())		std::cout << "HasCharID: true [" << data.GetCharID() << "]\n"; 
	else						std::cout << "HasCharID: false" << "\n"; 
	if(data.HasMatrix())
	{
		std::cout << "HasMatrix: true"  << "\n"; 
		PrintMatrix(data.GetMatrix());
	}
	else						std::cout << "HasMatrix: false" << "\n"; 
	
	if(data.HasColorTransform())
	{
		std::cout << "HasColorTransform: true"  << "\n"; 
		PrintCFX(data.GetColorTransform());
	}
	else						std::cout << "HasColorTransform: false" << "\n"; 
	if(data.HasMove())			std::cout << "HasMove: true"  << "\n"; 
	else						std::cout << "HasMove: false" << "\n"; 
	if(data.HasRatio())			std::cout << "HasRatio: true ["  << data.GetRatio() << "]\n"; 
	else						std::cout << "HasRatio: false" << "\n"; 
	if(data.HasName())			std::cout << "HasName: true ["  << data.GetName() << "]\n"; 
	else						std::cout << "HasName: false" << "\n"; 
	if(data.HasClipDepth())		std::cout << "HasClipDepth: true ["  << data.GetClipDepth() << "]\n"; 
	else						std::cout << "HasClipDepth: false" << "\n"; 


}
void FlashMyImporter::Import(FlashTagShowFrame &data) 
{
	std::cout << "\n<FlashTagShowFrame>\n";
}

void FlashMyImporter::Import(FlashTagBackgroundColor &data)
{
	std::cout << "\n<FlashTagBackgroundColor>\n";
	std::cout << "RGB: " << data.GetRGB().GetR() << ", " << data.GetRGB().GetG() << ", " << data.GetRGB().GetB() << "\n";
}
void FlashMyImporter::Import(FlashTagLabelFrame &data)
{
	std::cout << "\n<FlashTagLabelFrame>";
	if( data.IsNamedAnchor() )
		std::cout << "\t>> ANCHOR <<";
	std::cout << "\n";
	std::cout << "Label: " << data.GetLabel() << "\n";
}
void FlashMyImporter::Import(FlashTagProtect &data)
{
	std::cout << "\n<FlashTagProtect>\n";
	if(data.HasPassword())
	{
		std::cout << "Password (Encrypted): " << data.GetEncryptedPassword() << "\n";
	}
}
void FlashMyImporter::Import(FlashTagEnableDebugger &data)
{
	std::cout << "\n<FlashTagEnableDebugger>\n";
	
	if(data.HasPassword())
	{
		std::cout << "Password: " << data.GetEncryptedPassword() << "\n";
	}
}

void FlashMyImporter::Import(FlashTagDefineBitsPtr &data)
{
	std::cout << "\n<FlashTagDefineBitsPtr>\n";
	std::cout << "Ptr: " << data.GetPtr() << "\n";
	
}

void FlashMyImporter::Import(FlashTagEnd &data)
{
	std::cout << "\n<FlashTagEnd>\n";
}
void FlashMyImporter::Import(FlashTagExportAssets &data)
{
	std::cout << "\n<FlashTagExportAssets>\n";
	int num = data.GetAssetNum();
	for(int i=0; i < num; i++)
	{
		std::cout << "Asset " << i << " (name): " << data.GetAsset(i).first << "\n"; 
		std::cout << "Asset " << i << " (id):   " << data.GetAsset(i).second << "\n"; 
	}
	
}
void FlashMyImporter::Import(FlashTagImportAssets &data)
{
	std::cout << "\n<FlashTagImportAssets>\n";
	std::cout << "SWF Url: " << data.GetSwfUrl() << "\n";
	int num = data.GetAssetNum();
	for(int i=0; i < num; i++)
	{
		std::cout << "Asset " << i << " (name): " << data.GetAsset(i).first << "\n"; 
		std::cout << "Asset " << i << " (id):   " << data.GetAsset(i).second << "\n"; 
	}
}
void FlashMyImporter::Import(FlashTagDoAction &data)
{
	std::cout << "\n<FlashTagDoAction>\n";
	std::cout << "Num Actions: " << data.GetNumActions() << "\n";

}

void FlashMyImporter::Import(FlashTagDefineBitsJPEG1 &data)
{
	std::cout << "\n<FlashTagDefineBitsJPEG1>\n";
	std::cout << "ID:" << data.GetID() << "\n";
}
void FlashMyImporter::Import(FlashTagJPEGTables &data)
{
	std::cout << "\n<FlashTagJPEGTables>\n";
}
void FlashMyImporter::Import(FlashTagDefineBitsJPEG2 &data) 
{
	std::cout << "\n<FlashTagDefineBitsJPEG2>\n";
	std::cout << "ID:" << data.GetID() << "\n";

}
void FlashMyImporter::Import(FlashTagDefineBitsJPEG3 &data) 
{
	std::cout << "\n<FlashTagDefineBitsJPEG3>\n";
	std::cout << "ID:" << data.GetID() << "\n";

}
void FlashMyImporter::Import(FlashTagDefineBitsLossless &data) 
{
	std::cout << "\n<FlashTagDefineBitsLossless>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	std::cout << "Format: " << data.GetFormat() << "\n";
	std::cout << "Width: " << data.GetWidth() << " Height: " << data.GetHeight() << "\n";

}
void FlashMyImporter::Import(FlashTagDefineBitsLossless2 &data) 
{
	std::cout << "\n<FlashTagDefineBitsLossless2>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	std::cout << "Format: " << data.GetFormat() << "\n";
	std::cout << "Width: " << data.GetWidth() << " Height: " << data.GetHeight() << "\n";

}

void FlashMyImporter::Import(FlashTagDefineShape1 &data)
{
	std::cout << "\n<FlashTagDefineShape1>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	PrintRect(data.GetShapes().GetBounds());
	PrintRect(data.rimport);
}
void FlashMyImporter::Import(FlashTagDefineShape2 &data)
{
	std::cout << "\n<FlashTagDefineShape2>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	PrintRect(data.GetShapes().GetBounds());
	PrintRect(data.rimport);
}
void FlashMyImporter::Import(FlashTagDefineShape3 &data)
{
	std::cout << "\n<FlashTagDefineShape3>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	PrintRect(data.GetShapes().GetBounds());
	PrintRect(data.rimport);
}
void FlashMyImporter::Import( FlashTagDefineMorphShape &data )
{
	std::cout << "\n<FlashTagDefineMorphShape>\n";
	std::cout << "ID:" << data.GetID() << "\n";

// To be completed
}
void FlashMyImporter::Import(FlashTagFreeCharacter &data)
{
	std::cout << "\n<FreeCharacter>\n";
	std::cout << "ID:" << data.GetID() << "\n";
}

void FlashMyImporter::Import(FlashTagDefineSound &data)
{
	std::cout << "\n<FlashTagDefineSound>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	std::cout << "Flags:" << (int)data.GetFlags() << "\n";
	std::cout << "Sample Count:" << data.GetSampleCount() << "\n";
	std::cout << "Sample Length:" << data.GetSampleLength() << "\n";
}

void FlashMyImporter::Import(FlashTagDefineSoundMP3 &data)
{
	std::cout << "\n<FlashTagDefineSoundMP3>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	std::cout << "Flags:" << (int)data.GetFlags() << "\n";
	std::cout << "Sample Count:" << data.GetSampleCount() << "\n";
	std::cout << "Sample Length:" << data.GetSampleLength() << "\n";
}

void FlashMyImporter::Import(FlashTagStartSound &data)
{
	std::cout << "\n<FlashTagStartSound>\n";
	std::cout << "ID:" << data.GetID() << "\n";
	std::cout << "Sound Info:\n";
	std::cout << "  Flags: "  << (int)data.GetSoundInfo().GetFlags()   << "\n";
	
	if((data.GetSoundInfo().GetFlags() & FSI_HAS_IN) != 0) std::cout << "  In Point: "  << data.GetSoundInfo().GetInPoint()   << "\n";
	if((data.GetSoundInfo().GetFlags() & FSI_HAS_OUT) != 0) std::cout << "  Out Point: " << data.GetSoundInfo().GetOutPoint()  << "\n";
	if((data.GetSoundInfo().GetFlags() & FSI_HAS_LOOP) != 0) std::cout << "  Loop Count: " << data.GetSoundInfo().GetLoopCount() <<  "\n";
	if((data.GetSoundInfo().GetFlags() & FSI_HAS_ENVELOPE) != 0) std::cout << "  Sound Envelopes Found\n";
}
void FlashMyImporter::Import(FlashTagSoundStreamHead &data)
{
	std::cout << "\n<FlashTagSoundStreamHead>\n";
	std::cout << "Play Rate: " <<  (int)data.GetPlayRate() << "\n";
	std::cout << "Play 16 Bit: " << data.GetPlay16bit() << "\n";
	std::cout << "Play Stereo: " << data.GetPlayStereo() << "\n";
	std::cout << "Compression: " << (int)data.GetCompression() << "\n";
	std::cout << "Stream Rate: " << (int)data.GetStreamRate() << "\n";
	std::cout << "Stream 16 Bit: " << data.GetStream16bit() << "\n";
	std::cout << "Stream Stereo: " << data.GetStreamStereo() << "\n";
	std::cout << "Sample Count Avg.: " << data.GetSampleCountAvg() << "\n";

	
}
void FlashMyImporter::Import(FlashTagSoundStreamHead2 &data)
{
	std::cout << "\n<FlashTagSoundStreamHead2>\n";
	std::cout << "Play Rate: " << (int)data.GetPlayRate() << "\n";
	std::cout << "Play 16 Bit: " << data.GetPlay16bit() << "\n";
	std::cout << "Play Stereo: " << data.GetPlayStereo()<< "\n";
	std::cout << "Compression: " << (int)data.GetCompression() << "\n";
	std::cout << "Stream Rate: " << (int)data.GetStreamRate() << "\n";
	std::cout << "Stream 16 Bit: " << data.GetStream16bit() << "\n";
	std::cout << "Stream Stereo: " << data.GetStreamStereo() << "\n";
	std::cout << "Sample Count Avg.: " << data.GetSampleCountAvg() << "\n";

}
void FlashMyImporter::Import(FlashTagSoundStreamBlock &data)
{
	std::cout << "\n<FlashTagSoundStreamBlock>\n";
	std::cout << "Length:" << data.GetSampleLength() << "\n";
}

void FlashMyImporter::Import( FlashTagDefineButton &data )
{
	std::cout << "\n<FlashTagDefineButton>\n";

// To be completed
}

void FlashMyImporter::Import( FlashTagDefineButton2 &data )
{
	std::cout << "\n<FlashTagDefineButton2>\n";

// To be completed
}

void FlashMyImporter::Import( FlashTagDefineButtonSound &data )
{
	std::cout << "\n<FlashTagDefineButtonSound>\n";

// To be completed
}

void FlashMyImporter::Import( FlashTagDefineMovie &data )
{
	std::cout << "\n<FlashTagDefineMovie>\n";
	std::cout << "ID:" << data.GetID() << "\n";

	std::cout << "Movie: " << data.GetMovieName() << "\n";
}

void FlashMyImporter::Import( FlashTagSprite &data )
{
	std::cout << "\n<FlashTagSprite>\n";
	std::cout << "   ID:" << data.GetID() << ", Frame Count: " << data.GetFrameCount() << "\n";

	N_STD::vector<FlashSpriteEnabled*> &l_Tags = data.GetTags();
	for( N_STD::vector<FlashSpriteEnabled*>::iterator t = l_Tags.begin(); t != l_Tags.end(); t++ )
	{
		// call Base class to Import this sprite tag
		Import( *(*t) );
	}

	// VERY IMPORTANT NOTE: if "FlashMyImporter" wants to keep the SpriteTags for latter use, it HAS
	// to empty the SpriteTags list from FlashTagSprite without freeing the data right away.  Otherwise,
	// FlashSpriteImport handler (the caller) will take care of freeing the tags.
}

FlashHeader FlashImporter::ImportHeader(N_STD::istream &in, FlashImportHandler &i)
{
	FlashHeader h;
	in >> h;
	i.Import(h);
	return h;
}
 

#define IMPORT_MACRO(id, name)                              \
	if(tagHeader.GetTagID() == id)                           \
	{                                                        \
		name tmp;                                            \
		tmp.SetTagHeader(tagHeader.GetTagID(),tagHeader.GetTagLength());  \
		in >> tmp;                                           \
		i.Import(tmp);                                       \
} 

bool FlashImporter::ImportTag(N_STD::istream &in, FlashImportHandler &i)
{
	FlashTagHeader tagHeader;
	in >> tagHeader;
	
	N_STD::streampos start = in.tellg();

        IMPORT_MACRO(4,  FlashTagPlaceObject)
	else IMPORT_MACRO(26, FlashTagPlaceObject2)
	else IMPORT_MACRO(5,  FlashTagRemoveObject)
	else IMPORT_MACRO(28, FlashTagRemoveObject2)
	else IMPORT_MACRO(1,  FlashTagShowFrame)
	else IMPORT_MACRO(9,  FlashTagBackgroundColor)
	else IMPORT_MACRO(43, FlashTagLabelFrame)
	else IMPORT_MACRO(24, FlashTagProtect)
	else IMPORT_MACRO(0,  FlashTagEnd)
	else IMPORT_MACRO(56, FlashTagExportAssets)
	else IMPORT_MACRO(57, FlashTagImportAssets)
	else IMPORT_MACRO(8,  FlashTagJPEGTables)
	else IMPORT_MACRO(6,  FlashTagDefineBitsJPEG1)
	else IMPORT_MACRO(21, FlashTagDefineBitsJPEG2)
	else IMPORT_MACRO(35, FlashTagDefineBitsJPEG3)
	else IMPORT_MACRO(20, FlashTagDefineBitsLossless)
	else IMPORT_MACRO(36, FlashTagDefineBitsLossless2)

	else IMPORT_MACRO(2,  FlashTagDefineShape1)
	else IMPORT_MACRO(22, FlashTagDefineShape2)
	else IMPORT_MACRO(32, FlashTagDefineShape3)
	else IMPORT_MACRO(46, FlashTagDefineMorphShape)
	else IMPORT_MACRO(3,  FlashTagFreeCharacter)

	else if(tagHeader.GetTagID() == 14)
	{
		
		int c = in.get();
		if(c == EOF)
		{ 
			//throw
		}
		else
		{
			in.putback(c);
			if((c & FTDS_COMPRESS_MP3) != 0) 
			{
				IMPORT_MACRO(14, FlashTagDefineSoundMP3)
			}
			else
			{
				IMPORT_MACRO(14, FlashTagDefineSound)
			}
		}	
	}

	else IMPORT_MACRO(15, FlashTagStartSound)
	else IMPORT_MACRO(18, FlashTagSoundStreamHead)
	else IMPORT_MACRO(45, FlashTagSoundStreamHead2)
	else IMPORT_MACRO(19, FlashTagSoundStreamBlock)

	else IMPORT_MACRO(12, FlashTagDoAction)
	else IMPORT_MACRO(7,  FlashTagDefineButton)
	else IMPORT_MACRO(34, FlashTagDefineButton2)
	else IMPORT_MACRO(17, FlashTagDefineButtonSound)

	else IMPORT_MACRO(38, FlashTagDefineMovie)

	else if(tagHeader.GetTagID() == 39)    // FlashTagSprite
	{
		FlashTagSprite tmp;
		tmp.SetTagHeader(tagHeader.GetTagID(),tagHeader.GetTagLength());
		in >> tmp;

		// Read sub-tags, using the Alternate version of the Import Handler
		FlashSpriteImport Sprite_importer;
		while( ImportTag_AT( in, Sprite_importer ) );

		// Accumulated tags to be copied into FlashTagSprite
		N_STD::vector<FlashSpriteEnabled *>            tags = Sprite_importer.GetSpriteTagsList();
		N_STD::vector<FlashSpriteEnabled *>::iterator  t;
		for(t=tags.begin(); t != tags.end(); t++)
		{
			tmp.Add( (*t), false );
		}

		// Call the handler 
		i.Import(tmp);

		// Free the remaining tags in list (if Handler 'i' hasn't kept them for its own purposes)
		tags = Sprite_importer.GetSpriteTagsList();
		for(t=tags.begin(); t != tags.end(); t++)
		{
			delete (*t);
		}
	}

	else
	{
		for(UDWORD index=0; index < tagHeader.GetTagLength(); index++)
		{
			in.get();
		}
		i.Import(tagHeader);
	}
	
	if(tagHeader.GetTagLength() + start != (UDWORD)in.tellg())
	{
		if(tagHeader.GetTagLength() + start > (UDWORD)in.tellg())
		{
			UDWORD difference = (tagHeader.GetTagLength() + start) - (UDWORD)in.tellg();
			
			std::cout << "Extra Tag Data Encountered: " << difference << "\n";

			for(UDWORD index=0; index < difference; index++)
			{
				in.get();
			}
		}
		else
		{
			//ADD HANDLER???
			
			long difference = (long )in.tellg() - (long)(tagHeader.GetTagLength() + start);
			std::cout << "Tag Size Discrepancy: -" << difference << "\n";
			return false;
		}
	}
	if(tagHeader.GetTagID() == 0)
	{
		return false;
	}
	return true;
}

//*****************************************************************************

#define IMPORT_MACRO_AT(id, name)                           \
	if(tagHeader.GetTagID() == id)                           \
	{                                                        \
		name *tmp = new name();                               \
		tmp->SetTagHeader(tagHeader.GetTagID(),tagHeader.GetTagLength());    \
		in >> *tmp;                                           \
		i.Import(tmp);                                        \
	} 

bool FlashImporter::ImportTag_AT(N_STD::istream &in, FlashImportHandler &i)
{
	FlashTagHeader tagHeader;
	in >> tagHeader;
	
	N_STD::streampos start = in.tellg();

        IMPORT_MACRO_AT(4,  FlashTagPlaceObject)
	else IMPORT_MACRO_AT(26, FlashTagPlaceObject2)
	else IMPORT_MACRO_AT(5,  FlashTagRemoveObject)
	else IMPORT_MACRO_AT(28, FlashTagRemoveObject2)
	else IMPORT_MACRO_AT(1,  FlashTagShowFrame)
	else IMPORT_MACRO_AT(9,  FlashTagBackgroundColor)
	else IMPORT_MACRO_AT(43, FlashTagLabelFrame)
	else IMPORT_MACRO_AT(24, FlashTagProtect)
	else IMPORT_MACRO_AT(0,  FlashTagEnd)
	else IMPORT_MACRO_AT(56, FlashTagExportAssets)
	else IMPORT_MACRO_AT(57, FlashTagImportAssets)
	else IMPORT_MACRO_AT(8,  FlashTagJPEGTables)
	else IMPORT_MACRO_AT(6,  FlashTagDefineBitsJPEG1)
	else IMPORT_MACRO_AT(21, FlashTagDefineBitsJPEG2)
	else IMPORT_MACRO_AT(35, FlashTagDefineBitsJPEG3)
	else IMPORT_MACRO_AT(20, FlashTagDefineBitsLossless)
	else IMPORT_MACRO_AT(36, FlashTagDefineBitsLossless2)

	else IMPORT_MACRO_AT(2,  FlashTagDefineShape1)
	else IMPORT_MACRO_AT(22, FlashTagDefineShape2)
	else IMPORT_MACRO_AT(32, FlashTagDefineShape3)
	else IMPORT_MACRO_AT(46, FlashTagDefineMorphShape)
	else IMPORT_MACRO_AT(3,  FlashTagFreeCharacter)

	else if(tagHeader.GetTagID() == 14)
	{
		
		int c = in.get();
		if(c == EOF)
		{ 
			//throw
		}
		else
		{
			in.putback(c);
			if((c & FTDS_COMPRESS_MP3) != 0) 
			{
				IMPORT_MACRO_AT(14, FlashTagDefineSoundMP3)
			}
			else
			{
				IMPORT_MACRO_AT(14, FlashTagDefineSound)
			}
		}	
	}

	else IMPORT_MACRO_AT(15, FlashTagStartSound)
	else IMPORT_MACRO_AT(18, FlashTagSoundStreamHead)
	else IMPORT_MACRO_AT(45, FlashTagSoundStreamHead2)
	else IMPORT_MACRO_AT(19, FlashTagSoundStreamBlock)

	else IMPORT_MACRO_AT(12, FlashTagDoAction)
	else IMPORT_MACRO_AT(7,  FlashTagDefineButton)
	else IMPORT_MACRO_AT(34, FlashTagDefineButton2)
	else IMPORT_MACRO_AT(17, FlashTagDefineButtonSound)

	else IMPORT_MACRO_AT(38, FlashTagDefineMovie)

	else if(tagHeader.GetTagID() == 39)    // FlashTagSprite
	{
      FlashTagSprite *tmp = new FlashTagSprite();
		tmp->SetTagHeader(tagHeader.GetTagID(),tagHeader.GetTagLength());
		in >> *tmp;

		// Read sub-tags, using the Alternate version of the Import Handler
		FlashSpriteImport Sprite_importer;
		while( ImportTag_AT( in, Sprite_importer ) );

		// Accumulated tags to be copied into FlashTagSprite
		N_STD::vector<FlashSpriteEnabled *>            tags = Sprite_importer.GetSpriteTagsList();
		N_STD::vector<FlashSpriteEnabled *>::iterator  t;
		for(t=tags.begin(); t != tags.end(); t++)
		{
			tmp->Add( (*t), false );
		}

		// Call the handler 
		i.Import(tmp);

      // Free the remaining tags in list (if Handler 'i' hasn't kept them for its own purposes)
		tags = Sprite_importer.GetSpriteTagsList();
		for(t=tags.begin(); t != tags.end(); t++)
		{
			delete (*t);
		}
	}

	else
	{
		for(UDWORD index=0; index < tagHeader.GetTagLength(); index++)
		{
			in.get();
		}
		i.Import(tagHeader);
	}
	
	if(tagHeader.GetTagLength() + start != (UDWORD)in.tellg())
	{
		if(tagHeader.GetTagLength() + start > (UDWORD)in.tellg())
		{
			UDWORD difference = (tagHeader.GetTagLength() + start) - (UDWORD)in.tellg();
			
			std::cout << "Extra Tag Data Encountered: " << difference << "\n";

			for(UDWORD index=0; index < difference; index++)
			{
				in.get();
			}
		}
		else
		{
			//ADD HANDLER???
			
			long difference = (long )in.tellg() - (long)(tagHeader.GetTagLength() + start);
			std::cout << "Tag Size Discrepancy: -" << difference << "\n";
			return false;
		}
	}
	if(tagHeader.GetTagID() == 0)
	{
		return false;
	}
	return true;
}
