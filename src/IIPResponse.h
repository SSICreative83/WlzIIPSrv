#if defined(__GNUC__)
#ident "MRC HGU $Id$"
#else
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma ident "MRC HGU $Id$"
#else
static char _IIPResponse_h[] = "MRC HGU $Id$";
#endif
#endif

/*
    IIP Response Handler Class

    Copyright (C) 2008 Zsolt Husz, Medical research Council, UK.
    Copyright (C) 2003-2004 Ruven Pillay.

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


#ifndef _IIPRESPONSE_H
#define _IIPRESPONSE_H

#include <string>


/// Class to handle non-image IIP responses including errors

class IIPResponse{


 private:

  std::string mimeType;            // Mime type header
  std::string eof;                 // End of response delimitter eg "\r\n"
  std::string protocol;            // IIP protocol version
  std::string responseBody;        // The main response
  std::string error;               // Error message
  bool sent;                       // Indicate whether a response has been sent


 public:

  /// Constructor
  IIPResponse();


  /// Set the IIP protocol version
  /** \param p IIP protocol version */
  void setProtocol( const std::string& p ) { protocol = p; };


  /// Add a response string
  /** \param r response string */
  void addResponse( const std::string& r ); 


  /// Add a response string
  /** \param c response string */
  void addResponse( const char* c );


  /// Add a response string
  /** \param c response string
      \param a integer value
   */
  void addResponse( const char* c, int a );


  /// Add a response string
  /** \param c response string
      \param a string reply
   */
  void addResponse( std::string c, const char* a );


  /// Add a response string
  /** \param c response string
      \param a integer value
      \param b another integer value
   */
  void addResponse( const char* c, int a, int b );


  /// Set an error
  /** \param code error code
      \param arg the argument supplied by the client
   */
  void setError( const std::string& code, const std::string& arg );


  /// Get a formatted string to send back
  std::string formatResponse();


  /// Indicate whether this object has had any arguments passed to it
  bool isSet(){
    if( error.length() || responseBody.length() || protocol.length() ) return true;
    else return false;
  }


  /// Indicate whether we have an error message
  bool errorIsSet(){
    if( error.length() ) return true;
    else return false;
  }


  /// Set the sent flag indicating that some sort of response has been sent
  void setImageSent() { sent = true; };


  /// Indicate whether a response has been sent
  bool imageSent() { return sent; };


  /// Display our advertising banner ;-)
  /** \param version server version */
  std::string getAdvert( const std::string& version );


  /// Add a response string
  /** \param c response string
      \param a double value
      \param b another double value
   */
  void addResponse( const char* c, double a, double b );

  /// Add a response string
  /** \param d response string
      \param a first double value
      \param b second double value
      \param c thired double value
   */
  void addResponse( const char* d, double a, double b, double c );

};


#endif
