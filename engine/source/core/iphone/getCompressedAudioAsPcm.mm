//
//  getCompressedAudioAsPcm.cpp
//  iPhone_OSX_Build
//
//  Created by Theodore Watson on 3/14/13.
//
//

//This conversion function was pulled from: https://github.com/kstenerud/ObjectAL-for-iPhone
//by Karl Stenerud


//  Copyright (c) 2009 Karl Stenerud. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Attribution is not required, but appreciated :)
//



#import <Foundation/Foundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CoreAudio/CoreAudioTypes.h>

#include "getCompressedAudioAsPcm.h"

bool getCompressedAudioAsPcm(NSURL * urlIn, bool reduceToMono, void * &streamData, UInt32 * bufferSize, UInt32 * sampleRate, UInt32 * format){
  
  NSURL * url = urlIn;
  OSStatus error;
  UInt32 size;
  
  /** A description of the audio data in this file. */
  AudioStreamBasicDescription streamDescription;
  
  /** The OS specific file handle */
  ExtAudioFileRef fileHandle;
  
  SInt64 totalFrames;
  
  /** The actual number of channels in the audio data if not reducing to mono */
  UInt32 originalChannelsPerFrame;
		
		if(nil == url){
      NSLog(@"Cannot open NULL file / url");
      return false;
    }
  
		// Open the file
		if(noErr != (error = ExtAudioFileOpenURL((__bridge CFURLRef)url, &fileHandle))){
      NSLog(@"Could not open url %@", url);
      return false;
    }
  
		// Get some info about the file
		size = sizeof(SInt64);
		if(noErr != (error = ExtAudioFileGetProperty(fileHandle,
                                                 kExtAudioFileProperty_FileLengthFrames,
                                                 &size,
                                                 &totalFrames)))
    {
      NSLog(@"Could not get frame count for file (url = %@)", url);
      return false;
    }
		
		
		size = sizeof(AudioStreamBasicDescription);
		if(noErr != (error = ExtAudioFileGetProperty(fileHandle,
                                                 kExtAudioFileProperty_FileDataFormat,
                                                 &size,
                                                 &streamDescription)))
    {
      NSLog(@"Could not get audio format for file (url = %@)", url);
      return false;
    }
		
		// Specify the new audio format (anything not changed remains the same)
		streamDescription.mFormatID = kAudioFormatLinearPCM;
		streamDescription.mFormatFlags = kAudioFormatFlagsNativeEndian |
		kAudioFormatFlagIsSignedInteger |
		kAudioFormatFlagIsPacked;
		// Force to 16 bit since iOS doesn't seem to like 8 bit.
		streamDescription.mBitsPerChannel = 16;
  
		originalChannelsPerFrame = streamDescription.mChannelsPerFrame > 2 ? 2 : streamDescription.mChannelsPerFrame;
		if(reduceToMono)
    {
      streamDescription.mChannelsPerFrame = 1;
    }
		
		if(streamDescription.mChannelsPerFrame > 2)
    {
      // Don't allow more than 2 channels (stereo)
      NSLog(@"Audio stream in %@ contains %d channels. Capping at 2",
            url,
            streamDescription.mChannelsPerFrame);
      streamDescription.mChannelsPerFrame = 2;
    }
  
		streamDescription.mBytesPerFrame = streamDescription.mChannelsPerFrame * streamDescription.mBitsPerChannel / 8;
		streamDescription.mFramesPerPacket = 1;
		streamDescription.mBytesPerPacket = streamDescription.mBytesPerFrame * streamDescription.mFramesPerPacket;
		
		// Set the new audio format
		if(noErr != (error = ExtAudioFileSetProperty(fileHandle,
                                                 kExtAudioFileProperty_ClientDataFormat,
                                                 sizeof(AudioStreamBasicDescription),
                                                 &streamDescription)))
    {
      NSLog(@"Could not set new audio format for file (url = %@)", url);
      return false;
    }
		
  
  //-section 2
  
  
  if(nil == fileHandle){
    NSLog(@"Attempted to read from closed file. Returning nil (url = %@)", url);
    return nil;
		}
		
		UInt32 numFramesRead;
  AudioBufferList bufferList;
  UInt32 bufferOffset = 0;
  UInt32 startFrame = 0;
  
  UInt32 numFrames = totalFrames - startFrame;
		
		// Allocate some memory to hold the data
		UInt32 streamSizeInBytes = (UInt32)(streamDescription.mBytesPerFrame * numFrames);
		streamData = malloc(streamSizeInBytes);
  
		if(nil == streamData)
    {
      NSLog(@"Could not allocate %d bytes for audio buffer from file (url = %@)",
            streamSizeInBytes,
            url);
      goto onFail;
    }
		
		if(noErr != (error = ExtAudioFileSeek(fileHandle, startFrame)))
    {
      NSLog(@"Could not seek to %ll in file (url = %@)",
            startFrame,
            url);
      goto onFail;
    }
		
  {
    *sampleRate = streamDescription.mSampleRate;
    int whichFormat = AL_FORMAT_STEREO16;
    if( streamDescription.mChannelsPerFrame == 1){
      whichFormat = AL_FORMAT_MONO16;
    }
    *format = whichFormat;
  }
  
  bufferList.mNumberBuffers = 1;
  bufferList.mBuffers[0].mNumberChannels = streamDescription.mChannelsPerFrame;
  for(UInt32 framesToRead = (UInt32) numFrames; framesToRead > 0; framesToRead -= numFramesRead)
  {
    bufferList.mBuffers[0].mDataByteSize = streamDescription.mBytesPerFrame * framesToRead;
    bufferList.mBuffers[0].mData = (char*)streamData + bufferOffset;
    
    numFramesRead = framesToRead;
    if(noErr != (error = ExtAudioFileRead(fileHandle, &numFramesRead, &bufferList)))
    {
      NSLog(@"Could not read audio data in file (url = %@)",
            url);
      goto onFail;
    }
    bufferOffset += streamDescription.mBytesPerFrame * numFramesRead;
    if(numFramesRead == 0)
    {
      // Sometimes the stream description was wrong and you hit an EOF prematurely
      break;
    }
  }
		
  // Use however many bytes were actually read
  *bufferSize = bufferOffset;
		
  return true;
		
onFail:
		if(nil != streamData)
    {
      free(streamData);
      streamData = nil;
    }
		return false;
  
  //end section 2
  
}

bool getCompressedAudioAsPcm(const char* path, bool bMakeMono, void*& data, UInt32& bufferSize, UInt32& getSampleRate, UInt32& getAlFormat){
  NSString *str = [[NSString alloc] initWithCString:path encoding:NSASCIIStringEncoding];
  NSURL *url    = [NSURL fileURLWithPath:str];
  return getCompressedAudioAsPcm(url, bMakeMono, data, (UInt32 *)&bufferSize, &getSampleRate, &getAlFormat);
}

