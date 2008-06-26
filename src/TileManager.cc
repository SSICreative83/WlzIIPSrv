#if defined(__GNUC__)
#ident "MRC HGU $Id$"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id$"
#else
static char _TileManager_cc[] = "MRC HGU $Id$";
#endif
#endif

// Member functions for TileManager.h


/*  IIP Server: Tile Cache Handler

    Copyright (C) 2008 Zsolt Husz, Medical research Council, UK.
    Copyright (C) 2005-2006 Ruven Pillay.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/



#include "TileManager.h"


using namespace std;



RawTile TileManager::getNewTile( int resolution, int tile, int xangle, int yangle, CompressionType c ){

  if( loglevel >= 2 ) *logfile << "TileManager :: Cache Miss for resolution: " << resolution << ", tile: " << tile << endl
			       << "TileManager :: Cache Size: " << tileCache->getNumElements()
			       << " tiles, " << tileCache->getMemorySize() << " MB" << endl;


  RawTile ttt;
  int len = 0;

  // Get our raw tile
  ttt = image->getTile( xangle, yangle, resolution, tile );

  if( c == UNCOMPRESSED ){
    // Add to our tile cache
    if( loglevel >= 2 ) insert_timer.start();
    tileCache->insert( ttt );
    if( loglevel >= 2 ) *logfile << "TileManager :: Tile cache insertion time: " << insert_timer.getTime()
				 << " microseconds" << endl;
    return ttt;
  }


  /* We need to crop our edge tiles if they are not the full tile size
   */
  if( ((ttt.width != image->getTileWidth()) || (ttt.height != image->getTileHeight())) && (c == JPEG) ){
    this->crop( &ttt );
  }


  switch( c ){

  case JPEG:

    // Do our JPEG compression iff we have an 8 bit per channel image
    if( ttt.bpc == 8 ){
      if( loglevel >=2 ) compression_timer.start();
      len = jpeg->Compress( ttt );
      if( loglevel >= 2 ) *logfile << "TileManager :: JPEG Compression Time: "
				   << compression_timer.getTime() << " microseconds" << endl;
    }
    break;


  case DEFLATE:

    // No deflate for the time being ;-)
    if( loglevel >= 2 ) *logfile << "TileManager :: DEFLATE Compression requested: Not currently available" << endl;
    break;


  default:

    break;

  }


  // Add to our tile cache
  if( loglevel >= 2 ) insert_timer.start();
  tileCache->insert( ttt );
  if( loglevel >= 2 ) *logfile << "TileManager :: Tile cache insertion time: " << insert_timer.getTime()
			       << " microseconds" << endl;


  return ttt;

}



void TileManager::crop( RawTile *ttt ){

  //Wlz the data has already the correct length, crop is not needed
  if (ttt->dataLength == ttt->height * ttt->width * ttt->channels * ttt->bpc/8)
      return;

  int tw = image->getTileWidth();
  int th = image->getTileHeight();

  if( loglevel >= 3 ){
    *logfile << "TileManager :: Edge tile: Base size: " << tw << "x" << th
	     << ": This tile: " << ttt->width << "x" << ttt->height
	     << endl;
  }

  // Create a new buffer, fill it with the old data, then copy
  // back the cropped part into the RawTile buffer
  int len = tw * th * ttt->channels * ttt->bpc/8;
  unsigned char* buffer = (unsigned char*) malloc( len );
  unsigned char* src_ptr = (unsigned char*) memcpy( buffer, ttt->data, len );
  unsigned char* dst_ptr = (unsigned char*) ttt->data;

  // Copy one scanline at a time
  for( unsigned int i=0; i<ttt->height; i++ ){
    len =  ttt->width * ttt->channels * ttt->bpc/8;
    memcpy( dst_ptr, src_ptr, len );
    dst_ptr += len;
    src_ptr += tw * ttt->channels * ttt->bpc/8;
  }

  free( buffer );

  // Reset the data length
  len = ttt->width * ttt->height * ttt->channels * ttt->bpc/8;
  ttt->dataLength = len;

}




