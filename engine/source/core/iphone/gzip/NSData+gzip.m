#import "NSData+gzip.h"
#import <zlib.h>

@implementation NSData (NSDataAdditions)

// ================================================================================================
//  FOUND HERE http://code.google.com/p/drop-osx/source/browse/trunk/Source/NSData%2Bgzip.m
//  Also Check http://deusty.blogspot.com/2007/07/gzip-compressiondecompression.html
// ================================================================================================

- (NSData *)gzipInflate
{
  if ([self length] == 0)
    return self;

  unsigned int full_length = [self length];
  unsigned int half_length = [self length] / 2;

  NSMutableData *decompressed = [NSMutableData dataWithLength: full_length + half_length];
  BOOL done = NO;
  int status;

  z_stream strm;
  strm.next_in   = (Bytef *)[self bytes];
  strm.avail_in  = [self length];
  strm.total_out = 0;
  strm.zalloc    = Z_NULL;
  strm.zfree     = Z_NULL;

  if (inflateInit2(&strm, (15+32)) != Z_OK)
    return nil;

  while (!done)
  {
    // Make sure we have enough room and reset the lengths.
    if (strm.total_out >= [decompressed length])
      [decompressed increaseLengthBy: half_length];
    strm.next_out  = [decompressed mutableBytes] + strm.total_out;
    strm.avail_out = [decompressed length]       - strm.total_out;
    // Inflate another chunk.
    status = inflate(&strm, Z_SYNC_FLUSH);
    if (Z_STREAM_END == status) 
      done = YES;
    else if (Z_OK != status)
      break;
  }

  if (Z_OK != inflateEnd(&strm))
    return nil;

  if (!done)
    return nil;

  // Set real length.
  [decompressed setLength: strm.total_out];
  return [NSData dataWithData: decompressed];
}

@end
