#ifndef mt_base64_h__
#define mt_base64_h__

#include <string>
#include "core/common/com_misc.h"

NAMESPACE_BEGIN(mt)

std::string Base64Encode( unsigned char const* bytes_to_encode, u32 in_len );
i32 Base64Decode( const char* encoded_string, unsigned char* bytes_to_decode );

NAMESPACE_END(mt)

#endif // mt_base64_h__
