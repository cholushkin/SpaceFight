#import <QuartzCore/QuartzCore.h>
#import <mach/mach_time.h>

#if TARGET_OS_IPHONE
# import <UIKit/UIKit.h>
#else
# import <AppKit/AppKit.h>
#endif

NSString* pathForSaveFile()
{
#if TARGET_OS_IPHONE

  NSArray*  paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString* documentsDirectory = [paths objectAtIndex:0];
  return documentsDirectory;

#else // MAC OS

  NSString* folder = @"~/Library/Application Support/";
  NSString* bundleId = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleIdentifier"];

  folder = [folder stringByExpandingTildeInPath];
  folder = [folder stringByAppendingPathComponent: bundleId];

  NSFileManager* fileManager = [NSFileManager defaultManager];
  if ([fileManager fileExistsAtPath: folder] == NO)
    [fileManager createDirectoryAtPath: folder attributes: nil];

  return folder;

#endif // TARGET_OS_IPHONE
}

FILE* open_file(const char* szFileName, bool bRead, bool bIsResourceFile)
{
  if(bIsResourceFile)
  {
    if(!bRead)
    {
      //EALWAYS_ASSERT("Resources cannot be opened for writing");
      return NULL;
    }
    NSString* fullpath = [[NSString alloc] initWithCString: szFileName encoding:NSASCIIStringEncoding];
    NSString* nsFilename  = [fullpath stringByDeletingPathExtension];
    NSString* nsExtension = [fullpath pathExtension];
    NSString* path = [[NSBundle mainBundle] pathForResource: nsFilename ofType: nsExtension inDirectory: @"res"];
    FILE* f = fopen([path cStringUsingEncoding:1], "rb");
    return f;
  }
  else
  {
	NSString* documentsDirectory = pathForSaveFile();
    NSString* fullpath = [[NSString alloc] initWithCString: szFileName encoding:NSASCIIStringEncoding];
    NSString* path = [documentsDirectory stringByAppendingPathComponent:fullpath];
    FILE* f = fopen([path cStringUsingEncoding:1], bRead ? "rb" : "wb");
    if(f || bRead)
      return f;
    if(![[NSFileManager defaultManager] createFileAtPath:path contents:nil attributes:nil])
      return NULL;
    return fopen([path cStringUsingEncoding:1],"wb");
  }
  return NULL;
}

float GetTimeInSecSinceCPUStart()
{
  static unsigned int num   = 0;
  static unsigned int denom = 0;
  if (denom == 0) {
    struct mach_timebase_info tbi;
    kern_return_t r;
    r = mach_timebase_info(&tbi);
    assert(KERN_SUCCESS == r);
    num   = tbi.numer;
    denom = tbi.denom;
  }
  unsigned long now = mach_absolute_time();
  return (float)(now * (float)num / denom / NSEC_PER_SEC);
}

unsigned long long GetCPUCycles()
{
  return mach_absolute_time();
}

unsigned long long  GetCPUCyclesPerMSec()
{
  static unsigned long long num   = 0;
  static unsigned long long denom = 0;
  if (denom == 0) {
    struct mach_timebase_info tbi;
    kern_return_t r;
    r = mach_timebase_info(&tbi);
    assert(KERN_SUCCESS == r);
    num   = tbi.numer;
    denom = tbi.denom;
  }
  return denom * 1000000 / num;
}

const char* GetResourcePath(const char* szFileName) // $$$remove me ASAP
{
  // copy-pasta
  int len = strlen(szFileName);
  int dot = len - 1;
  while(dot > 0 && ('.' != szFileName[dot]))
    --dot;
  NSString *fullpath = [[NSString alloc] initWithCString: szFileName encoding:NSASCIIStringEncoding];
  NSString *nsExtension = [[fullpath substringFromIndex: (dot+1)] lowercaseString];
  NSString *nsPath   = [[fullpath substringToIndex: dot] stringByReplacingOccurrencesOfString:@"\\" withString:@"/"];
  NSString *filename = [nsPath lastPathComponent];
  NSString *nsDir = [@"res" stringByAppendingPathComponent:[nsPath stringByDeletingLastPathComponent]];
  NSString* path = [[NSBundle mainBundle] pathForResource:filename ofType: nsExtension inDirectory:nsDir];
  return nil != path ? [path cStringUsingEncoding:1] : NULL;
}

extern void OpenURL(const char* url)
{
#if TARGET_OS_IPHONE
  [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSASCIIStringEncoding]]];
#endif
}

bool GetLocale(char bytes[2])
{
  NSString *language = [[[NSLocale preferredLanguages] objectAtIndex:0] lowercaseString];
  const char* loc = [language cStringUsingEncoding:NSASCIIStringEncoding];
  if(NULL == loc)
    return false;
  bytes[0] = loc[0];
  bytes[1] = loc[1];
  return true;
}
