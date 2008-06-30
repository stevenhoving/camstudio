/*
 * File:	ximapng.cpp
 * Purpose:	Platform Independent PNG Image Class Loader and Writer
 * 07/Aug/2001 Davide Pizzolato - www.xdp.it
 * CxImage version 5.99c 17/Oct/2004
 */

#include "ximapng.h"

#if CXIMAGE_SUPPORT_PNG

#include "ximaiter.h"

////////////////////////////////////////////////////////////////////////////////
void CxImagePNG::ima_png_error(png_struct *png_ptr, char *message)
{
	strcpy(info.szLastError,message);
	longjmp(png_ptr->jmpbuf, 1);
}
////////////////////////////////////////////////////////////////////////////////
void CxImagePNG::expand2to4bpp(BYTE* prow)
{
	BYTE *psrc,*pdst;
	BYTE pos,idx;
	for(long x=head.biWidth-1;x>=0;x--){
		psrc = prow + ((2*x)>>3);
		pdst = prow + ((4*x)>>3);
		pos = (BYTE)(2*(3-x%4));
		idx = (BYTE)((*psrc & (0x03<<pos))>>pos);
		pos = (BYTE)(4*(1-x%2));
		*pdst &= ~(0x0F<<pos);
		*pdst |= (idx & 0x0F)<<pos;
	}
}
////////////////////////////////////////////////////////////////////////////////
bool CxImagePNG::Decode(CxFile *hFile)
{
	int number_passes;
	png_struct *png_ptr;
	png_info *info_ptr;
	BYTE *row_pointers=NULL;
	CImageIterator iter(this);

  try {
	/* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,(void *)NULL,NULL,NULL);
	if (png_ptr == NULL)  throw "Failed to create PNG structure";

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		throw "Failed to initialize PNG info structure";
	}

    /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
	if (setjmp(png_ptr->jmpbuf)) {
		/* Free all of the memory associated with the png_ptr and info_ptr */
		if (row_pointers) delete[] row_pointers;
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		throw "";
	}
	/* set up the input control */
	//png_init_io(png_ptr, hFile);

	// use custom I/O functions
    png_set_read_fn(png_ptr, hFile, (png_rw_ptr)user_read_data);
	png_set_error_fn(png_ptr,info.szLastError,(png_error_ptr)user_error_fn,NULL);

	/* read the file information */
	png_read_info(png_ptr, info_ptr);

	/* allocate the memory to hold the image using the fields of png_info. */
	png_color_16 my_background={ 0, 192, 192, 192, 0 };
	png_color_16 *image_background;

	if (info_ptr->pixel_depth != 32){
		//<yeonjun jeong> preserve original background info.
		if (png_get_bKGD(png_ptr, info_ptr, &image_background))
			png_set_background(png_ptr, image_background,PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
		else
			png_set_background(png_ptr, &my_background,PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
// <vho> - we call png_set_bgr() below
// <vho>	//<yeonjun jeong> safe check
// <vho>	if (info_ptr->pixel_depth > 16 ) info_ptr->color_type = COLORTYPE_COLOR;
	}

	//<DP> hack for images with alpha channel
	if (info_ptr->pixel_depth == 32){
//		info.nBkgndIndex = 0; //enable transparency
		if (png_get_bKGD(png_ptr, info_ptr, &image_background)){
			info.nBkgndColor.rgbRed   = (BYTE)image_background->red;
			info.nBkgndColor.rgbGreen = (BYTE)image_background->green;
			info.nBkgndColor.rgbBlue  = (BYTE)image_background->blue;
			info.nBkgndColor.rgbReserved = 0;	// <vho>
		}
	}

	/* tell libpng to strip 16 bit depth files down to 8 bits */
	if (info_ptr->bit_depth == 16)	png_set_strip_16(png_ptr);

	int pixel_depth=info_ptr->pixel_depth;
	if (pixel_depth >  16 ) pixel_depth=24;
	if (pixel_depth == 16 ) pixel_depth=8;

	Create(info_ptr->width, info_ptr->height, pixel_depth, CXIMAGE_FORMAT_PNG);

	/* get metrics */
	switch (info_ptr->phys_unit_type)
	{
	case PNG_RESOLUTION_UNKNOWN:
		SetXDPI(info_ptr->x_pixels_per_unit);
		SetYDPI(info_ptr->y_pixels_per_unit);
		break;
	case PNG_RESOLUTION_METER:
		SetXDPI((long)floor(info_ptr->x_pixels_per_unit * 254.0 / 10000.0 + 0.5));
		SetYDPI((long)floor(info_ptr->y_pixels_per_unit * 254.0 / 10000.0 + 0.5));
		break;
	}

	if (info_ptr->num_palette>0)
	  SetPalette((rgb_color*)info_ptr->palette,info_ptr->num_palette);
	else if (info_ptr->bit_depth ==2) { //<DP> needed for 2 bpp grayscale PNGs
		SetPaletteColor(0,0,0,0);
		SetPaletteColor(1,85,85,85);
		SetPaletteColor(2,170,170,170);
		SetPaletteColor(3,255,255,255);
	} else SetGrayPalette(); //<DP> needed for grayscale PNGs

	// simple transparency (the real PGN transparency is more complex)
	if (info_ptr->num_trans!=0){
		//palette transparency
		RGBQUAD* pal=GetPalette();
		if (pal){
			DWORD ip;
			for (ip=0;ip<min(head.biClrUsed,(unsigned long)info_ptr->num_trans);ip++)
				pal[ip].rgbReserved=info_ptr->trans[ip];
			if (info_ptr->num_trans==1 && pal[0].rgbReserved==0){
				info.nBkgndIndex = 0;
			} else {
				info.bAlphaPaletteEnabled=true;
				for (;ip<head.biClrUsed;ip++)
					pal[ip].rgbReserved=255;
			}
		}
	}

	if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA || //Alpha channel
		(info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA && info_ptr->pixel_depth == 32)){
		if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
			png_set_gray_to_rgb(png_ptr);
			png_set_expand(png_ptr);
		}
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
		AlphaCreate();
#else
		png_set_strip_alpha(png_ptr);
#endif	//CXIMAGE_SUPPORT_ALPHA
	}

	// <vho> - flip the RGB pixels to BGR (or RGBA to BGRA)
	if (info_ptr->color_type & PNG_COLOR_MASK_COLOR) png_set_bgr(png_ptr);

	// <vho> - handle cancel
	if (info.nEscape) longjmp(png_ptr->jmpbuf, 1);

	//allocate the buffer
	int row_stride = info_ptr->width * ((info_ptr->pixel_depth+7)>>3);
	row_pointers = new BYTE[10+row_stride];

	// turn on interlace handling
	number_passes = png_set_interlace_handling(png_ptr);

	if (number_passes>1){
		SetCodecOption(1);
	} else {
		SetCodecOption(0);
	}

	for (int pass=0; pass< number_passes; pass++) {
		iter.Upset();
		int y=0;
		do	{

			// <vho> - handle cancel
			if (info.nEscape) longjmp(png_ptr->jmpbuf, 1);

#if CXIMAGE_SUPPORT_ALPHA	// <vho>
			if (!AlphaIsValid())
#endif // CXIMAGE_SUPPORT_ALPHA
			{
				//recover data from previous scan
				if (info_ptr->interlace_type && pass>0)
					iter.GetRow(row_pointers, info.dwEffWidth);
				//read next row
				png_read_row(png_ptr, row_pointers, NULL);
// <vho> - already done by png_set_bgr()
// <vho>			//HACK BY OP && (<DP> for interlace, swap only in the last pass)
// <vho>			if (info_ptr->color_type==COLORTYPE_COLOR && pass==(number_passes-1))
// <vho>				RGBtoBGR(row_pointers, info.dwEffWidth);
				//<DP> expand 2 bpp images only in the last pass
				if (info_ptr->bit_depth==2 && pass==(number_passes-1))
					expand2to4bpp(row_pointers);
				//copy the pixels
				iter.SetRow(row_pointers, info.dwEffWidth);
				//go on
				iter.PrevRow();
			}
#if CXIMAGE_SUPPORT_ALPHA	// <vho>
			else { //alpha blend

				//compute the correct position of the line
				long ax,ay;
				ay = head.biHeight-1-y;
				BYTE* prow= iter.GetRow(ay);

				//recover data from previous scan
				if (info_ptr->interlace_type && pass>0 && pass!=7){
					for(ax=head.biWidth;ax>=0;ax--){
						row_pointers[ax*4]=prow[3*ax];
						row_pointers[ax*4+1]=prow[3*ax+1];
						row_pointers[ax*4+2]=prow[3*ax+2];
						row_pointers[ax*4+3]=AlphaGet(ax,ay);
					}
				}

				//read next row
				png_read_row(png_ptr, row_pointers, NULL);

				//RGBA -> RGB + A
				for(ax=0;ax<head.biWidth;ax++){
					prow[3*ax]=row_pointers[ax*4];
					prow[3*ax+1]=row_pointers[ax*4+1];
					prow[3*ax+2]=row_pointers[ax*4+2];
					AlphaSet(ax,ay,row_pointers[ax*4+3]);
				}
			}
#endif // CXIMAGE_SUPPORT_ALPHA		// vho
			y++;
		} while(y<head.biHeight);
	}
	delete[] row_pointers;

	/* read the rest of the file, getting any additional chunks in info_ptr */
	png_read_end(png_ptr, info_ptr);

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

  } catch (char *message) {
	if (strcmp(message,"")) strncpy(info.szLastError,message,255);
	return FALSE;
  }
	/* that's it */
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
bool CxImagePNG::Encode(CxFile *hFile)
{
	if (EncodeSafeCheck(hFile)) return false;

	CImageIterator iter(this);
	BYTE trans[256];	//for transparency (don't move)
	png_struct *png_ptr;
	png_info *info_ptr;

  try{
   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,(void *)NULL,NULL,NULL);
	if (png_ptr == NULL) throw "Failed to create PNG structure";

	/* Allocate/initialize the image information data.  REQUIRED */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL){
		png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
		throw "Failed to initialize PNG info structure";
	}

   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error hadnling functions in the png_create_write_struct() call.
    */
	if (setjmp(png_ptr->jmpbuf)){
		/* If we get here, we had a problem reading the file */
		if (info_ptr->palette) free(info_ptr->palette);
		png_destroy_write_struct(&png_ptr,  (png_infopp)&info_ptr);
		throw "Error saving PNG file";
	}
            
	int row_stride = info.dwEffWidth;
	/* set up the output control */
	//png_init_io(png_ptr, hFile);

	// use custom I/O functions
    png_set_write_fn(png_ptr,hFile,(png_rw_ptr)user_write_data,(png_flush_ptr)user_flush_data);

	/* set the file information here */
	info_ptr->width = GetWidth();
	info_ptr->height = GetHeight();
	info_ptr->pixel_depth = (BYTE)GetBpp();
	info_ptr->channels = (GetBpp()>8) ? (BYTE)3: (BYTE)1;
	info_ptr->bit_depth = (BYTE)(GetBpp()/info_ptr->channels);
	info_ptr->color_type = GetColorType();
	info_ptr->compression_type = info_ptr->filter_type = 0;
	info_ptr->valid = 0;
	info_ptr->rowbytes = row_stride;

	switch(GetCodecOption(CXIMAGE_FORMAT_PNG)){
	case 1:
		info_ptr->interlace_type = PNG_INTERLACE_ADAM7;
		break;
	default:
		info_ptr->interlace_type = PNG_INTERLACE_NONE;
	}

	/* set compression level */
	//png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	/* set background */
	png_color_16 image_background={ 0, 255, 255, 255, 0 };
	if (info.nBkgndIndex!=-1) {
		image_background.blue = info.nBkgndColor.rgbBlue;
		image_background.green = info.nBkgndColor.rgbGreen;
		image_background.red = info.nBkgndColor.rgbRed;
	}
	png_set_bKGD(png_ptr, info_ptr, &image_background);

	/* set metrics */
	png_set_pHYs(png_ptr, info_ptr, head.biXPelsPerMeter, head.biYPelsPerMeter, PNG_RESOLUTION_METER);

	/* set the palette if there is one */
	if (GetPalette()){
		png_set_IHDR(png_ptr, info_ptr, info_ptr->width, info_ptr->height, info_ptr->bit_depth, 
					PNG_COLOR_TYPE_PALETTE, info_ptr->interlace_type, 
					PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		info_ptr->valid |= PNG_INFO_PLTE;

		//<DP> simple transparency
		if (info.nBkgndIndex != -1){
			trans[0]=0;
			info_ptr->num_trans = 1;
			info_ptr->valid |= PNG_INFO_tRNS;
			info_ptr->trans = trans;
			// the transparency indexes start from 0
			if (info.nBkgndIndex){
				SwapIndex(0,(BYTE)info.nBkgndIndex);
				// the ghost must set the changed attributes in the body
				if (info.pGhost) info.pGhost->SetTransIndex(0);
			}
		}

		int nc = GetNumColors();

		/* We not need to write unused colors! <Basara>*/
		/* only for small images <DP>*/
		if ((nc>2)&&((head.biWidth*head.biHeight)<65536)){
			nc = 0;
			for (DWORD y=0;y<GetHeight();y++){
				for (DWORD x=0;x<GetWidth();x++){
					if (GetPixelIndex(x,y)>nc){
						nc=GetPixelIndex(x,y);
					}
				}
			}
			nc++;
		}

		if (info.bAlphaPaletteEnabled){
			for(WORD ip=0; ip<nc;ip++)
				trans[ip]=GetPaletteColor((BYTE)ip).rgbReserved;
			info_ptr->num_trans = (WORD)nc;
			info_ptr->valid |= PNG_INFO_tRNS;
			info_ptr->trans = trans;
		}

		// copy the palette colors
		info_ptr->palette = new png_color[nc];
		info_ptr->num_palette = (png_uint_16) nc;
		for (int i=0; i<nc; i++)
			GetPaletteColor(i, &info_ptr->palette[i].red, &info_ptr->palette[i].green, &info_ptr->palette[i].blue);

	}  

#if CXIMAGE_SUPPORT_ALPHA	// <vho>
	//Merge the transparent color with the alpha channel
	bool bNeedTempAlpha = false;
	if (head.biBitCount==24 && info.nBkgndIndex>=0){
		if (!AlphaIsValid()){
			bNeedTempAlpha = true;
			AlphaCreate();
		}
		RGBQUAD c,ct=GetTransColor();
		for(long y=0; y < head.biHeight; y++){
			for(long x=0; x < head.biWidth ; x++){
				c=GetPixelColor(x,y,false);
				if (*(long*)&c==*(long*)&ct)
					AlphaSet(x,y,0);
		}}
	}
#endif // CXIMAGE_SUPPORT_ALPHA	// <vho>

#if CXIMAGE_SUPPORT_ALPHA	// <vho>
	if (AlphaIsValid()){
		row_stride = 4 * head.biWidth;

		info_ptr->pixel_depth = 32;
		info_ptr->channels = 4;
		info_ptr->bit_depth = 8;
		info_ptr->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		info_ptr->rowbytes = row_stride;

		/* write the file information */
		png_write_info(png_ptr, info_ptr);
		
		//<Ranger> "10+row_stride" fix heap deallocation problem during debug???
		BYTE *row_pointers = new BYTE[10+row_stride];

		//interlace handling
		int num_pass = png_set_interlace_handling(png_ptr);
		for (int pass = 0; pass < num_pass; pass++){

			//write image
  			iter.Upset();
			long ay=head.biHeight-1;
			RGBQUAD c;
			do	{
				for (long ax=head.biWidth-1; ax>=0;ax--){
					c=GetPixelColor(ax,ay);
					row_pointers[ax*4+3]=(BYTE)((AlphaGet(ax,ay)*info.nAlphaMax)/255);
					row_pointers[ax*4+2]=c.rgbBlue;
					row_pointers[ax*4+1]=c.rgbGreen;
					row_pointers[ax*4]=c.rgbRed;
				}
				png_write_row(png_ptr, row_pointers);
				ay--;
			} while(iter.PrevRow());
		}
		
		delete [] row_pointers;
	}
	else
#endif //CXIMAGE_SUPPORT_ALPHA	// <vho>
	{
		/* write the file information */
		png_write_info(png_ptr, info_ptr);
		/* If you are only writing one row at a time, this works */
		BYTE *row_pointers = new BYTE[10+row_stride];
 
		//interlace handling
		int num_pass = png_set_interlace_handling(png_ptr);
		for (int pass = 0; pass < num_pass; pass++){
			
			//write image
			iter.Upset();
			do	{
				iter.GetRow(row_pointers, row_stride);
				//HACK BY OP
				if (info_ptr->color_type == 2 /*COLORTYPE_COLOR*/)
					RGBtoBGR(row_pointers, row_stride);
				png_write_row(png_ptr, row_pointers);
			} while(iter.PrevRow());

		}
		
		delete [] row_pointers;
	}

#if CXIMAGE_SUPPORT_ALPHA	// <vho>
	/* remove the temporary alpha channel*/
	if (bNeedTempAlpha) AlphaDelete();
#endif // CXIMAGE_SUPPORT_ALPHA	// <vho>

	/* It is REQUIRED to call this to finish writing the rest of the file */
	png_write_end(png_ptr, info_ptr);

	/* if you malloced the palette, free it here */
	if (info_ptr->palette)	delete[] (info_ptr->palette);

	/* clean up after the write, and free any memory allocated */
	png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);

  } catch (char *message) {
	strncpy(info.szLastError,message,255);
	return FALSE;
  }
	/* that's it */
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_PNG
