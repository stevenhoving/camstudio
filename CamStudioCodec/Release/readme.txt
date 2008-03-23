CamStudio Lossless Codec v1.0
Copyright 2003 RenderSoft Software

* CamStudio Lossless Codec is a very fast codec optimized for screen capture applications.
* It operates in RGB mode and is able to compress 16, 24 or 32 bit RGB bitmaps.
* It supports temporal compression and is able to render the movies with perfect quality. 
* A application that work nicely this codec is the CamStudio Screen Recorder. This can be downloaded at
http://www.rendersoftware.com

* CamStudio codec is able to compress using two lossless compression algorithms : LZO and GZIP.
* The LZO algorithm is very fast and is best used for screen capturing.

* The GZIP (zlib) algorithm is most useful when you are converting / recompressing an existing AVI file with CamStudio codec.
The small file size produced with the GZIP (zlib) algorithm makes it ideal for archiving purposes.
When encoding with a third party application, you will have to enable / force keyframes to take advantage of this codec's temporal compression. 
Otherwise every frame will be stored as a key frame and the resulting file size will be very big.

* The source code for this codec is available at http://www.rendersoftware.com/products/camstudio/codecs.htm
and is released under the GNU General Public License, v2. The code is for Visual C++ 5.0 / 6.0.

* This codec is based on the LZO compression library by Markus Franz Xaver Johannes Oberhumer, 
zlib compression library by Jean-loup Gailly and Mark Adler, 
and the HUFFYUV codec by Ben Rudiak-Gould.

* Bugs and limitations :

This codec may display some artifacts when it is played with CamStudio Player 2.0/2.1 in some systems.

These artifacts do not appear when it is used with Windows Media Player or other well-known AVI players.



RenderSoft Software and Web Publishing specifically disclaims ALL warranties, express or implied, including but not limited to implied warranties of merchantability, fitness for a particular purpose, and non-infingement with respect to this software, its source code and any accompanying written materials.

IN NO EVENT will RenderSoft Software and Web Publishing be liable to you for damages, including any loss of profits, data, or other incidental or consequential damages arising out of your use or inability to use this software, even if RenderSoft Software and Web Publishing has been specifically advised on the possiblility of such damages.

By agreeing below, you indicate that you have read and understood the above licensing aggreement, and accept it as legally binding upon you.

If you choose not to accept any of the terms of this licensing agreement, please click the 'Exit' button now.


