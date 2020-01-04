/*==================================================================================================
	SoundEngine.h
==================================================================================================*/
#if !defined(__SoundEngine_h__)
#define __SoundEngine_h__

//==================================================================================================
//	Includes
//==================================================================================================

//	System Includes
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioToolbox/AudioToolbox.h>

#if defined(__cplusplus)
extern "C"
{
#endif

//==================================================================================================
//	Sound Engine
//==================================================================================================
/*!
    @enum SoundEngine error codes
    @abstract   These are the error codes returned from the SoundEngine API.
    @constant   kSoundEngineErrUnitialized 
		The SoundEngine has not been initialized. Use SoundEngine_Initialize().
    @constant   kSoundEngineErrInvalidID 
		The specified EffectID was not found. This can occur if the effect has not been loaded, or
		if an unloaded is trying to be accessed.
    @constant   kSoundEngineErrFileNotFound 
		The specified file was not found.
    @constant   kSoundEngineErrInvalidFileFormat 
		The format of the file is invalid. Effect data must be little-endian 8 or 16 bit LPCM.
    @constant   kSoundEngineErrDeviceNotFound 
		The output device was not found.

*/
enum {
		kSoundEngineErrUnitialized			= 1,
		kSoundEngineErrInvalidID			= 2,
		kSoundEngineErrFileNotFound			= 3,
		kSoundEngineErrInvalidFileFormat	= 4,
		kSoundEngineErrDeviceNotFound		= 5
};


/*!
    @function       SoundEngine_Initialize
    @abstract       Initializes and sets up the sound engine. Calling after a previous initialize will
						reset the state of the SoundEngine, with all previous effects and music tracks
						erased. Note: This is not required, loading an effect or background track will 
						initialize as necessary.
    @param          inMixerOutputRate
                        A Float32 that represents the output sample rate of the mixer unit. Setting this to 
						0 will use the default rate (the sample rate of the device)
	@result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_Initialize(Float32 inMixerOutputRate);

/*!
    @function       SoundEngine_Teardown
    @abstract       Tearsdown the sound engine.
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_Teardown();

/*!
	@function		SoundEngine_Interrupt
	@abstract		Deselects and pauses the underlying OpenAL context.  Use this when the audio session
						has been interrupted.
	@result			A OSStatus indicating success or failure.
*/
OSStatus SoundEngine_Interrupt();

/*!
	@function		SoundEngine_Resume
	@abstract		Restores the underlying OpenAL context as the current context.  Use this to restore
						OpenAL sounds when the audio session is no longer interrupted.
	@result			A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_Resume();

/*!
    @function       SoundEngine_SetMasterVolume
    @abstract       Sets the overall volume of all sounds coming from the process
    @param          inValue
                        A Float32 that represents the level. The range is between 0.0 and 1.0 (inclusive).
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_SetMasterVolume(Float32 inValue);

/*!
    @function       SoundEngine_SetListenerPosition
    @abstract       Sets the position of the listener in the 3D space
    @param          inX
                        A Float32 that represents the listener's position along the X axis.
    @param          inY
                        A Float32 that represents the listener's position along the Y axis.
    @param          inZ
                        A Float32 that represents the listener's position along the Z axis.
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_SetListenerPosition(Float32 inX, Float32 inY, Float32 inZ);

/*!
    @function       SoundEngine_SetListenerGain
    @abstract       Sets the gain of the listener. Must be >= 0.0
    @param          inValue
                        A Float32 that represents the listener's gain
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_SetListenerGain(Float32 inValue);
	
/*!
    @function       SoundEngine_LoadLoopingEffect
    @abstract       Loads a sound effect from a file and return an ID to refer to that effect. Note: The files
						MUST all be in the same data format and sample rate
    @param          inLoopFilePath
                        The absolute path to the file to load. This is the file that will loop continually.
    @param          inAttackFilePath
                        The absolute path to the file to load. This will play once at the start of the effect.
						Set to NULL if no attack is desired, looping file will play immediately.
    @param          inDecayFilePath
                        The absolute path to the file to load. This will play once after looping has been ended. 
						Triggered using SoundEngine_StopEffect with the inDoDecay set to true. Set to NULL if no
						decay is desired, looping file will stop immediately. 
	@param			outEffectID
						A UInt32 ID that refers to the effect.
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_LoadLoopingEffect(const char* inLoopFilePath, const char* inAttackFilePath, const char* inDecayFilePath, UInt32* outEffectID);

/*!
    @function       SoundEngine_LoadEffect
    @abstract       Loads a sound effect from a file and return an ID to refer to that effect.
    @param          inPath
                        The absolute path to the file to load.
	@param			outEffectID
						A UInt32 ID that refers to the effect.
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_LoadEffect(const char* inPath, UInt32* outEffectID);

/*!
    @function       SoundEngine_UnloadEffect
    @abstract       Releases all resources associated with the given effect ID
    @param          inEffectID
                        The ID of the effect to unload.
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_UnloadEffect(UInt32 inEffectID);

/*!
    @function       SoundEngine_StartEffect
    @abstract       Starts playback of an effect
    @param          inEffectID
                        The ID of the effect to start.
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_StartEffect(UInt32 inEffectID, bool looped);

/*!
    @function       SoundEngine_StopEffect
    @abstract       Stops playback of an effect
    @param          inEffectID
                        The ID of the effect to stop.
    @param          inDoDecay
                        Whether to play the decay file or stop immmediately (this is ignored
						for non-looping effects)
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_StopEffect(UInt32 inEffectID, Boolean inDoDecay);

/*!
    @function       SoundEngine_Vibrate
    @abstract       Tells the device to vibrate
*/
#if TARGET_OS_IPHONE
	#define SoundEngine_Vibrate() AudioServicesPlaySystemSound(kSystemSoundID_Vibrate)
#endif

/*!
    @function       SoundEngine_SetEffectPitch
    @abstract       Applies pitch shifting to an effect
    @param          inEffectID
                        The ID of the effect to adjust
    @param          inValue
                        A Float32 that represents the pitch scalar, with 1.0 being unchanged. Must 
						be greater than 0.
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_SetEffectPitch(UInt32 inEffectID, Float32 inValue);

/*!
    @function       SoundEngine_SetEffectVolume
    @abstract       Sets the volume for an effect
    @param          inEffectID
                        The ID of the effect to adjust
    @param          inValue
                        A Float32 that represents the level. The range is between 0.0 and 1.0 (inclusive).
    @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_SetEffectLevel(UInt32 inEffectID, Float32 inValue);

/*!
    @function       SoundEngine_SetEffectPosition
    @abstract       Tells the engine whether a given effect should loop when played or if it should
					play through just once and stop.
    @param          inEffectID
                        The ID of the effect to adjust
    @param          inX
                        A Float32 that represents the effect's position along the X axis. Maximum distance
						is 100000.0 (absolute, not per axis), reference distance (distance from which gain 
						begins to attenuate) is 1.0
    @param          inY
                        A Float32 that represents the effect's position along the Y axis. Maximum distance
						is 100000.0 (absolute, not per axis), reference distance (distance from which gain 
						begins to attenuate) is 1.0
	@param          inZ
                        A Float32 that represents the effect's position along the Z axis. Maximum distance
						is 100000.0 by default (absolute, not per axis), reference distance (distance from 
						which gain begins to attenuate) is 1.0
	@result         A OSStatus indicating success or failure.
*/
OSStatus	SoundEngine_SetEffectPosition(UInt32 inEffectID, Float32 inX, Float32 inY, Float32 inZ);

/*!
   @function       SoundEngine_SetEffectsVolume
   @abstract       Sets the overall volume for the effects
   @param          inValue
                       A Float32 that represents the level. The range is between 0.0 and 1.0 (inclusive).
   @result         A OSStatus indicating success or failure.
*/
OSStatus  SoundEngine_SetEffectsVolume(Float32 inValue);

/*!
   @function       SoundEngine_SetMaxDistance
   @abstract       Sets the maximum distance for effect attenuation. Gain will attenuate between the
				   reference distance and the maximum distance, after which gain will be 0.0
   @param          inValue
                       A Float32 that represents the level. Must be greater than 0.0.
   @result         A OSStatus indicating success or failure.
*/
OSStatus	SoundEngine_SetMaxDistance(Float32 inValue);

/*!
   @function       SoundEngine_SetReferenceDistance
   @abstract       Sets the distance at which effect gain attenuation begins. It will attenuate between
				   the reference distance and the maximum distance. Sounds closer than the reference
				   distance will have no attenuation applied
   @param          inValue
                       A Float32 that represents the level. Must be greater than 0.0.
   @result         A OSStatus indicating success or failure.
*/
OSStatus	SoundEngine_SetReferenceDistance(Float32 inValue);

OSStatus SoundEngine_Pause(UInt32 inEffectID);
bool     SoundEngine_isPlaying(UInt32 inEffectID);

//////////////////////////////////////////////////////
// Music

OSStatus SoundEngine_LoadBackgroundMusicTrack(const char* inPath);
OSStatus SoundEngine_UnloadBackgroundMusicTrack();
OSStatus SoundEngine_StartBackgroundMusic(Boolean looped);
OSStatus SoundEngine_StopBackgroundMusic(Boolean inStopAtEnd);
OSStatus SoundEngine_SetBackgroundMusicVolume(Float32 inValue);
void SoundEngine_pauseBackgroundMusic();
void SoundEngine_resumeBackgroundMusic();
bool SoundEngine_hasTrackStopped(); 

#if defined(__cplusplus)
}
#endif

#endif
