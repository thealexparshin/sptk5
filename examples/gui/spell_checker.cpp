/*
╔══════════════════════════════════════════════════════════════════════════════╗
║                       SIMPLY POWERFUL TOOLKIT (SPTK)                         ║
║                       spell_checker.cpp - description                        ║
╟──────────────────────────────────────────────────────────────────────────────╢
║  begin                Thursday May 25 2000                                   ║
║  copyright            (C) 1999-2018 by Alexey Parshin. All rights reserved.  ║
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

#include <cstdio>

#include <FL/fl_ask.H>
#include <sptk5/cgui>
#include <sptk5/gui/CEditorSpellChecker.h>

using namespace std;
using namespace sptk;

CEditor* editor;

void cb_spellCheck(Fl_Widget*, void*)
{
    CEditorSpellChecker sc(editor);
    try {
        sc.spellCheck();
    }
    catch (exception& e) {
        fl_alert("%s", e.what());
    }
}

int main(int argc, char* argv[])
{
    CThemes themes;
    CWindow window(400, 300, "CSpellChecker test");

    editor = new CEditor(10, SP_ALIGN_CLIENT);

    editor->textBuffer()->text("Mary has a little lemb, big botl of whiskie, and cucomber");

    CToolBar toolBar;
    CButton spellCheckButton("Spell Check", SP_ALIGN_LEFT);
    spellCheckButton.callback(cb_spellCheck);

    window.show();

    CThemes::set("OSX");

    window.relayout();

    Fl::run();

    return 0;
}
