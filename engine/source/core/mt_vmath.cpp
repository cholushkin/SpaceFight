/*
 * vmath, set of classes for computer graphics mathemtics.
 * Copyright (c) 2005-2006, Jan Bartipan < barzto at gmail dot com >
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 * - Redistributions of source code must retain the above copyright 
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in 
 *   the documentation and/or other materials provided with the 
 *   distribution.
 * - Neither the names of its contributors may be used to endorse or 
 *   promote products derived from this software without specific 
 *   prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "core/math/mt_vmath.h"

NAMESPACE_BEGIN(mt)
template class Vector2<float>;
//template class Vector2<double>;
template class Vector3<float>;
//template class Vector3<double>;
template class Vector4<float>;
//template class Vector4<double>;
template class Matrix3<float>;
//template class Matrix3<double>;
template class Matrix4<float>;
//template class Matrix4<double>;
template class Quaternion<float>;
//template class Quaternion<double>;

template<>
const Matrix23<float> Matrix23<float>::_matrix23Identity(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

NAMESPACE_END(mt)