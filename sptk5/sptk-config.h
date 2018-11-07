/*
╔══════════════════════════════════════════════════════════════════════════════╗
║                       SIMPLY POWERFUL TOOLKIT (SPTK)                         ║
║                       sptk-config.h - description                            ║
╟──────────────────────────────────────────────────────────────────────────────╢
║  begin                Thursday May 25 2000                                   ║
║  copyright            (C) 1999-2016 by Alexey Parshin. All rights reserved.  ║
║  email                alexeyp@gmail.com                                      ║
╚══════════════════════════════════════════════════════════════════════════════╝
┌──────────────────────────────────────────────────────────────────────────────┐
│   This library is free software; you can redistribute it and/or modify it    │
│   under the terms of the GNU Library General Public License as published by  │
│   the Free Software Foundation; either version 2 of the License, or (at your │
│   option) any later version.                                                 │
│                                                                              │
│   This library is distributed in the hope that it will be useful, but        │
│   WITHOUT ANY WARRANTY; without even the implied warranty of                 │
│   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library   │
│   General Public License for more details.                                   │
│                                                                              │
│   You should have received a copy of the GNU Library General Public License  │
│   along with this library; if not, write to the Free Software Foundation,    │
│   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.               │
│                                                                              │
│   Please report all bugs and problems to alexeyp@gmail.com.                  │
└──────────────────────────────────────────────────────────────────────────────┘
*/

#ifndef __SPTK_CONFIG_H__
#define __SPTK_CONFIG_H__

#define VERSION          "5.2.15"
#define THEMES_PREFIX    "/usr/local"

/// @addtogroup utility Utility Classes
/// @{

#define HAVE_FLTK        1        ///< Flag indicating if FLTK library is installed
#define HAVE_ODBC        1        ///< Flag indicating if ODBC library is installed
#define HAVE_SQLITE3     1     ///< Flag indicating if SQLite3 library is installed
#define HAVE_POSTGRESQL  1  ///< Flag indicating if LibPQ (PostgreSQL client) library is installed
#define HAVE_FIREBIRD    1    ///< Flag indicating if Firebird client library is installed
#define HAVE_MYSQL       1       ///< Flag indicating if MySQL client library is installed
#define HAVE_ORACLE      1      ///< Flag indicating if Oracle client library is installed
#define HAVE_ASPELL      1      ///< Flag indicating if ASpell library is installed
#define HAVE_PCRE        1        ///< Flag indicating if PCRE library is installed
#define HAVE_OPENSSL     1     ///< Flag indicating if OpenSSL library is installed
#define HAVE_ZLIB        1        ///< Flag indicating if ZLib library is installed
#define WORDS_BIG_ENDIAN   ///< Flag indicating if byte order for mashine is big endian
#define BITNESS          64          ///< Number of machine bits

#define USE_NEW_ABI      1     ///< Flag if new ABI is used
  #define USE_GTEST      1       ///< Flag indicating if GTest library is installed and used

/// @}

#endif
