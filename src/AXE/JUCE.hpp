//
// Created by vince on 9/20/25.
//

#ifndef SHADOW_NATIVE_AXE_JUCE_HPP
#define SHADOW_NATIVE_AXE_JUCE_HPP

#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED

// #include "juce_build_tools/juce_build_tools.h"
#include "juce_core/juce_core.h"
#include "juce_cryptography/juce_cryptography.h"
#include "juce_data_structures/juce_data_structures.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_utils/juce_audio_utils.h"

// #include "BinaryData.h"

// #if defined (JUCE_PROJUCER_VERSION) && JUCE_PROJUCER_VERSION < JUCE_VERSION
//  /** If you've hit this error then the version of the Projucer that was used to generate this project is
// 	 older than the version of the JUCE modules being included. To fix this error, re-save your project
// 	 using the latest version of the Projucer or, if you aren't using the Projucer to manage your project,
// 	 remove the JUCE_PROJUCER_VERSION define.
//  */
//  #error "This project was last saved using an outdated version of the Projucer! Re-save this project with the latest version to fix this error."
// #endif

// #if ! DONT_SET_USING_JUCE_NAMESPACE
//  // If your code uses a lot of JUCE classes, then this will obviously save you
//  // a lot of typing, but can be disabled by setting DONT_SET_USING_JUCE_NAMESPACE.
//  using namespace juce;
// #endif

// #if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
const char* const  projectName    = "Projucer";
const char* const  companyName    = "Raw Material Software Limited";
const char* const  versionString  = "8.0.10";
const int          versionNumber  = 0x8000a;
}
// #endif

#endif // SHADOW_NATIVE_AXE_JUCE_HPP
