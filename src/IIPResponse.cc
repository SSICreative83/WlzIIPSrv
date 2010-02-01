#if defined(__GNUC__)
#ident "MRC HGU $Id$"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id$"
#else
static char _IIPResponse_cc[] = "MRC HGU $Id$";
#endif
#endif

/*
    IIP Response Handler Class

    Copyright (C) 2008 Zsolt Husz, Medical research Council, UK.
    Copyright (C) 2003-2006 Ruven Pillay.

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

#include "IIPResponse.h"
#include <cstdio>
#include <string.h>


using namespace std;



IIPResponse::IIPResponse(){

  responseBody = "";
  error = "";
  protocol = "";
  mimeType = "Content-type: application/vnd.netfpx";
  eof = "\r\n";
  sent = false;
}


void IIPResponse::addResponse( const std::string& r ){

  responseBody.append( r );
  responseBody.append( eof );
}


void IIPResponse::addResponse( const char* c ){

  responseBody.append( c );
  responseBody.append( eof );
}


void IIPResponse::addResponse( const char* c, int a ){

  char tmp[64];
  snprintf( tmp, 64, "%s:%d", c, a );
  responseBody.append( tmp );
  responseBody.append( eof );
}


void IIPResponse::addResponse( std::string arg, const char* a ){

  /* We should convert these responses to UTF-8, but for now we won't bother
     as all metadata should be in ASCII anyway and I can't get iconv to work
     properly :-(
  */
  char tmp[64];
  snprintf( tmp, 64, "/%d:%s", strlen( a ), a );
  responseBody.append( arg );
  responseBody.append( tmp );
  responseBody.append( eof );

//   char tmp1[64];
//   char tmp2[64];
//   size_t inbuf, outbuf;
//   inbuf = outbuf = 0;
//   iconv_t ict = iconv_open( "UTF-8", "ASCII" );
//   iconv( ict, (char**) &a, &inbuf, (char**) &tmp1, &outbuf );
//   iconv_close( ict );
//   snprintf( tmp2, 64, "%s/%d:%ls", arg.c_str(), strlen( a ) * 2, (wchar_t*) tmp1 );
//   responseBody.append( tmp2 );
//   responseBody.append( eof );

}


void IIPResponse::addResponse( const char* c, int a, int b ){

  char tmp[64]; 
  snprintf( tmp, 64, "%s:%d %d", c, a, b );
  responseBody.append( tmp );
  responseBody.append( eof );
}


void IIPResponse::setError( const std::string& code, const std::string& arg ){

  char tmp[128];
  snprintf( tmp, 128, "Error/%d:%s %s", code.length() + arg.length() + 1, code.c_str(), arg.c_str() );
  error += tmp + eof;
}


string IIPResponse::formatResponse() {

  /* We always need 2 sets of eof after the MIME headers to stop apache from complaining
   */
  string response;
  if( error.length() ){
    response = mimeType + eof +
      "Content-disposition: inline;filename=\"IIPisAMadGameClosedToOurUnderstanding.netfpx\"" +
      eof + eof + error;
  }
  else{
    response = mimeType + eof + eof + protocol + eof + responseBody;
  }

  return response;
}



string IIPResponse::getAdvert( const std::string& version ){

  string advert = "Content-type: text/html" + eof;
  advert += "Content-disposition: inline;filename=\"iipsrv.html\"" + eof + eof;
  advert += "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"><html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" ><head><title>IIP Server</title><meta name=\"author\" content=\"Zsolt Husz &lt;Zsolt.Huszn@hgu.mrc.ac.uk&gt; Ruven Pillay &lt;ruven@users.sourceforge.net&gt;\"/></head><body style=\"font-family:Helvetica,sans-serif; margin:4em\"><center><h1>Woolz Internet Imaging Protocol Server</h1><h2>Version "
    + version +
    "</h2><br/><h3>Edinburgh Mouse Atlas: <a href=\"http://www.emouseatlas.org\">http://www.emouseatlas.org</a></h3> <br/><h4>by<br/>MRC, Human Genetics Unit, Zsolt Husz</h4> <h3> IIP Home Page: <a href=\"http://iipimage.sourceforge.net\">http://iipimage.sourceforge.net</a></h3><br/><h4>by<br/>Ruven Pillay</h4></center></body></html>";

  return advert;

}

void IIPResponse::addResponse( const char* c, double a, double b ){

  char tmp[64]; 
  snprintf( tmp, 64, "%s:%g %g", c, a, b );
  responseBody.append( tmp );
  responseBody.append( eof );
}

void IIPResponse::addResponse( const char* d, double a, double b, double c ){

  char tmp[128]; 
  snprintf( tmp, 128, "%s:%g %g %g", d, a, b, c );
  responseBody.append( tmp );
  responseBody.append( eof );
}

void IIPResponse::addResponse( const char* g, int a, int b, int c, int d, int e, int f  ){

  char tmp[128]; 
  snprintf( tmp, 128, "%s:%d %d %d %d %d %d", g, a, b, c, d, e, f);
  responseBody.append( tmp );
  responseBody.append( eof );
}