RawTile TileManager::getTile( int resolution, int tile, int xangle, int yangle, CompressionType c ){

  RawTile* rawtile = NULL;
  string tileCompression;
  string compName;


  // Time the tile retrieval
  if( loglevel >= 2 ) tile_timer.start();


  /* Try to get this tile from our cache first as a JPEG, then uncompressed
     Otherwise decode one from the source image and add it to the cache
   */
  switch( c )
    {

    case JPEG:
      *logfile << "Get 2: " << image->getHash() <<" res:" << resolution << ", tile: " << tile << endl;
      if( (rawtile = tileCache->getTile( image->getHash(), resolution, tile,
					  xangle, yangle, JPEG, jpeg->getQuality() )) ) break;
      if( (rawtile = tileCache->getTile( image->getHash(), resolution, tile,
					 xangle, yangle, DEFLATE, 0 )) ) break;
      if( (rawtile = tileCache->getTile( image->getHash(), resolution, tile,
					 xangle, yangle, UNCOMPRESSED, 0 )) ) break;
      break;


    case DEFLATE:

      if( (rawtile = tileCache->getTile( image->getHash(), resolution, tile,
					 xangle, yangle, DEFLATE, 0 )) ) break;
      if( (rawtile = tileCache->getTile( image->getHash(), resolution, tile,
					 xangle, yangle, UNCOMPRESSED, 0 )) ) break;
      break;


    case UNCOMPRESSED:

      if( (rawtile = tileCache->getTile( image->getHash(), resolution, tile,
					 xangle, yangle, UNCOMPRESSED, 0 )) ) break;
      break;


    default: 
      break;

    }


  // If we haven't been able to get a tile, get a raw one
  if( !rawtile ){
    RawTile newtile = this->getNewTile( resolution, tile, xangle, yangle, c );
    if( loglevel >= 2 ) *logfile << "TileManager :: Total Tile Access Time: "
				 << tile_timer.getTime() << " microseconds" << endl;
    return newtile;
  }


  // Define our compression names
  switch( rawtile->compressionType ){
    case JPEG: compName = "JPEG"; break;
    case DEFLATE: compName = "DEFLATE"; break;
    case UNCOMPRESSED: compName = "UNCOMPRESSED"; break;
    default: break;
  }

  if( loglevel >= 2 ) *logfile << "TileManager :: Cache Hit for resolution: " << resolution
			       << ", tile: " << tile
			       << ", compression: " << compName << endl
			       << "TileManager :: Cache Size: "
			       << tileCache->getNumElements() << " tiles, "
			       << tileCache->getMemorySize() << " MB" << endl;


  // Check whether the compression used for out tile matches our requested compression type.
  // If not, we must convert

  if( c == JPEG && rawtile->compressionType == UNCOMPRESSED ){

    // Rawtile is a pointer to the cache data, so we need to create a copy of it in case we compress it
    RawTile ttt( *rawtile );

    // Do our JPEG compression iff we have an 8 bit per channel image
    if( rawtile->bpc == 8 ){

      // Crop if this is an edge tile
      if( (ttt.width != image->getTileWidth()) || (ttt.height != image->getTileHeight()) ){
	this->crop( &ttt );
      }

      if( loglevel >=2 ) compression_timer.start();
      unsigned int oldlen = rawtile->dataLength;
      unsigned int newlen = jpeg->Compress( ttt );
      if( loglevel >= 2 ) *logfile << "TileManager :: JPEG requested, but UNCOMPRESSED compression found in cache." << endl
				   << "TileManager :: JPEG Compression Time: "
				   << compression_timer.getTime() << " microseconds" << endl
				   << "TileManager :: Compression Ratio: " << newlen << "/" << oldlen << " = "
				   << ( (float)newlen/(float)oldlen ) << endl;

      // Add our compressed tile to the cache
      if( loglevel >= 2 ) insert_timer.start();
      tileCache->insert( ttt );
      if( loglevel >= 2 ) *logfile << "TileManager :: Tile cache insertion time: " << insert_timer.getTime()
				   << " microseconds" << endl;

      if( loglevel >= 2 ) *logfile << "TileManager :: Total Tile Access Time: "
				   << tile_timer.getTime() << " microseconds" << endl;
      return RawTile( ttt );
    }
  }

  if( loglevel >= 2 ) *logfile << "TileManager :: Total Tile Access Time: "
			       << tile_timer.getTime() << " microseconds" << endl;

  return RawTile( *rawtile );


}
