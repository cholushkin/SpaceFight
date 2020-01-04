//
//  getCompressedAudioAsPcm.h
//  iPhone_OSX_Build
//
//  Created by Theodore Watson on 3/14/13.
//
//

#pragma once

bool getCompressedAudioAsPcm(const char* path, bool bMakeMono, void * &data, UInt32& bufferSize, UInt32& getSampleRate, UInt32& getAlFormat);