#ifndef __PRECOMP_H__
#define __PRECOMP_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
	#include "wx/wx.h"
	#include "wx/artprov.h"
	#include "wx/image.h"
	#include "wx/imaglist.h"
	#include "wx/cshelp.h"
	#include "wx/utils.h"
	#include "wx/mstream.h"
	#include "wx/toolbar.h"
	#include "wx/file.h"
	#include "wx/collpane.h"
	#include "wx/clrpicker.h"
	#include "wx/filepicker.h"
	#include "wx/fontpicker.h"
	#include "wx/filedlg.h"
	#include "wx/toolbook.h"
	#include "wx/panel.h"
	#include "wx/thread.h"
#endif //precompiled headers

#include "wx/dcbuffer.h"
#include "wx/fileconf.h"
#include "wx/settings.h"
#include "wx/sysopt.h"
#include "wx/listctrl.h"
#include "wx/notebook.h"
#include "wx/panel.h"
#include "wx/caret.h"
#include "wx/confbase.h"
#include "wx/tokenzr.h"
#include "wx/gdicmn.h"
#include "wx/image.h"
#include "wx/dataobj.h"
#include "wx/config.h"
#include "wx/dir.h"
#include "wx/textfile.h"
#include "wx/filename.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/wfstream.h"
#include "wx/aui/aui.h"
#include "wx/artprov.h"
#include "wx/filename.h"

#ifdef __WXGTK__
#   include "clipbrd_gtk.h"
#else
#   include <wx/clipbrd.h>
#endif

// C CRT includes
#include <stdio.h>

// C++ CRT includes
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <boost/smart_ptr.hpp>

// STL includes
#include <functional>
#include <vector>
#include <map>
#include <algorithm>

#include <stdint.h>

#include "resource.h"

#endif // __PRECOMP_H__